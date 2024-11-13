#include <stdio.h>
#include <curses.h>
#include <unistd.h>

#define LEFT 50
#define HEIGHT 5

int main(){
    
    initscr();
    clear();

    move(HEIGHT,LEFT);
    addstr("<< DevTok >>");

    move(HEIGHT+5,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+6,LEFT-9);
    addstr("|         register(R)        |");
    move(HEIGHT+7,LEFT-9);
    addstr("------------------------------");

    move(HEIGHT+10,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+11,LEFT-9);
    addstr("|           join(J)          |");
    move(HEIGHT+12,LEFT-9);
    addstr("------------------------------");

    move(HEIGHT+15,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+16,LEFT-9);
    addstr("|           quit(Q)          |");
    move(HEIGHT+17,LEFT-9);
    addstr("------------------------------");
    
    move(HEIGHT+20,LEFT-1);
    addstr("Type a menu: ");

    refresh();

    switch(getch()){
        case 'r': case'R': /*등록 화면으로*/break;
        case 'j': case'J': /*접속*/break;
        case 'q': case'Q': /*종료*/break;
    }

    if(getch()=='\n'){
        sleep(1);
    }
    
    endwin();

    return 0;
}