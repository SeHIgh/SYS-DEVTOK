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

    char* welcome = "DevTok";
    char* m1 = "1.  study ";
    char* m2 = "2.  record";
    char* m3 = "3.  option";
    char* m4 = "4.  return";
    char* choice = "enter the menu: ";

    //박스 크기 설정
    int boxheight = 3;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //Title
    mvprintw(starty-8, width/2-strlen(welcome)/2, welcome);

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty-5, startx);
    box(win_1,'|','-');//box 그리기

    WINDOW* win_2 = newwin(boxheight, boxwidth, starty-2, startx);
    box(win_2,'|','-');

    WINDOW* win_3 = newwin(boxheight, boxwidth, starty+1, startx);
    box(win_3,'|','-');

    WINDOW* win_4 = newwin(boxheight, boxwidth, starty+4, startx);
    box(win_4,'|','-');

    WINDOW* win_5 = newwin(boxheight, boxwidth, starty+8, startx);

    //메뉴창
    mvwprintw(win_1, 1, boxwidth/2-strlen(m1)/2, m1);
    mvwprintw(win_2, 1, boxwidth/2-strlen(m1)/2, m2);
    mvwprintw(win_3, 1, boxwidth/2-strlen(m1)/2, m3);
    mvwprintw(win_4, 1, boxwidth/2-strlen(m1)/2, m4);
    mvwprintw(win_5, 1, boxwidth/2-strlen(choice)/2, choice);

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);
    wrefresh(win_3);
    wrefresh(win_4);
    wrefresh(win_5);
    
    //사용자 입력 대기
    int c = getch();
    switch(c){
        case '1': break; //공부하기
        case '2': break; //기록보기(순위?)
        case '3': break; //옵션
        case '4': break; //시작메뉴로 돌아가기
    }

    //ncurses 종료
    endwin();

    return 0;
}
