#ifndef RANKING_H
#define RANKING_H

#include <stdio.h>

#define MAX_USERS 100
#define MAX_NAME_LENGTH 50
#define HASH_SIZE 100

// 유저 구조체 정의
typedef struct {
    int rank;
    char name[MAX_NAME_LENGTH];
    int cumulative_time;
    int continuous_time;
} User;

extern User users[MAX_USERS];
extern int user_count;

// 해시 구조체 정의
typedef struct {
    char name[MAX_NAME_LENGTH];
    int index;
} HashEntry;

extern HashEntry name_index[HASH_SIZE];

// 함수 프로토타입
int load_users(const char *filename);
int load_user_data(const char *filename, int *cumulative_time, int *continuous_time);
void save_users(const char *filename);
void add_or_update_user(const char *name, int cumulative_time, int continuous_time);
int find_user_index(const char *name);
void update_name_index();
unsigned int hash_function(const char *str);
void insert_and_sort(User *user_array, int count);
void update_and_save_ranking(const char *ranking_filename, const char *user_filename, const char *user_name);
void print_all_users();
void print_user_info(const char *name);

#endif // RANKING_H