/**
 * register.c
 * 접속 시 메뉴 3개(등록, 접속, 나가기) 띄움
 * 등록 시 사용자 이름 파일 생성
 */

#include <stdio.h>
#include <termios.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ino_t get_inode(char *);
void printMenu();
void tty_mode(int);
void turnOffEchoAndIcanon();
int find_filename(char *);

int main() {
	char choice;
	char* filename;
	int flag = 0; // 프로그램 종료 인식

	// 기존 정보 백업
	tty_mode(0);

	// 시작 시 ECHO, ICANON bit 끄기
	turnOffEchoAndIcanon();


	while (1) {
		printMenu();

		choice = getchar();
		printf("\n");

		switch (choice) {
			case '1':
				printf("등록하실 사용자 이름을 입력해주세요(1 ~ 256 bytes): ");
				
				// 이름 입력받을땐 잠깐 설정 되돌리기
				tty_mode(1);
				scanf("%s", filename);

				if (find_filename(filename)) {
					// 등록 실패
					// 이미 있는 사용자 이름
					printf("이미 있는 이름입니다!\n");
				} else {
					// 등록 성공
					// 사용자 파일 만들어주기
					chdir("storage");
					creat(filename, 0777);
					printf("등록 성공!\n");
					chdir("..");
				}

				tty_mode(0);
				turnOffEchoAndIcanon();

				break;

			case '2':
				printf("접속하실 사용자 이름을 입력해주세요(1 ~ 256 bytes): ");
				
				// 이름 입력받을땐 잠깐 설정 되돌리기
				tty_mode(1);
				scanf("%s", filename);

				if (find_filename(filename)) {
					// 접속 성공
					printf("접속 성공!\n");
				} else {
					printf("존재하지 않는 사용자입니다!\n");
				}

				tty_mode(0);
				turnOffEchoAndIcanon();

				break;

			case '3':
				flag = 1;
				break;
		}
		if (flag == 1)
			break;
		
		choice = getchar();
	}

	// 복원 후 프로그램 종료
	tty_mode(1);
	printf("프로그램 종료\n");
	return 0;
}

void turnOffEchoAndIcanon() {
	// ECHO and ICANON bit 끄기
	struct termios info;

	tcgetattr(0, &info);
	info.c_lflag &= ~ECHO;
	info.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &info);
}

int find_filename(char* filename) {
	// 저장소 접속 후 이름이 filename인 파일이 있으면 1, 없으면 0 반환

	DIR *dir_ptr = NULL;
	struct dirent *dirent_ptr = NULL;

	filename = strcat(filename, ".txt");

	// 저장소 접속(없으면 생성)
	if ((dir_ptr = opendir("storage")) == NULL) {
		if (mkdir("storage", 0777) == -1) {
			exit(EXIT_FAILURE);
		}
		dir_ptr = opendir("storage");
	}

	while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
		if (strcmp(dirent_ptr->d_name, filename) == 0) {
			return 1;
		}
	}

	closedir(dir_ptr);
	return 0;
}

void tty_mode(int how) {
	static struct termios orig_mode;
	if (how == 0) {
		tcgetattr(0, &orig_mode);
	} else if (how == 1) {
		tcsetattr(0, TCSANOW, &orig_mode);
	}
}

void printMenu() {
	printf("----------------------\n");
	printf("1. Register\n");
	printf("2. Login\n");
	printf("3. quit\n");
	printf("----------------------\n");
}