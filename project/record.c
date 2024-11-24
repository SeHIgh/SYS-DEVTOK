#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

int main(){

    //초기화
    initscr();
    cbreak();
    noecho();

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "<< view record >>";
    char* m1 = "1. my record";
    char* m2 = "2.  Ranking";
    char* choice = "enter the menu: ";

    //박스 크기 설정
    int boxheight = 7;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //Title
    mvprintw(starty-3, width/2-strlen(title)/2, title);

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1,'|','-');//box 그리기

    WINDOW* win_5 = newwin(boxheight, boxwidth, starty+8, startx);

    //메뉴창
    mvwprintw(win_1, 2, boxwidth/2-strlen(m1)/2, m1);
    mvwprintw(win_1, 4, boxwidth/2-strlen(m1)/2, m2);
    mvwprintw(win_5, 1, boxwidth/2-strlen(choice)/2, choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_5);
    
    //사용자 입력 대기
    int c = getch();
     switch(c){
        case '1': break; //내 기록 보기
        case '2': break; //순위 보기
    }

    //ncurses 종료
    endwin();

    return 0;
}
