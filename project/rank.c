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
    clear();

    //메시지
    char* title = "<< ranking >> ";
    char* menu = "ranking           name            total time         duration";

    //사용자 정보
    int count = 7;
    member m[7] = {{1, "aaaaa", 3600000, 400}, {2, "bbbbb", 3400000, 500}, {3, "ccccc", 3300000, 600}, {4, "ddddd", 3000000, 400}, {5, "eeeee", 2800000, 500}, 
                   {6, "fffff", 2400000, 200}, {7, "ggggg", 2100000, 500}};

    //터미널 크기 가져오기
    int height, width;
    getmaxyx(stdscr, height, width);

    //박스 크기 설정
    int boxheight = 3;
    int boxwidth = 80;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //title
    mvprintw(starty-10, width/2-strlen(title)/2, title);

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty-8, startx);
    box(win_1,'|','=');//box 그리기

    WINDOW* win_2 = newwin(20, boxwidth, starty-5, startx);
    box(win_2, '|', '-');

    mvwprintw(win_1, 1, boxwidth/2-strlen(menu)/2, menu);

    for(int i=0;i<count;i++){
        mvwprintw(win_2, 2+i*2, boxwidth/2-25, "%d", m[i].rank);
        mvwprintw(win_2, 2+i*2, boxwidth/2-12, "%s", m[i].name);
        mvwprintw(win_2, 2+i*2, boxwidth/2+5, "%d", m[i].total_time);
        mvwprintw(win_2, 2+i*2, boxwidth/2+26, "%d", m[i].duration);
    }

    refresh();
    wrefresh(win_1);
    wrefresh(win_2);

    getchar();

    endwin();

    return 0;
}