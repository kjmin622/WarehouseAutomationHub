#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <stdlib.h>
#include "log.h"
using namespace std;

#define N 5
#define M 5
#define MAX_CONTAINER (N * M - 1)

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

typedef struct position
{
    int x;
    int y;
} position;

typedef struct container
{
    int id;
    position *p;
    int type_id;
} container;

typedef struct command
{
    int id;
    int dir;
} command;

typedef vector<command> commands;

extern container *space[N][M];

extern map<int, int> priority_map;
extern map<int, container *> container_map;

extern int next_id;
extern int container_cnt;
extern bool use_id[MAX_CONTAINER + 1];

extern int dir_arr[4][2];

// 특정 위치에 컨테이너 추가
bool container_add(position pos, int type);

// 특정 위치의 컨테이너 삭제
bool container_remove(position pos);

// 컨테이너 정리
vector<commands> container_organize();

// 컨테이너 특정 위치로 이동
vector<commands> container_move(int id, position pos);

// 특정 위치에 빈 공간 만들기
vector<commands> container_make_empty(position pos);

// 커맨드 적용
void apply_commands(vector<commands> vcmds, bool debug);

void container_all_remove();

vector<vector<pair<int, int>>> get_map();

void print_grid();
void print_cmd(vector<commands> cmds);
