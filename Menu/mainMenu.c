#include <stdio.h>
#include <curses.h>
#include <unistd.h>

#define LEFT 50
#define HEIGHT 5

int main(){
    
    initscr();
    clear();

    move(HEIGHT,LEFT-3);
    addstr("Welcome To DevTok");

    move(HEIGHT+3,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+4,LEFT-9);
    addstr("|          study(S)          |");
    move(HEIGHT+5,LEFT-9);
    addstr("------------------------------");

    move(HEIGHT+7,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+8,LEFT-9);
    addstr("|          record(R)         |");
    move(HEIGHT+9,LEFT-9);
    addstr("------------------------------");

    move(HEIGHT+11,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+12,LEFT-9);
    addstr("|          option(O)         |");
    move(HEIGHT+13,LEFT-9);
    addstr("------------------------------");
    
    move(HEIGHT+15,LEFT-9);
    addstr("------------------------------");
    move(HEIGHT+16,LEFT-9);
    addstr("|     back to StartMenu(B)   |");
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