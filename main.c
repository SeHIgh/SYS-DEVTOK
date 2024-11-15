#include "ranking.h"
#include <stdio.h>

int main() {
    // 사용자 이름 입력받기
    char user_name[MAX_NAME_LENGTH];
    printf("사용자 이름을 입력하세요: ");
    scanf("%s", user_name);

    // 랭킹 파일과 유저 파일 경로 설정
    const char *ranking_filename = "ranking.txt";
    char user_filename[MAX_NAME_LENGTH + 20];
    snprintf(user_filename, sizeof(user_filename), "users/%s.txt", user_name);

    // 랭킹 업데이트 및 저장
    update_and_save_ranking(ranking_filename, user_filename, user_name);

    // 사용자 정보 출력
    print_user_info(user_name);

    // 전체 사용자 목록 출력
    print_all_users();

    return 0;
}