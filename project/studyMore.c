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

    char* m1 = "time is over !!";
    char* m2 = ">> Study more? (y/n)";

    //박스 크기 설정
    int boxheight = 7;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1,'|','-');//box 그리기

    WINDOW* win_2 = newwin(3, boxwidth, starty+7, startx);

    //
    mvwprintw(win_1, 3, boxwidth/2-strlen(m1)/2, m1);
    mvwprintw(win_2, 1, boxwidth/2-strlen(m2)/2, m2);

    refresh();
    wrefresh(win_1);
    flash();
    sleep(1);

    refresh();
    wrefresh(win_2);
    
    //사용자 입력 대기
    int c = getch();
    switch(c){
        case 'y': break; // 추가 공부 하기
        case 'n': break; // 공부 끝내기 -> 코멘트 입력
    }

    //ncurses 종료
    endwin();

    return 0;
}
