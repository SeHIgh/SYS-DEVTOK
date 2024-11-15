DevTok 메뉴얼

- 사용자 정보 저장 파일
    예시) (username).txt 파일
    (누적 공부시간) (연속 공부시간)
    (공부 시작 일자) (공부시간) (코멘트)
    (공부 시작 일자) (공부시간) (코멘트)
    (공부 시작 일자) (공부시간) (코멘트)
    (공부 시작 일자) (공부시간) (코멘트)
    ...


- 사용자 순위 (누적 시간 기준) 파일
    예시) userRanking.txt 파일
    (등수) (사용자 이름) (누적 공부시간) (연속 공부시간)
    (등수) (사용자 이름) (누적 공부시간) (연속 공부시간)
    (등수) (사용자 이름) (누적 공부시간) (연속 공부시간)
    (등수) (사용자 이름) (누적 공부시간) (연속 공부시간)
    ...

- 사용자 순위 파일 작동 방식
    1. 프로그램 시작 시 파일 읽어서 저장
    2. 사용자 공부 끝낼 때마다 공부시간을 순위에 반영하여 파일 수정
    3. 누적 시간 기준으로 정렬
    4. 기록 보기에 들어가면 저장된 순위 출력

- 세부 알고리즘
    compare_users 함수:
        1. 누적 공부 시간을 먼저 비교
        2. 누적 공부 시간이 같을 경우, 연속 공부 시간을 기준으로 내림차순 정렬
        (userB->continuous_time - userA->continuous_time)
        을 통해 연속 공부 시간이 더 큰 유저가 높은 순위를 갖도록 정렬
        3. 연속 공부 시간까지 같다면, 이름 순서에 관계없이 첫 번째로 등장한 유저가 더 높은 순위 유지
    
- 알고리즘 수정
    1. quick sort -> insertion sort
        - qsort 는 정렬되지 않은 케이스가 많은 전체 데이터를 정렬 시에 적합 
        - insertion sort 는 변화가 많지 않은 케이스를 정렬 시 적합

    2. insertion sort -> hash function + partial insertion sort
        케이스 특성 상, 누적 시간은 커지면 커졌지 작아지진 않기 때문에 등수 변화가 오직 위로만 이루어짐
        -> 즉, 아래 등수의 누적시간과 비교는 필요가 없음
        - insertion sort 방식도 결국엔 필요없는 부분도 비교를 하게 됨 (아랫등수와 비교)
        - hash function 을 통해 사용자 이름에 맞는 인덱스에 상수시간으로 접근
        - partial insertion sort 를 이용해 접근한 인덱스의 윗등수(아래 인덱스)와만 insertion sort를 진행

- 일자 받아오기
    1. 그냥 받아오기
        $ date
        2024. 11. 11. (월) 13:57:08 KST

    2. 포맷 정해서 받아오기
        $ date +"%y%m%d%H%M%S"
        241111135708


- 컴파일 방법
    1. 같은 디렉토리 상에 존재 할 때
    예시) 실행할 파일 : ranking.c, main.c
    
    $gcc main.c ranking.c -o (실행파일이름)

    두 소스코드가 함께 컴파일 되면서, ranking.h 파일의 함수 및 구조체 선언도 컴파일에 자동 포함됨

    2. 다른 디렉토리 상에 존재할 때 (.을 root 라 가정)
    예시) 실행할 파일 : ./algorithm/(ranking.c, ranking.h) , ./main.c

    $gcc -Ialgorithm -Wall main.c algorithm/ranking.c -o main

    -Ialgorithm 옵션을 넣어줌으로써 algorithm 폴더 내에 header파일이 정의 되어있다는 것을 알려줌
    -> main.c 에서 세부 경로 적을 필요 없이 #include "ranking.h" 로 하면됨

- 디버깅 방법
    -1. DDEBUG 옵션 추가

    $gcc -DDEBUG -Ialgorithm -Wall main.c algorithm/ranking.c -o main

    -2. #define DEBUG 추가