#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

// 추가 : ranking.c 함수 이용
#include "ranking.h"

#define LEFT 50
#define HEIGHT 5

#define CHANGE_KEY 'q' // 쉬는시간이랑 공부시간 바꾸는 키
#define EXIT_KEY 'e' // 공부 조기종료하는 키
#define STORAGE "users/" // 저장소 디랙토리

int time_to_study; // 입력받은 공부시간(시)
int realStudySecond; // 실제로 한 공부시간(초)

time_t t;
struct tm tm;

// 추가 : 랭킹 파일 경로
#define RANKING_FILE "ranking.txt"

typedef struct member
{
    int rank;
    char name[NAME_MAX];
    int total_time;
    int duration;
} member;

// 추가 : 전역 멤버 구조체로 현재 로그인된 사용자 이름 관리
member current_user;

void tty_mode(int);
void turnOffEchoAndIcanon();
void restoreSettings();
void forceRestoreEcho(); // 강제로 ECHO 보용
int find_filename(char *);
void printMenuUI();
void registerUser();
void loginUser();
void loginSuccessMenu();
int getTargetTime();
void showAllRankings();
void showMyRecord();
void viewRecordMenu();
void doStudy();
void showDetail(const char*);
void printMainMenu();
void printStudyUI();


void *change_to_restTime(void *);
void endStudy();

int main()
{
    int flag = 0; // 프로그램 종료 플래그

    // 기존 터미널 설정 백업 및 종료 시 보원 등록
    tty_mode(0);
    atexit(forceRestoreEcho); // 종료 시 강제 보원 등록

    // 시작 시 ECHO, ICANON 비트 끄기
    turnOffEchoAndIcanon();

    initscr(); // curses 초기화

    load_users(RANKING_FILE);

    while (!flag)
    {
        clear();
        printMenuUI();
        refresh();

        switch (getch())
        {
        case 'r':
        case 'R':
            erase();
            refresh();
            registerUser();
            break;

        case 'j':
        case 'J':
            erase();
            refresh();
            loginUser();
            break;

        case 'q':
        case 'Q':
            flag = 1;
            break;

        default:
            break;
        }
    }

    echo();
    nodelay(stdscr, FALSE);

    tty_mode(1);
    endwin(); // curses 종료
    printf("프로그램 종료\n");
    return 0;
}

void forceRestoreEcho()
{
    struct termios info;
    tcgetattr(0, &info);          // 현재 설정 가져오기
    info.c_lflag |= ECHO;         // ECHO 켜기
    info.c_lflag |= ICANON;       // ICANON 켜기
    tcsetattr(0, TCSANOW, &info); // 설정 적용
}

void registerUser()
{
    FILE* fptr;

    clear();
    refresh();

    // 터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char *title = "<< Register >> ";
    char *m1 = "set your NAME: ";
    char *m2 = " was registered!";
    char *m3 = " is already registered.";
    char name[NAME_MAX];

    // 박스 크기 설정
    int boxheight = 11;
    int boxwidth = 30;

    // 중앙 좌표 계산
    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    // 새로운 WINDOW 생성
    WINDOW *win_1 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_1, '|', '-');

    WINDOW *win_2 = newwin(3, boxwidth, starty + 12, startx);

    // Title
    mvprintw(starty - 1, width / 2 - strlen(title) / 2, "%s", title);
    refresh();
    wrefresh(win_1);

    // 등록창
    mvwprintw(win_1, boxheight / 2, boxwidth / 2 - strlen(m1) / 2 - 3, "%s", m1);
    wrefresh(win_1);
    echo(); // 사용자 입력 허용
    mvwgetstr(win_1, boxheight / 2, boxwidth / 2 - strlen(m1) / 2 + strlen(m1) - 3, name);
    noecho(); // 사용자 입력 다시 비활성화

    // 이름 중복 확인 및 결과 표시
    box(win_2, '|', '-');
    if (find_filename(name)) // 이미 있는 이름
    {
        mvwprintw(win_2, 1, boxwidth / 2 - strlen(m3) / 2 - strlen(name) / 2, "[%s]%s", name, m3);
        wrefresh(win_2);
    }
    else // 등록 진행
    {
        char ext_name[NAME_MAX + 10]; // 확장자 붙이기
        chdir("users");

        sprintf(ext_name, "%s.txt", name);
        creat(ext_name, 0777); // 사용자 파일 생성

        // 기본값 입력
        FILE *ii = fopen(ext_name, "w");
        fprintf(ii, "0 0\n");
        fclose(ii);

        chdir("..");
        mvwprintw(win_2, 1, boxwidth / 2 - strlen(m2) / 2 - strlen(name) / 2, "[%s]%s", name, m2);
        wrefresh(win_2);

        // ranking 파일에 해당 유저 추가
        add_or_update_user(name, 0, 0);
        save_users("ranking.txt");

    }

    getch(); // 사용자 입력 대기

    wclear(win_1);
    wclear(win_2);
    wrefresh(win_1);
    wrefresh(win_2);

    delwin(win_1);
    delwin(win_2);

}

void loginUser()
{
    // 터미널 크기 가져오기
    clear();
    refresh();

    cbreak();

    int height, width;
    getmaxyx(stdscr, height, width);

    char *title = "<< Login >> ";
    char *m = "Type your NAME: ";
    char *success = ">> connected!";
    char *fail = "does not exist.";
    char *question = "Sign up? (Y/N)";
    char name[NAME_MAX];

    // 박스 크기 설정
    int boxheight = 11;
    int boxwidth = 30;

    // 중앙 좌표 계산
    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    // 새로운 WINDOW 생성
    WINDOW *win_1 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_1, '|', '-'); // 박스 그리기
    wrefresh(win_1);

    WINDOW *win_2 = newwin(3, boxwidth, starty + 12, startx);

    // Title
    mvprintw(starty - 1, width / 2 - strlen(title) / 2, "%s", title);
    refresh();

    // 입력받기전에 echo() 켜기
    echo();

    // 로그인 창
    mvwprintw(win_1, boxheight / 2, boxwidth / 2 - strlen(m) / 2 - 3, "%s", m);
    wscanw(win_1, "%s", name);

    refresh();
    wrefresh(win_1);

    // 이름 확인 및 결과 표시
    if (find_filename(name))
    {

        // 추가 : 전역 변수에 현재 로그인된 사용자 이름 저장
        strncpy(current_user.name, name, NAME_MAX - 1);
        current_user.name[NAME_MAX - 1] = '\0';

        mvwprintw(win_2, 1, boxwidth / 2 - strlen(success) / 2, "%s", success);
        wrefresh(win_2);

        refresh();
        getch();

        wclear(win_1);
        wclear(win_2);

        wrefresh(win_1);
        wrefresh(win_2);

        clear();
        refresh();

        delwin(win_1);
        delwin(win_2);
        loginSuccessMenu();
    }
    else
    {
        mvwprintw(win_2, 1, 1, ">> [%s] %s", name, fail);
        wrefresh(win_2);

        refresh();
        getch();



        // 회원가입 여부 묻는 창 생성
        WINDOW *win_3 = newwin(5, boxwidth, starty + 15, startx);
        box(win_3, '|', '-');
        mvwprintw(win_3, 2, boxwidth / 2 - strlen(question) / 2, "%s", question);
        wrefresh(win_3);

        refresh();

        char choice = wgetch(win_3); // 사용자 입력 받기

        if (choice == 'Y' || choice == 'y')
        {
            clear();
            refresh();

            delwin(win_1);
            delwin(win_2);
            delwin(win_3);
            registerUser();
        }
        else
        {
            mvwprintw(win_3, 3, boxwidth / 2 - 9, "Returning to menu...");
            wrefresh(win_3);
            sleep(1);

            clear();
            refresh();

            delwin(win_1);
            delwin(win_2);
            delwin(win_3);
        }
    }
}

int find_filename(char *filename)
{
    DIR *dir_ptr;
    struct dirent *dirent_ptr;

    // strcat(filename, ".txt");

    // 추가 : 임시 변수를 사용해 사용자 이름 변동으로 인한 기능 문제 예방
    char temp_filename[NAME_MAX];
    snprintf(temp_filename, sizeof(temp_filename), "%s.txt", filename);

    if ((dir_ptr = opendir("users")) == NULL)
    {
        mkdir("users", 0777);
        return 0;
    }

    while ((dirent_ptr = readdir(dir_ptr)) != NULL)
    {
        // 추가 : filename -> temp_filename
        if (strcmp(dirent_ptr->d_name, temp_filename) == 0)
        {
            closedir(dir_ptr);
            return 1;
        }
    }

    closedir(dir_ptr);
    return 0;
}

void tty_mode(int how)
{
    static struct termios orig_mode;
    if (how == 0)
    {
        tcgetattr(0, &orig_mode);
    }
    else if (how == 1)
    {
        tcsetattr(0, TCSANOW, &orig_mode);
    }
}

void turnOffEchoAndIcanon()
{
    struct termios info;

    tcgetattr(0, &info);
    info.c_lflag &= ~ECHO;
    info.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &info);
}

void printMenuUI()
{
    int height, width;
    getmaxyx(stdscr, height, width);

    char *welcome = "<< Welcome To DevTok >> ";
    char *m1 = "1.  register(R)";
    char *m2 = "2.  join(J)";
    char *m3 = "3.  exit(Q)";
    char *choice = "enter the menu: ";

    int boxheight = 13;
    int boxwidth = 30;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 4, width / 2 - strlen(welcome) / 2, "%s", welcome);

    WINDOW *win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1, '|', '-');

    mvwprintw(win_1, 3, boxwidth / 2 - strlen(m1) / 2, "%s", m1);
    mvwprintw(win_1, 6, boxwidth / 2 - strlen(m2) / 2, "%s", m2);
    mvwprintw(win_1, 9, boxwidth / 2 - strlen(m3) / 2, "%s", m3);

    WINDOW *win_2 = newwin(3, boxwidth, starty + boxheight, startx);
    mvwprintw(win_2, 1, boxwidth / 2 - strlen(choice) / 2, "%s", choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);

    delwin(win_1);
    delwin(win_2);
}

void loginSuccessMenu()
{
    int flag = 0;

    clear();
    refresh();

    noecho();

    while(flag != 1) {

        clear();
        refresh();

        printMainMenu();

        int c = getch();
        switch (c)
        {
        case '1':
            time_to_study = getTargetTime();
            realStudySecond = 0;
            
            clear();
            refresh();
            
            doStudy();
            break;
        case '2':
            clear();
            refresh();

            viewRecordMenu();
            break;
        case '3':
            flag = 1;
            break;
        }
    }

    clear();
    refresh();
}

void printMainMenu() {
    int height, width;
    getmaxyx(stdscr, height, width);

    char *welcome = "DevTok";
    char *m1 = "1.  study ";
    char *m2 = "2.  view record";
    char *m3 = "3.  return";
    char *choice = "enter the menu: ";

    int boxheight = 3;
    int boxwidth = 30;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 8, width / 2 - strlen(welcome) / 2, "%s", welcome);

    WINDOW *win_1 = newwin(boxheight, boxwidth, starty - 5, startx);
    box(win_1, '|', '-');

    WINDOW *win_2 = newwin(boxheight, boxwidth, starty - 2, startx);
    box(win_2, '|', '-');

    WINDOW *win_3 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_3, '|', '-');

    WINDOW *win_4 = newwin(boxheight, boxwidth, starty + 8, startx);
    mvwprintw(win_1, 1, boxwidth / 2 - strlen(m1) / 2, "%s", m1);
    mvwprintw(win_2, 1, boxwidth / 2 - strlen(m2) / 2, "%s", m2);
    mvwprintw(win_3, 1, boxwidth / 2 - strlen(m3) / 2, "%s", m3);
    mvwprintw(win_4, 1, boxwidth / 2 - strlen(choice) / 2, "%s", choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);
    wrefresh(win_3);
    wrefresh(win_4);

    delwin(win_1);
    delwin(win_2);
    delwin(win_3);
    delwin(win_4);
}

void viewRecordMenu()
{

    clear();
    refresh();
    noecho();

    int height, width;
    getmaxyx(stdscr, height, width);

    char *title = "<< view record >>";
    char *m1 = "1. my record";
    char *m2 = "2. Ranking";
    char *choice = "enter the menu: ";

    int boxheight = 7;
    int boxwidth = 30;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 3, width / 2 - strlen(title) / 2, "%s", title);

    WINDOW *win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1, '|', '-');

    WINDOW *win_2 = newwin(boxheight, boxwidth, starty + 8, startx);
    mvwprintw(win_1, 2, boxwidth / 2 - strlen(m1) / 2, "%s", m1);
    mvwprintw(win_1, 4, boxwidth / 2 - strlen(m2) / 2, "%s", m2);
    mvwprintw(win_2, 1, boxwidth / 2 - strlen(choice) / 2, "%s", choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);

    int c = getch();
    switch (c)
    {
    case '1':
        showMyRecord();
        break;
    case '2':
        showAllRankings();
        break;
    }

    delwin(win_1);
    delwin(win_2);
    clear();
    refresh();
}

int getTargetTime()
{
    clear();
    refresh();

    int height, width;
    getmaxyx(stdscr, height, width);

    char *title = "<< Goal >>";
    char *m1 = "set your target_time(h m s): ";
    char *m2 = "target_time was set!";
    int h, m, s;

    int boxheight = 11;
    int boxwidth = 40;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    WINDOW *win_1 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_1, '|', '-');
    wrefresh(win_1);

    WINDOW *win_2 = newwin(3, boxwidth, starty + 12, startx);

    mvprintw(starty - 1, width / 2 - strlen(title) / 2, "%s", title);
    refresh();

    mvwprintw(win_1, boxheight / 2, boxwidth / 2 - strlen(m1) / 2 - 3, "%s", m1);
    wrefresh(win_1);

    echo();
    wscanw(win_1, "%d %d %d", &h, &m, &s);
    noecho();

    mvwprintw(win_2, 1, boxwidth / 2 - strlen(m2) / 2, "%s", m2);
    wrefresh(win_2);

    getchar();

    delwin(win_1);
    delwin(win_2);

    clear();
    refresh();

    return h * 3600 + m * 60 + s;
}

// 추가 : 랭킹 보기
void showAllRankings()
{
    clear();
    refresh();
    noecho();

    // 랭킹 파일에서 사용자 정보 불러오기
    if (load_users(RANKING_FILE) < 0)
    {
        mvprintw(0, 0, "랭킹 파일을 불러오는 데 실패했습니다.");
        refresh();
        getch();
        return;
    }

    char *title = "<< All Rankings >>";
    char *menu = "Ranking           Name            Total Time         Duration";

    // int count = 7;
    // member m[7] = {
    //     {1, "aaaaa", 3600000, 400},
    //     {2, "bbbbb", 3400000, 500},
    //     {3, "ccccc", 3300000, 600},
    //     {4, "ddddd", 3000000, 400},
    //     {5, "eeeee", 2800000, 500},
    //     {6, "fffff", 2400000, 200},
    //     {7, "ggggg", 2100000, 500}
    // };

    int height, width;
    getmaxyx(stdscr, height, width);

    int boxheight = 3;
    int boxwidth = 80;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 10, width / 2 - strlen(title) / 2, "%s", title);

    WINDOW *win_1 = newwin(boxheight, boxwidth, starty - 8, startx);
    box(win_1, '|', '=');

    WINDOW *win_2 = newwin(20, boxwidth, starty - 5, startx);
    box(win_2, '|', '-');

    mvwprintw(win_1, 1, boxwidth / 2 - strlen(menu) / 2, "%s", menu);

    for (int i = 0; i < user_count; i++)
    {
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 - 25, "%d", users[i].rank);
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 - 12, "%s", users[i].name);
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 + 5, "%d", users[i].cumulative_time);
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 + 26, "%d", users[i].continuous_time);
    }

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);

    getch();

    delwin(win_1);
    delwin(win_2);
    clear();
    refresh();
}

// 추가 : 내 기록 보기
void showMyRecord()
{
    clear();
    refresh();
    noecho();

    // 사용자 데이터 로드
    if (load_users(RANKING_FILE) < 0)
    {
        mvprintw(0, 0, "랭킹 파일을 불러오는 데 실패했습니다.");
        refresh();
        getch();
        return;
    }

    int height, width;
    getmaxyx(stdscr, height, width);

    // 사용자 정보 찾기
    int index = -1;
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, current_user.name) == 0) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        mvprintw(0, 0, "사용자 %s 는 랭킹에 등록되지 않은 이름 입니다.", current_user.name);
        refresh();
        getch();
        return;
    }

    // member m = {2, "aaa", 2010, 400};
    // 랭킹 파일에서 사용자 정보 불러오기
    // if (load_users(RANKING_FILE) < 0)
    // {
    //     mvprintw(0, 0, "랭킹 파일을 불러오는 데 실패했습니다.");
    //     refresh();
    //     getch();
    //     endwin();
    //     return;
    // }

    User user = users[index];

    // 사용자 정보 출력
    char *title = "<< My Record >>";
    char *m1 = "Ranking     : ";
    char *m2 = "Total Time  : ";
    char *m3 = "Duration    : ";
    char *detail = "show detail (P)";

    int boxheight = 13;
    int boxwidth = 50;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 5, width / 2 - strlen(title) / 2, "%s", title);

    WINDOW *win_1 = newwin(3, boxwidth, starty - 2, startx);
    box(win_1, '|', '=');
    mvwprintw(win_1, 1, 20, "%s's record", user.name);

    WINDOW *win_2 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_2, '|', '-');

    mvwprintw(win_2, 3, 5, "%s%d", m1, user.rank);
    mvwprintw(win_2, 6, 5, "%s%d", m2, user.cumulative_time);
    mvwprintw(win_2, 9, 5, "%s%d", m3, user.continuous_time);

    WINDOW *win_3 = newwin(3, boxwidth, starty + boxheight + 2, startx);
    box(win_3, '|', '-');
    mvwprintw(win_3, 1, boxwidth / 2 - strlen(detail) / 2, "%s", detail);


    refresh();
    wrefresh(win_1);
    wrefresh(win_2);
    wrefresh(win_3);

    char c = getch();
    if (c == 'p' || c == 'P') {
        clear();
        char ext_name[NAME_MAX + 10];

        sprintf(ext_name, "%s.txt", user.name);
        showDetail(ext_name);
    }


    delwin(win_1);
    delwin(win_2);
    clear();
    refresh();

}

void showDetail(const char* filename) {
    initscr();
    clear();
    noecho();

    int height, width;
    getmaxyx(stdscr, height, width);

    char filePath[256];
    snprintf(filePath, sizeof(filePath), "users/%s", filename); // 파일 경로 생성

    FILE* file = fopen(filePath, "r");
    if (!file) {
        mvprintw(height / 2, width / 2 - 10, "File not found: %s", filePath);
        refresh();
        getch();
        return;
    }

    // 파일 내용을 읽어와 화면에 출력
    char buffer[256];
    int line = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        mvprintw(line++, 2, "%s", buffer);
        if (line >= height - 2) {
            mvprintw(height - 1, 2, "Press any key to continue...");
            refresh();
            getch();
            clear();
            line = 0;
        }
    }

    fclose(file);

    mvprintw(height - 1, 2, "Press any key to return...");
    refresh();
    getch();
    clear();
    endwin();
}

void doStudy() {
    // 공부 시작 시간 기록
	t = time(NULL);
	tm = *localtime(&t);

    char c;
    pthread_t thread1;

    clear();
    refresh();

    noecho();
    cbreak();

    nodelay(stdscr, TRUE);

    realStudySecond = 0;
    while (1) {
        clear();
        refresh();

        printStudyUI();

		sleep(1);
		realStudySecond += 1;

        if ((c = getch()) != EOF) { // 입력 없으면 건너뛰기
			if (c == CHANGE_KEY) { // 휴식시간 진입
                clear();
                refresh();

				pthread_create(&thread1, NULL, change_to_restTime, NULL);
				pthread_join(thread1, NULL);

			} else if (c == EXIT_KEY) { // 공부 조기 종료
                clear();
                refresh();

				endStudy();
				break;
			}
		} else {
			clearerr(stdin);
		}



		if (realStudySecond == time_to_study) { // 시간 종료: 추가공부 여부 물어보기
            clear();
            refresh();

            nodelay(stdscr, FALSE);

			int height, width;
            getmaxyx(stdscr, height, width);

            char* m1 = "time is over !!";
            char m2[40];
            char* m3 = ">> Study more? (y/n)";
            sprintf(m2, "Study Time: %02dh %02dm %02ds", realStudySecond / 3600, realStudySecond % 3600 / 60, realStudySecond % 60);

            int boxheight = 7;
            int boxwidth = 30;

            int starty = (height - boxheight)/2;
            int startx = (width - boxwidth)/2;

            WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
            box(win_1, '|', '-');
            mvwprintw(win_1, 1, boxwidth/2 - strlen(m1)/2, "%s", m1);
            mvwprintw(win_1, 3, boxwidth/2 - strlen(m2)/2, "%s", m2);
            wrefresh(win_1);
            refresh();

            sleep(1);

            WINDOW* win_2 = newwin(3, boxwidth, starty+7, startx);
            mvwprintw(win_2, 1, boxwidth/2 - strlen(m3)/2, "%s", m3);
            wrefresh(win_2);
            refresh();
			
            c = getch();
			if (c == 'y') { // 추가 시간 입력 후 반복문으로 올라가기
                delwin(win_1);
                delwin(win_2);

                clear();
                refresh();

				time_to_study += getTargetTime();

                clear();
                refresh();
                
                nodelay(stdscr, TRUE);
				continue;
			} else {
				endStudy();
			}



			break;
		}

    }
    // 설정 다시 되돌리기
    nodelay(stdscr, FALSE);

}

void printStudyUI() {
    // 터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "On Studying...";
    char change[20];
    char stop[20];
    char current[40];
    char goal[40];
    sprintf(change, ">> REST(%c) <<", CHANGE_KEY);
    sprintf(stop, ">> STOP(%c) <<", EXIT_KEY);
    sprintf(current, "Study time: %02dh %02dm %02ds", realStudySecond / 3600, (realStudySecond % 3600) / 60, realStudySecond % 60);
    sprintf(goal, "Goal Time: %02dh %02dm %02ds", time_to_study / 3600, time_to_study % 3600 / 60, time_to_study % 60);
    int barSize = 28;

    // 박스 크기 설정
    int boxheight = 3;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //Title
    mvprintw(starty-5, width/2-strlen(title)/2, "%s", title);
    mvprintw(starty-4, width/2-strlen(goal)/2, "%s", goal);
    refresh();

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1,'|','-');//box 그리기
    wrefresh(win_1);

    WINDOW* win_2 = newwin(boxheight, boxwidth, starty+5, startx);
    mvwaddstr(win_2,1,boxwidth/2-strlen(change)/2, change);
    mvwaddstr(win_2,2,boxwidth/2-strlen(stop)/2, stop);
    wrefresh(win_2);

    mvwaddstr(win_1, 0, boxwidth/2-strlen(current)/2, current);

    wmove(win_1, 1, 1);
    for(int i=0; i<realStudySecond * 100 / time_to_study * (boxwidth - 2) / 100; i++) {
        wstandout(win_1);
        waddstr(win_1, "/");
    }

    wstandend(win_1);
    wrefresh(win_1);
    refresh();

    delwin(win_1);
    delwin(win_2);
}

void *change_to_restTime(void *f) {
	/**
	 * 쉬는시간: CHANGE_KEY 누르면 다시 공부시간
	 * 쉬는시간(초) 기록: 1분마다 출력(분)
	 */
	int restTime = 0;
	char c = 0;

    //초기화
    initscr();
    cbreak();
    noecho();

    nodelay(stdscr, TRUE);

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "On Resting";
    char stop[20];
    sprintf(stop, ">> STOP(%c) <<", CHANGE_KEY);

    int barSize = 28;

    //박스 크기 설정
    int boxheight = 11;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1,'|','-');//box 그리기
    mvwprintw(win_1,boxheight/2, boxwidth/2-strlen(title)/2-2, "%s", title);
    wrefresh(win_1);

    WINDOW* win_2 = newwin(3, boxwidth, starty+12, startx);
    mvwprintw(win_2,1, boxwidth/2-strlen(stop)/2, "%s", stop);
    refresh();
    wrefresh(win_2);

	while (1) {
		if ((c = getch()) == CHANGE_KEY) {
			break;
		}

		sleep(1);
		restTime += 1;

	}

    delwin(win_1);
    delwin(win_2);

    clear();
    refresh();

	return NULL;
}

void endStudy() {
/**
	 * 현재 사용자 파일을 열고 다음을 수행한다.
	 * 1. 첫줄에 누적 공부 시간, 최장 연속 공부 시간 수정
	 * 2. 오늘 공부에 대한 코멘트를 받은 뒤 이를 마지막 줄에 현재 시간, 공부 시간과 함께 추가
	 */
	char directory[500]; // 파일 경로 저장
	char tmp_dir[500];   // 임시 파일 경로 저장
	
	char comment[200];        // 코멘트 저장
	char str[500];   		  // 최종적으로 파일에 추가할 줄 저장

	sprintf(directory, "users/%s.txt", current_user.name);

	int max_time, sum_time;
	char buf[500]; // 한줄씩 가져오기

	FILE *fptr;
	FILE *temp;
	fptr = fopen(directory, "r+");

	// 첫줄에 있는 내용 수정
	// 임시 파일 만든 후 첫줄 빼고 다 복사한뒤 기존 파일 삭제 후 임시 파일 이름 변경
	fscanf(fptr, "%d %d", &sum_time, &max_time);
	sum_time += realStudySecond;
	if (realStudySecond > max_time) max_time = realStudySecond;

	sprintf(tmp_dir, "%stemp.txt", STORAGE);

	temp = fopen(tmp_dir, "w+");

	// 첫줄 수정 후 나머지 한줄씩 복사
	fprintf(temp, "%d %d", sum_time, max_time);
	while(fgets(buf, 200, fptr) != NULL) {
		fputs(buf, temp);
	}

	fclose(fptr);
	fclose(temp);

	// 기존 파일 삭제 후 임시 파일 이름 변경
	remove(directory);
	rename(tmp_dir, directory);



	// 2. 코멘트 받고 마지막 줄에 추가
	fptr = fopen(directory, "a+");

	// printf("오늘 공부에 대한 코멘트 입력(100자 이내): ");
	// tty_mode(1);
	// tty_mode(0);
	// fgets(comment, 100, stdin);

    clear();
    refresh();

    int height, width;
    getmaxyx(stdscr, height, width);

    char* m1 = "<< Comment on today's study >>";

    int boxheight = 7;
    int boxwidth = 30;

    int starty = (height-boxheight)/2;
    int startx = (width-boxwidth)/2;

    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);

    mvprintw(height/2 - 5, width/2 - strlen(m1)/2, "%s", m1);
    mvprintw(height/2 - 4, width/2 - 15, "==============================");
    mvprintw(height/2 + 5, width/2 - 15, "==============================");
    refresh();

    nodelay(stdscr, FALSE);
    echo();

    wmove(win_1, 1, 1);
    wgetnstr(win_1, comment, sizeof(comment));

	sprintf(str, "%d-%02d-%02d %02d:%02d:%02d %d %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, realStudySecond, comment);
	
	fputs(str, fptr);
	fclose(fptr);

    mvprintw(height/2 + 7, width/2 - 10, "Success to save Log!");

    getch();

    nodelay(stdscr, TRUE);
    noecho();
	
	// ranking.txt 파일 수정

	// 랭킹 파일과 유저 파일 경로 설정
	const char *ranking_filename = "ranking.txt";
	char user_filename[NAME_MAX + 20];
	snprintf(user_filename, sizeof(user_filename), "users/%s.txt", current_user.name);

	// 랭킹 업데이트 및 저장
	update_and_save_ranking(ranking_filename, user_filename, current_user.name);
	
}