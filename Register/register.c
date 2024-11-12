/**
 * register.c
 * ���� �� �޴� 3��(���, ����, ������) ���
 * ��� �� ����� �̸� ���� ����
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
	int flag = 0; // ���α׷� ���� �ν�

	// ���� ���� ���
	tty_mode(0);

	// ���� �� ECHO, ICANON bit ����
	turnOffEchoAndIcanon();


	while (1) {
		printMenu();

		choice = getchar();
		printf("\n");

		switch (choice) {
			case '1':
				printf("����Ͻ� ����� �̸��� �Է����ּ���(1 ~ 256 bytes): ");
				
				// �̸� �Է¹����� ��� ���� �ǵ�����
				tty_mode(1);
				scanf("%s", filename);

				if (find_filename(filename)) {
					// ��� ����
					// �̹� �ִ� ����� �̸�
					printf("�̹� �ִ� �̸��Դϴ�!\n");
				} else {
					// ��� ����
					// ����� ���� ������ֱ�
					chdir("storage");
					creat(filename, 0777);
					printf("��� ����!\n");
					chdir("..");
				}

				tty_mode(0);
				turnOffEchoAndIcanon();

				break;

			case '2':
				printf("�����Ͻ� ����� �̸��� �Է����ּ���(1 ~ 256 bytes): ");
				
				// �̸� �Է¹����� ��� ���� �ǵ�����
				tty_mode(1);
				scanf("%s", filename);

				if (find_filename(filename)) {
					// ���� ����
					printf("���� ����!\n");
				} else {
					printf("�������� �ʴ� ������Դϴ�!\n");
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

	// ���� �� ���α׷� ����
	tty_mode(1);
	printf("���α׷� ����\n");
	return 0;
}

void turnOffEchoAndIcanon() {
	// ECHO and ICANON bit ����
	struct termios info;

	tcgetattr(0, &info);
	info.c_lflag &= ~ECHO;
	info.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &info);
}

int find_filename(char* filename) {
	// ����� ���� �� �̸��� filename�� ������ ������ 1, ������ 0 ��ȯ

	DIR *dir_ptr = NULL;
	struct dirent *dirent_ptr = NULL;

	filename = strcat(filename, ".txt");

	// ����� ����(������ ����)
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