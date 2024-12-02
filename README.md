# SYS-DEVTOK
리눅스 환경의 타이머 기능을 활용하여 개발자 및 학생들의 공부 및 개발을 돕는 타이머 앱 개발 (KNU / SystemProgramming Team Project)

# DevTok (데브톡)
- Develop + Tick-Tock (시계 초침소리)
## 개발자들이 주로 사용하는 타이머 앱

## 주요 기능
타이머 기능을 활용하여 공부시간 기록, 공부시간과 쉬는시간을 번갈아 재거나 사용자 등록을 하여 사용자별로 누적 공부시간 기록하기. 그 외에도 공부시간 동안에는 화이트 노이즈 발생 등의 옵션 만들기

## 기능 구현
1. 보기 편하게 아스키 아트
2. 타이머 기능
3. 타이머가 끝난 후에 코멘트
4. 스톱워치 기능
5. 사용자 이름으로 txt파일
6. 파일 안에는 누적 시간, 연속 시간
7. 화이트 노이즈 발생
8. 일정시간 동안 활동 없을 경우 알림

## 폴더 구조
```
.(root)/
├── main.c                  # 메인 동작 파일
├── ranking.c               # 순위 정렬 & 사용자 기록 접근 알고리즘
├── ranking.h               # 순위 정렬 & 사용자 기록 접근 알고리즘 헤더파일
├── ranking.txt             # 유저 전체 순위 기록 파일
└── users/                  # 유저 기록 폴더
    ├── (username).txt      # 유저 개별 기록 파일
    └── ...
```
