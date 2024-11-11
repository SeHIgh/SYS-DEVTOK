#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 20
#define MAX_USERS 100

typedef struct
{
    int rank;
    char name[MAX_NAME_LENGTH];
    int cumulative_time;
    int continuous_time;
} User;

// 등수를 기준으로 정렬할 함수
int compare_users(const void *a, const void *b)
{
    User *userA = (User *)a;
    User *userB = (User *)b;

    // 누적 공부시간을 기준으로 먼저 비교
    if (userA->cumulative_time != userB->cumulative_time)
    {
        return userB->cumulative_time - userA->cumulative_time;
    }
    else
    {
        // 누적 공부시간이 같다면 연속 공부시간을 기준으로 비교
        return userB->continuous_time - userA->continuous_time;
    }
}

// 파일에서 유저 데이터를 불러오는 함수
int load_users(const char *filename, User users[])
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("파일 열기 실패");
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%d %s %d %d", &users[count].rank, users[count].name,
                  &users[count].cumulative_time, &users[count].continuous_time) == 4)
    {
        count++;
    }
    fclose(file);
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
void save_users(const char *filename, User users[], int count)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("파일 쓰기 실패");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%d %s %d %d\n", i + 1, users[i].name,
                users[i].cumulative_time, users[i].continuous_time);
    }
    fclose(file);
}

// 유저를 추가하고 정렬 및 등수 갱신
void add_or_update_user(User users[], int *count, const char *name, int cumulative_time, int continuous_time)
{
    for (int i = 0; i < *count; i++)
    {
        if (strcmp(users[i].name, name) == 0)
        {
            users[i].cumulative_time += cumulative_time;
            users[i].continuous_time = continuous_time;
            qsort(users, *count, sizeof(User), compare_users);
            return;
        }
    }

    User new_user = {0, "", cumulative_time, continuous_time};
    strncpy(new_user.name, name, MAX_NAME_LENGTH - 1);
    users[(*count)++] = new_user;

    qsort(users, *count, sizeof(User), compare_users);
}

int main()
{
    User users[MAX_USERS];
    int user_count = load_users("ranking.txt", users);
    if (user_count < 0)
        return 1;

    char new_name[] = "user9";
    char user_filename[MAX_NAME_LENGTH + 4];
    snprintf(user_filename, sizeof(user_filename), "%s.txt", new_name);

    int new_cumulative_time, new_continuous_time;
    if (load_user_data(user_filename, &new_cumulative_time, &new_continuous_time) < 0)
        return 1;

    add_or_update_user(users, &user_count, new_name, new_cumulative_time, new_continuous_time);

    save_users("ranking.txt", users, user_count);

    return 0;
}