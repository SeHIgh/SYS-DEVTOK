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

#define LEFT 50
#define HEIGHT 5

typedef struct member {
    int rank;
    char* name;
    int total_time;
    int duration;
} member;

void tty_mode(int);
void turnOffEchoAndIcanon();
void restoreSettings();
void forceRestoreEcho(); // 강제로 ECHO 복원
int find_filename(char *);
void printMenuUI();
void registerUser();
void loginUser();
void loginSuccessMenu();
void setTargetTime();
void showRanking();

int main() {
    int flag = 0; // 프로그램 종료 플래그

    // 기존 터미널 설정 백업 및 종료 시 복원 등록
    tty_mode(0);
    atexit(forceRestoreEcho); // 종료 시 강제 복원 등록

    // 시작 시 ECHO, ICANON 비트 끄기
    turnOffEchoAndIcanon();

    while (!flag) {
        initscr(); // curses 초기화
        clear();

        // UI 출력
        printMenuUI();
        refresh();

        switch (getch()) {
            case 'r': case 'R':
                endwin(); // curses 종료
                registerUser();
                break;

            case 'j': case 'J':
                endwin(); // curses 종료
                loginUser();
                break;

            case 'q': case 'Q':
                flag = 1;
                break;

            default:
                break;
        }

        endwin(); // curses 종료
    }

    printf("프로그램 종료\n");
    return 0;
}

void forceRestoreEcho() {
    struct termios info;
    tcgetattr(0, &info);     // 현재 설정 가져오기
    info.c_lflag |= ECHO;    // ECHO 켜기
    info.c_lflag |= ICANON;  // ICANON 켜기
    tcsetattr(0, TCSANOW, &info); // 설정 적용
}

void registerUser() {
    initscr();
    clear();

    // 터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "<< Register >> ";
    char* m1 = "set your NAME: ";
    char* m2 = " was registered!";
    char* m3 = " is already registered.";
    char name[NAME_MAX];

    // 박스 크기 설정
    int boxheight = 11;
    int boxwidth = 30;

    // 중앙 좌표 계산
    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    // 새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_1, '|', '-');

    WINDOW* win_2 = newwin(3, boxwidth, starty + 12, startx);
    box(win_2, '|', '-');

    // Title
    mvprintw(starty - 1, width / 2 - strlen(title) / 2,"%s",title);
    refresh();
    wrefresh(win_1);

    // 등록창
    mvwprintw(win_1, boxheight / 2, boxwidth / 2 - strlen(m1) / 2 - 3, "%s", m1);
    wrefresh(win_1);
    echo(); // 사용자 입력 허용
    mvwgetstr(win_1, boxheight / 2, boxwidth / 2 - strlen(m1) / 2 + strlen(m1) - 3, name);
    noecho(); // 사용자 입력 다시 비활성화

    // 이름 중복 확인 및 결과 표시
    if (find_filename(name)) {
        mvwprintw(win_2, 1, boxwidth / 2 - strlen(m3) / 2 - strlen(name) / 2, "[%s]%s", name, m3);
    } else {
        mkdir("users", 0777); // 디렉토리가 없으면 생성
        chdir("users");
        creat(name, 0777); // 사용자 파일 생성
        chdir("..");
        mvwprintw(win_2, 1, boxwidth / 2 - strlen(m2) / 2 - strlen(name) / 2, "[%s]%s", name, m2);
    }

    wrefresh(win_2);
    getch(); // 사용자 입력 대기

    delwin(win_1);
    delwin(win_2);
    endwin();
}

void loginUser() {
    char filename[257]; // 사용자 이름 저장

    printf("접속하실 사용자 이름을 입력해주세요(1 ~ 256 bytes): ");

    tty_mode(1);
    scanf("%256s", filename);

    if (find_filename(filename)) {
        printf("접속 성공!\n");
        sleep(1);
        endwin();
        loginSuccessMenu();
    } else {
        printf("존재하지 않는 사용자입니다!\n");
        printf("회원가입 하시겠습니까? (Y/N): ");

        char choice;
        tty_mode(1);
        scanf(" %c", &choice);

        if (choice == 'Y' || choice == 'y') {
            printf("회원가입 메뉴로 이동합니다.\n");
            sleep(1);
            endwin();
            registerUser();
        } else {
            printf("메인 메뉴로 이동합니다.\n");
            sleep(1);
        }
    }

    tty_mode(0);
    turnOffEchoAndIcanon();
}

int find_filename(char* filename) {
    DIR *dir_ptr;
    struct dirent *dirent_ptr;

    strcat(filename, ".txt");

    if ((dir_ptr = opendir("users")) == NULL) {
        mkdir("users", 0777);
        return 0;
    }

    while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
        if (strcmp(dirent_ptr->d_name, filename) == 0) {
            closedir(dir_ptr);
            return 1;
        }
    }

    closedir(dir_ptr);
    return 0;
}

void tty_mode(int how) {
    static struct termios orig_mode;
    if (how == 0) {
        tcgetattr(0, &orig_mode);
    } else if (how == 1) {
        tcsetattr(0, TCSANOW, &orig_mode);
    }
}

void turnOffEchoAndIcanon() {
    struct termios info;

    tcgetattr(0, &info);
    info.c_lflag &= ~ECHO;
    info.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &info);
}

void printMenuUI() {
    int height, width;
    getmaxyx(stdscr, height, width);

    char* welcome = "<< Welcome To DevTok >> ";
    char* m1 = "1.  register(R)";
    char* m2 = "2.  join(J)";
    char* m3 = "3.  exit(Q)";
    char* choice = "enter the menu: ";

    int boxheight = 13;
    int boxwidth = 30;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 4, width / 2 - strlen(welcome) / 2, "%s", welcome);

    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1, '|', '-');

    mvwprintw(win_1, 3, boxwidth / 2 - strlen(m1) / 2, "%s", m1);
    mvwprintw(win_1, 6, boxwidth / 2 - strlen(m2) / 2, "%s", m2);
    mvwprintw(win_1, 9, boxwidth / 2 - strlen(m3) / 2, "%s", m3);

    WINDOW* win_2 = newwin(3, boxwidth, starty + boxheight, startx);
    mvwprintw(win_2, 1, boxwidth / 2 - strlen(choice) / 2, "%s", choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);

    delwin(win_1);
    delwin(win_2);
}

void loginSuccessMenu() {
    initscr();
    clear();
    noecho();

    int height, width;
    getmaxyx(stdscr, height, width);

    char* welcome = "DevTok";
    char* m1 = "1.  study ";
    char* m2 = "2.  ranking";
    char *m3 = "3. Option";
    char* m4 = "4.  set target time";
    char* choice = "enter the menu: ";

    int boxheight = 3;
    int boxwidth = 30;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 8, width / 2 - strlen(welcome) / 2, "%s", welcome);

    WINDOW* win_1 = newwin(boxheight, boxwidth, starty - 5, startx);
    box(win_1, '|', '-');

    WINDOW* win_2 = newwin(boxheight, boxwidth, starty - 2, startx);
    box(win_2, '|', '-');

    WINDOW* win_3 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_3, '|', '-');

    WINDOW* win_4 = newwin(boxheight, boxwidth, starty + 4, startx);
    box(win_4, '|', '-');

    WINDOW* win_5 = newwin(boxheight, boxwidth, starty + 8, startx);

    mvwprintw(win_1, 1, boxwidth / 2 - strlen(m1) / 2, "%s", m1);
    mvwprintw(win_2, 1, boxwidth / 2 - strlen(m2) / 2, "%s", m2);
    mvwprintw(win_3, 1, boxwidth / 2 - strlen(m3) / 2, "%s", m3);
    mvwprintw(win_4, 1, boxwidth / 2 - strlen(m4) / 2, "%s", m4);
    mvwprintw(win_5, 1, boxwidth / 2 - strlen(choice) / 2, "%s", choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);
    wrefresh(win_3);
    wrefresh(win_4);
    wrefresh(win_5);

    int c = getch();
    switch (c) {
        case '1': break;
        case '2': showRanking(); break;
        case '3': break;
        case '4': setTargetTime(); break;
    }

    delwin(win_1);
    delwin(win_2);
    delwin(win_3);
    delwin(win_4);
    delwin(win_5);

    endwin();
}

void setTargetTime() {
    initscr();
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "<< Goal >>";
    char* m1 = "set your target_time: ";
    char* m2 = "target_time was set!";
    int target_time;

    int boxheight = 11;
    int boxwidth = 40;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    WINDOW* win_1 = newwin(boxheight, boxwidth, starty + 1, startx);
    box(win_1, '|', '-');

    WINDOW* win_2 = newwin(3, boxwidth, starty + 12, startx);

    mvprintw(starty - 1, width / 2 - strlen(title) / 2, "%s", title);
    refresh();
    wrefresh(win_1);

    mvwprintw(win_1, boxheight / 2, boxwidth / 2 - strlen(m1) / 2 - 3, "%s", m1);
    wscanw(win_1, "%d", &target_time);

    mvwprintw(win_2, 1, boxwidth / 2 - strlen(m2) / 2, "%s", m2);

    refresh();
    wrefresh(win_2);

    getchar();
    delwin(win_1);
    delwin(win_2);
}

void showRanking() {
    clear(); // 화면 초기화
    noecho(); // 입력 값 출력 방지

    char* title = "<< ranking >> ";
    char* menu = "ranking           name            total time         duration";

    int count = 7;
    member m[7] = {
        {1, "aaaaa", 3600000, 400},
        {2, "bbbbb", 3400000, 500},
        {3, "ccccc", 3300000, 600},
        {4, "ddddd", 3000000, 400},
        {5, "eeeee", 2800000, 500},
        {6, "fffff", 2400000, 200},
        {7, "ggggg", 2100000, 500}
    };

    int height, width;
    getmaxyx(stdscr, height, width);

    int boxheight = 3;
    int boxwidth = 80;

    int starty = (height - boxheight) / 2;
    int startx = (width - boxwidth) / 2;

    mvprintw(starty - 10, width / 2 - strlen(title) / 2, "%s", title);

    WINDOW* win_1 = newwin(boxheight, boxwidth, starty - 8, startx);
    box(win_1, '|', '=');

    WINDOW* win_2 = newwin(20, boxwidth, starty - 5, startx);
    box(win_2, '|', '-');

    mvwprintw(win_1, 1, boxwidth / 2 - strlen(menu) / 2, "%s", menu);

    for (int i = 0; i < count; i++) {
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 - 25, "%d", m[i].rank);
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 - 12, "%s", m[i].name);
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 + 5, "%d", m[i].total_time);
        mvwprintw(win_2, 2 + i * 2, boxwidth / 2 + 26, "%d", m[i].duration);
    }

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);

    getch(); // 사용자 입력 대기

    delwin(win_1);
    delwin(win_2);
    clear(); // 화면 정리
    refresh(); // 새로고침
}
