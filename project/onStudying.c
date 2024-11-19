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

    char* title = "On Studying...";
    char* stop = ">> STOP(Enter) <<";
    int barSize = 28;

    //박스 크기 설정
    int boxheight = 3;
    int boxwidth = 30;

    //중앙 좌표 계산
    int starty = (height-boxheight)/2; //중앙 Y좌표
    int startx = (width-boxwidth)/2;   //중앙 X좌표

    //Title
    mvprintw(starty-5, width/2-strlen(title)/2, title);

    //새로운 WINDOW 생성
    WINDOW* win_1 = newwin(boxheight, boxwidth, starty, startx);
    box(win_1,'|','-');//box 그리기

    WINDOW* win_2 = newwin(boxheight, boxwidth, starty+5, startx);
    mvwaddstr(win_2,1,boxwidth/2-strlen(stop)/2, stop);
    refresh();
    wrefresh(win_2);

    int c;
    keypad(stdscr, TRUE); //특수 키 입력 활성화

    //공부중
    while(1){
        for(int i=0;i<barSize;i++){
            wstandout(win_1);
            wmove(win_1,1,i+1);
            waddstr(win_1, "/");
            refresh();
            wrefresh(win_1);
            sleep(1);

            //키 입력 감지
            if((c=getch())=='\n'){
                goto end_loop; //엔터키가 눌리면 반복문 종료
            }

        }

        wmove(win_1,1,1);
        wstandend(win_1);
        waddstr(win_1, "                            ");
    }

    refresh();
    wrefresh(win_1);

    getchar();

    //ncurses 종료
    endwin();

end_loop:
    delwin(win_1); //윈도우 삭제
    endwin(); //ncurses 종료

    return 0;
}