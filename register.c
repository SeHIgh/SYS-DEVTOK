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

#include "./algorithm/ranking.h"    // 사용자 기록 보기 위해 추가

#define LEFT 50
#define HEIGHT 5

void tty_mode(int);
void turnOffEchoAndIcanon();
void restoreSettings();
void forceRestoreEcho(); 
int find_filename(char *);
void printMenuUI();
void registerUser();
void loginUser();
void loginSuccessMenu();

// 추가 4. 유저 이름의 폴더를 탐색하여 안의 내용을 출력하는 함수
void recordUserInfo(const char *user_name);

// 추가 1. 로그인한 사용자 이름을 저장할 전역 변수
char user_name[257];

int main() {
    int flag = 0; // 프로그램 종료 플래그

    // 기존 터미널 설정 백업
    tty_mode(0);
    atexit(restoreSettings); // 종료 시 설정 복원

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

    forceRestoreEcho(); // 강제로 ECHO 복원
    printf("프로그램 종료\n");
    return 0;
}

// 종료 시 설정 복원 함수
void restoreSettings() {
    tty_mode(1); // 터미널 설정 복원
}

//ECHO 복원
void forceRestoreEcho() {
    struct termios info;
    tcgetattr(0, &info);     // 현재 설정 가져오기
    info.c_lflag |= ECHO;    // ECHO 비트 켜기
    info.c_lflag |= ICANON;  // ICANON 비트 켜기
    tcsetattr(0, TCSANOW, &info); // 설정 적용
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
    // 추가 2. 전역 변수 사용으로 인해 주석처리
    //char filename[257]; // 사용자 이름 저장

    printf("접속하실 사용자 이름을 입력해주세요(1 ~ 256 bytes): ");

    // 이름 입력받을 때 잠깐 설정 되돌리기
    tty_mode(1);
    scanf("%256s", user_name);

    if (find_filename(user_name)) {
        // 접속 성공
        printf("접속 성공!\n");
        sleep(1);
        endwin();
        loginSuccessMenu(); // 로그인 성공 시 새로운 화면
    } else {
        // 접속 실패 - 존재하지 않는 사용자
        printf("존재하지 않는 사용자입니다!\n");
        printf("회원가입 하시겠습니까? (Y/N): ");
    
        char choice;
        tty_mode(1); // 입력 받기 위해 설정 복원
        scanf(" %c", &choice);

        if (choice == 'Y' || choice == 'y') {
            printf("회원가입 메뉴로 이동합니다.\n");
            sleep(1);
            endwin(); 
            registerUser(); // 등록 함수 호출
        } else if (choice == 'N' || choice == 'n') {
            printf("메인 메뉴로 이동합니다.\n");
            sleep(1);
        } else {
            printf("잘못된 입력입니다. 메인 메뉴로 이동합니다.\n");
            sleep(1);
        }
    }

    // 설정 복원 및 ECHO, ICANON 비트 끄기
    tty_mode(0);
    turnOffEchoAndIcanon();
}

int find_filename(char* filename) {
    // 저장소 디렉토리에서 파일 검색
    DIR *dir_ptr;
    struct dirent *dirent_ptr;

    // 추가 3. 전역 변수 (user_name)을 건들이지 않도록 임시 변수 사용
    char full_filename[260]; // 유저이름의 파일을 저장할 임시 변수
    snprintf(full_filename, sizeof(full_filename), "%s.txt", filename); // ".txt"를 붙임

    if ((dir_ptr = opendir("users")) == NULL) {
        mkdir("users", 0777);
        return 0; // 디렉토리가 없으면 파일도 없음
    }

    // 추가 4. while문의 비교문에서도 임시변수로 변경
    while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
        if (strcmp(dirent_ptr->d_name, full_filename) == 0) {
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
        tcgetattr(0, &orig_mode); // 현재 설정 저장
    } else if (how == 1) {
        tcsetattr(0, TCSANOW, &orig_mode); // 설정 복원
    }
}

void turnOffEchoAndIcanon() {
    struct termios info;

    tcgetattr(0, &info);
    info.c_lflag &= ~ECHO;   // ECHO 비트 끄기
    info.c_lflag &= ~ICANON; // ICANON 비트 끄기
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

        case 'r' : case 'R':
            endwin();
            // 이전에 입력받은 사용자 이름으로 사용자 정보 출력
            // print_user_info(user_name);
            recordUserInfo(user_name);
            break;

        default:
            endwin();
            printf("잘못된 입력입니다. 메인 메뉴로 돌아갑니다.\n");
            sleep(1);
            break;
    }
}

void recordUserInfo(const char *user_name) {
    char user_filename[MAX_NAME_LENGTH + 4]; // 파일 이름을 저장할 배열
    snprintf(user_filename, sizeof(user_filename), "users/%s.txt", user_name); // 파일 경로 생성

    FILE *file_ptr = fopen(user_filename, "r"); // 파일 열기
    if (file_ptr == NULL) {
        printf("사용자 %s 의 정보를 불러오는 데 실패했습니다.\n", user_name);
        return; // 파일이 없으면 종료
    }

    char line[256]; // 파일에서 읽을 줄을 저장할 배열
    printf("\n사용자 %s 의 공부 기록:\n\n", user_name);

    // 첫 줄 처리 (누적시간, 연속시간)
    if (fgets(line, sizeof(line), file_ptr) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0'; // 개행 문자 제거
        }

        char total_time[32], streak_time[32];
        if (sscanf(line, "%31s %31s", total_time, streak_time) == 2) {
            printf("누적 시간: %s\n", total_time);
            printf("연속 시간: %s\n\n", streak_time);
        } else {
            printf("파일의 첫 줄 형식이 잘못되었습니다.\n");
            fclose(file_ptr);
            return;
        }
    }

    // 테이블 헤더 출력
    printf("| %-16s | %-16s | %-32s\n", "공부 시작 시간", "공부한 시간", "코멘트");
    printf("+----------------+-------------+--------------------------------+\n");

    // 나머지 줄 처리
    while (fgets(line, sizeof(line), file_ptr) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0'; // 개행 문자 제거
        }

        char start_time[32], study_duration[32], comment[160];
        if (sscanf(line, "%31s %31s %159[^\n]", start_time, study_duration, comment) == 3) {
            printf("| %-14s | %-11s | %-32s\n", start_time, study_duration, comment);
        } else {
            printf("| %-14s | %-11s | %-32s\n", "잘못된 형식", "-", "-");
        }
    }

    printf("+----------------+-------------+--------------------------------+\n");
    fclose(file_ptr); // 파일 닫기
}