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
#include <curses.h>

#define LEFT 50
#define HEIGHT 5

#define STORAGE "users" // ����ҷ� ����� ���丮 �̸�

#define LEFT 50
#define HEIGHT 5

void tty_mode(int);
void turnOffEchoAndIcanon();
int find_filename(char *);
void printMenuUI();
void registerUser();
void loginUser();
void loginSuccessMenu();

int main() {
    int flag = 0; // 프로그램 종료 플래그

    // 기존 터미널 설정 백업
    tty_mode(0);

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

        // Enter 대기
        if (getch() == '\n') {
            sleep(1);
        }

        endwin(); // curses 종료
    }

    // 터미널 설정 복원 후 프로그램 종료
    tty_mode(1);
    printf("프로그램 종료\n");
    return 0;
}

void registerUser() {
    char filename[257]; // 사용자 이름 저장

    printf("등록하실 사용자 이름을 입력해주세요(1 ~ 256 bytes): ");

    // 이름 입력받을 때 잠깐 설정 되돌리기
    tty_mode(1);
    scanf("%256s", filename);

    if (find_filename(filename)) {
        // 등록 실패 - 이미 있는 사용자 이름
        printf("이미 있는 이름입니다!\n");
    } else {
        // 등록 성공 - 사용자 파일 생성
        mkdir("users", 0777); // 디렉토리가 없으면 생성
        chdir("users");
        creat(filename, 0777);
        printf("등록 성공!\n");
        chdir("..");
    }

    // 설정 복원 및 ECHO, ICANON 비트 끄기
    tty_mode(0);
    turnOffEchoAndIcanon();
}

void loginUser() {
    char filename[257]; // 사용자 이름 저장

    printf("접속하실 사용자 이름을 입력해주세요(1 ~ 256 bytes): ");

    // 이름 입력받을 때 잠깐 설정 되돌리기
    tty_mode(1);
    scanf("%256s", filename);

    if (find_filename(filename)) {
        // 접속 성공
        printf("접속 성공!\n");
        sleep(1);
        endwin();
        loginSuccessMenu(); // 로그인 성공 시 새로운 화면
    } else {
        // 접속 실패 - 존재하지 않는 사용자
        printf("존재하지 않는 사용자입니다!\n");
    }

    // 설정 복원 및 ECHO, ICANON 비트 끄기
    tty_mode(0);
    turnOffEchoAndIcanon();
}

int find_filename(char* filename) {
    // 저장소 디렉토리에서 파일 검색

    DIR *dir_ptr;
    struct dirent *dirent_ptr;

    strcat(filename, ".txt");

    if ((dir_ptr = opendir("users")) == NULL) {
        mkdir("users", 0777);
        return 0; // 디렉토리가 없으면 파일도 없음
    }

    while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
        if (strcmp(dirent_ptr->d_name, filename) == 0) {
            closedir(dir_ptr);
            return 1; // 파일 존재
        }
    }

    closedir(dir_ptr);
    return 0; // 파일 없음
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
    move(HEIGHT, LEFT);
    addstr("<< DevTok >>");

    move(HEIGHT + 5, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 6, LEFT - 9);
    addstr("|         register(R)        |");
    move(HEIGHT + 7, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 10, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 11, LEFT - 9);
    addstr("|           join(J)          |");
    move(HEIGHT + 12, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 15, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 16, LEFT - 9);
    addstr("|           quit(Q)          |");
    move(HEIGHT + 17, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 20, LEFT - 1);
    addstr("Type a menu: ");
}

void loginSuccessMenu() {
    initscr();
    clear();

    move(HEIGHT, LEFT - 3);
    addstr("Welcome To DevTok");

    move(HEIGHT + 3, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 4, LEFT - 9);
    addstr("|          study(S)          |");
    move(HEIGHT + 5, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 7, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 8, LEFT - 9);
    addstr("|          record(R)         |");
    move(HEIGHT + 9, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 11, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 12, LEFT - 9);
    addstr("|          option(O)         |");
    move(HEIGHT + 13, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 15, LEFT - 9);
    addstr("------------------------------");
    move(HEIGHT + 16, LEFT - 9);
    addstr("|     back to StartMenu(B)   |");
    move(HEIGHT + 17, LEFT - 9);
    addstr("------------------------------");

    move(HEIGHT + 20, LEFT - 1);
    addstr("Type a menu: ");
    refresh();

    switch (getch()) {
        case 'b': case 'B':
            endwin();
            break;

        default:
            endwin();
            printf("잘못된 입력입니다. 메인 메뉴로 돌아갑니다.\n");
            sleep(1);
            break;
    }
}
