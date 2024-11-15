#include "ranking.h"
#include <string.h>
#include <stdlib.h>

#define DEBUG

User users[MAX_USERS];
int user_count = 0;
HashEntry name_index[HASH_SIZE];

// 등수와 사용자 정보를 출력하는 함수
void print_all_users()
{
    printf("\n전체 사용자 목록 (누적 시간[초] 등수 순):\n\n");
    printf("+-------------------------------------+\n");
    printf("| %s | %-8s | %-10s | %-10s |\n", "등수", "이름", "누적", "연속");
    printf("+=====================================+\n");

    for (int i = 0; i < user_count; i++)
    {
        printf("| %-4d | %-6s | %-8d | %-8d |\n",
               users[i].rank, users[i].name, users[i].cumulative_time, users[i].continuous_time);
    }
    printf("+-------------------------------------+\n");
}

// 특정 사용자의 정보를 출력하는 함수
void print_user_info(const char *name)
{
    int index = find_user_index(name);
    if (index >= 0)
    {
        printf("\n사용자 %s 의 정보 (초 단위):\n\n", name);
        printf("+----------------+\n");
        printf("| %-6s | %-8s |\n", "항목", "값");
        printf("+================+\n");
        printf("| %-6s | %-7d |\n", "등수", users[index].rank);
        printf("| %-6s | %-7d |\n", "누적", users[index].cumulative_time);
        printf("| %-6s | %-7d |\n", "연속", users[index].continuous_time);
        printf("+----------------+\n");
    }
    else
    {
        printf("사용자 %s를 찾을 수 없습니다.\n", name);
    }
}

// 등수를 기준으로 삽입 정렬을 수행하는 함수
void insert_and_sort(User *user_array, int count)
{
    for (int i = count - 1; i > 0; i--)
    {
        if (user_array[i].cumulative_time > user_array[i - 1].cumulative_time ||
            (user_array[i].cumulative_time == user_array[i - 1].cumulative_time &&
             user_array[i].continuous_time > user_array[i - 1].continuous_time))
        {
            // 사용자 swap
            User temp = user_array[i];
            user_array[i] = user_array[i - 1];
            user_array[i - 1] = temp;
#ifdef DEBUG
            // 디버깅 출력: 스왑 후 배열 상태
            printf("스왑 후 배열 상태:\n");
            for (int j = 0; j < count; j++)
            {
                printf("등수: %d, 이름: %s, 누적: %d, 연속: %d\n",
                       user_array[j].rank, user_array[j].name,
                       user_array[j].cumulative_time, user_array[j].continuous_time);
            }
#endif
        }
        else
        {
            break;
        }
    }

    // 순위 갱신 (1부터 시작하도록)
    for (int i = 0; i < count; i++)
    {
        user_array[i].rank = i + 1; // 등수를 1부터 시작
    }
#ifdef DEBUG
    // 디버깅 출력: 순위 갱신 후 배열 상태
    printf("순위 갱신 후 배열 상태:\n");
    for (int i = 0; i < count; i++)
    {
        printf("등수: %d, 이름: %s, 누적: %d, 연속: %d\n",
               user_array[i].rank, user_array[i].name, user_array[i].cumulative_time, user_array[i].continuous_time);
    }
#endif
}

// 파일에서 유저 데이터를 불러오는 함수
int load_users(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        // 파일이 없는 경우 생성
        file = fopen(filename, "w");
        if (!file)
        {
            perror("파일 생성 실패");
            return -1;
        }
        fclose(file); // 생성 후 닫고, 다시 읽기 모드로 열기
        printf("랭킹 파일이 존재하지 않아 새로 생성되었습니다.\n");
        return 0; // 빈 목록을 반환 (유저 없음)
    }

    int count = 0;
    while (fscanf(file, "%d %s %d %d", &users[count].rank, users[count].name,
                  &users[count].cumulative_time, &users[count].continuous_time) == 4)
    {
        count++;
    }
    fclose(file);
    user_count = count;

    // 이름 인덱스 초기화
    update_name_index();

    return count;
}

// 특정 유저의 정보를 파일에서 불러오는 함수
int load_user_data(const char *filename, int *cumulative_time, int *continuous_time)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("새 유저 파일 열기 실패");
        return -1;
    }

    if (fscanf(file, "%d %d", cumulative_time, continuous_time) != 2)
    {
        perror("유저 데이터 읽기 실패");
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

// 업데이트된 유저 데이터를 파일에 저장하는 함수
void save_users(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("파일 쓰기 실패");
        return;
    }

    for (int i = 0; i < user_count; i++)
    {
        fprintf(file, "%d %s %d %d\n", i + 1, users[i].name,
                users[i].cumulative_time, users[i].continuous_time);
    }
    fclose(file);
}

// 유저를 추가하거나 업데이트하고, 순위와 이름 인덱스를 갱신하는 함수
void add_or_update_user(const char *name, int cumulative_time, int continuous_time)
{
    int index = find_user_index(name);

    if (index >= 0)
    {
        // 이미 존재하는 유저인 경우
        users[index].cumulative_time = cumulative_time;
        users[index].continuous_time = continuous_time;
    }
    else
    {
        // 새로운 유저 추가
        User new_user = {0, "", cumulative_time, continuous_time};
        strncpy(new_user.name, name, MAX_NAME_LENGTH - 1);
        users[user_count++] = new_user;
    }

    // 삽입 정렬을 통해 순위 갱신
    insert_and_sort(users, user_count);

    // 이름 인덱스 갱신
    update_name_index();
}

// 유저 이름으로 인덱스를 찾는 함수 (해시 테이블 사용)
int find_user_index(const char *name)
{
    unsigned int hash = hash_function(name);
    if (name_index[hash].index >= 0 && strcmp(name_index[hash].name, name) == 0)
    {
        return name_index[hash].index;
    }
    return -1;
}

// 이름 인덱스를 갱신하는 함수 (해시 테이블)
void update_name_index()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        name_index[i].index = -1;
    }
    for (int i = 0; i < user_count; i++)
    {
        unsigned int hash = hash_function(users[i].name);
        strncpy(name_index[hash].name, users[i].name, MAX_NAME_LENGTH - 1);
        name_index[hash].index = i;
    }
}

// 해시 함수
unsigned int hash_function(const char *str)
{
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// (main 에서 사용할 함수) 유저 데이터를 불러오고 랭킹을 업데이트하며 저장하는 통합 함수
void update_and_save_ranking(const char *ranking_filename, const char *user_filename, const char *user_name)
{
    // 유저 데이터 로드
    if (load_users(ranking_filename) < 0)
    {
        printf("랭킹 파일을 불러오는 데 실패했습니다.\n");
        return;
    }

    // 새 유저의 데이터 불러오기
    int new_cumulative_time, new_continuous_time;
    if (load_user_data(user_filename, &new_cumulative_time, &new_continuous_time) < 0)
    {
        printf("유저 데이터를 불러오는 데 실패했습니다.\n");
        return;
    }

    // 유저 정보 추가/업데이트 및 랭킹 갱신
    add_or_update_user(user_name, new_cumulative_time, new_continuous_time);

    // 갱신된 랭킹 정보를 파일에 저장
    save_users(ranking_filename);
}

// 테스트 코드
/*
int main()
{
    // 유저 데이터 로드
    if (load_users("../ranking.txt") < 0)
        return 1;

    // 새로운 유저 추가 또는 기존 유저 업데이트
    char new_name[] = "user1";
    char user_filename[MAX_NAME_LENGTH + 4];
    snprintf(user_filename, sizeof(user_filename), "../users/%s.txt", new_name);

    int new_cumulative_time, new_continuous_time;
    if (load_user_data(user_filename, &new_cumulative_time, &new_continuous_time) < 0)
        return 1;

    add_or_update_user(new_name, new_cumulative_time, new_continuous_time);

    // 업데이트된 랭킹 저장
    save_users("../ranking.txt");

    return 0;
}
*/