#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define MAX 100

int main(){

    //초기화
    initscr();
    cbreak();

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* m1 = "<< Comment on today's study >>";
    char comment[MAX] = "";

    //박스 크기 설정
    int boxheight = 7;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);

    mvprintw(height/2-5, width/2-strlen(m1)/2, m1);
    mvprintw(height/2-4, width/2-15, "==============================");
    mvprintw(height/2+5, width/2-15, "==============================");

    //
    refresh();
    wrefresh(win_1);
    
    //사용자 코멘트 입력
    mvwscanw(win_1, 1, 1, "%s", &comment);

    //ncurses 종료
    endwin();

    return 0;
}
