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

    char* welcome = "<< Welcome To DevTok >> ";
    char* m1 = "1.  register";
    char* m2 = "2.  join";
    char* m3 = "3.  exit";
    char* choice = "enter the menu: ";

    //박스 크기 설정
    int boxheight = 13;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //Title
    mvprintw(starty-2, width/2-strlen(welcome)/2, welcome);

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty+1, startx);
    box(win_1,'|','-');//box 그리기

    WINDOW* win_2 = newwin(3, boxwidth, starty+15, startx);

    //메뉴창
    mvwprintw(win_1, 3, boxwidth/2-strlen(m1)/2, m1);
    mvwprintw(win_1, 6, boxwidth/2-strlen(m1)/2, m2);
    mvwprintw(win_1, 9, boxwidth/2-strlen(m1)/2, m3);
    mvwprintw(win_2, 1, boxwidth/2-strlen(choice)/2, choice);

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