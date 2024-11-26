#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<queue>
#include<stack>
#include<stdlib.h>
#include "log.h"

// #include <pybind11/pybind11.h>
using namespace std;



#define N 5
#define M 5
#define MAX_CONTAINER (N*M-1)

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

typedef struct position{
    int x;
    int y;
} position;

typedef struct container{
    int id;
    position* p; 
    int type_id;
} container;

typedef struct command{
    int id;
    int dir;
} command;

typedef vector<command> commands;

container* space[N][M];

map<int, int> priority_map;
map<int, container*> container_map;

int next_id = 0;
int container_cnt = 0;
bool use_id[MAX_CONTAINER+1];

int dir_arr[4][2] = {{-1, 0}, {1, 0}, {0,-1}, {0, 1}};


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


void print_grid();
void print_cmd(vector<commands> cmds);

vector<vector<container>> get_map();