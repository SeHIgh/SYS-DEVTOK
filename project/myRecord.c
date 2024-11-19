#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

typedef struct member{
    int rank;
    char* name;
    int total_time;
    int duration;
}member;

int main(){

    //초기화
    initscr();
    cbreak();
    noecho();

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    //사용자 정보
    member m = {2, "aaa", 2010, 400};

    //
    char* title = "<< Record >> ";
    char* m1 = "ranking     : ";
    char* m2 = "total time  : ";
    char* m3 = "duration    : ";

    //박스 크기 설정
    int boxheight = 13;
    int boxwidth = 50;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //Title
    mvprintw(starty-5, width/2-strlen(title)/2, title);

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(3, boxwidth, starty-2, startx);
    box(win_1,'|','=');//box 그리기
    mvwprintw(win_1, 1, 20, "%s's record", m.name);

    WINDOW* win_2 = newwin(boxheight, boxwidth, starty+1, startx);
    box(win_2,'|','-');

    //기록
    mvwprintw(win_2, 3, startx-20, m1);
    mvwprintw(win_2, 3, startx, "%d", m.rank);

    mvwprintw(win_2, 6, startx-20, m2);
    mvwprintw(win_2, 6, startx, "%d", m.total_time);

    mvwprintw(win_2, 9, startx-20, m3);
    mvwprintw(win_2, 9, startx, "%d", m.duration);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);
    
    //사용자 입력 대기
    int c = getch();
    switch(c){
        case '1': break; //등록
        case '2': break; //접속
        case '3': break; //나가기
    }

    //ncurses 종료
    endwin();

    return 0;
}