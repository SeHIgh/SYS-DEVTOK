#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define NAME_MAX 20

int main(){

    //초기화
    initscr();
    cbreak();

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    char* title = "<< Register >> ";
    char* m1 = "set your NAME: ";
    char* m2 = " was registered!";
    char* m3 = " is already registered.";
    
    char name[NAME_MAX];

    //박스 크기 설정
    int boxheight = 11;
    int boxwidth = 30;

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

    //등록창
    mvwprintw(win_1, boxheight/2, boxwidth/2-strlen(m1)/2-3, m1);
    wscanw(win_1, "%s", &name);

    refresh();
    wrefresh(win_1);

    //등록 성공한 경우
    mvwprintw(win_2,1, 1, ">> [%s] %s", name, m2);

    //이미 존재하는 이름인 경우
    //mvwprintw(win_2,1, startx, ">> [%s] %s", name, m3); 

    refresh();
    wrefresh(win_2);

    getchar();
    //ncurses 종료
    endwin();

    return 0;
}