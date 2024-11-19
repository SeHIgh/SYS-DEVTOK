#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

int main(){

    //초기화
    initscr();
    cbreak();

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "<< Goal >>";
    char* m1 = "set your target_time: ";
    char* m2 = "target_time was set!";
    
    int target_time;

    //박스 크기 설정
    int boxheight = 11;
    int boxwidth = 40;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty+1, startx);
    box(win_1,'|','-');//box 그리기

    WINDOW* win_2 = newwin(3, boxwidth, starty+12, startx);
    box(win_1,'|','-');

    //Title
    mvprintw(starty-1, width/2-strlen(title)/2, title);
    refresh();
    wrefresh(win_1);

    //목표시간 설정
    mvwprintw(win_1, boxheight/2, boxwidth/2-strlen(m1)/2-3, m1);
    wscanw(win_1, "%d", &target_time);

    refresh();
    wrefresh(win_1);

    //목표시간 설정 완료
    mvwprintw(win_2,1, boxwidth/2-strlen(m2)/2, "%s", m2);

    refresh();
    wrefresh(win_2);

    getchar();
    //ncurses 종료
    endwin();

    return 0;
}