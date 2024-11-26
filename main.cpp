#include "main.h"


int main(){

    // test code
    for(int i=0; i<22; i++){
        container_add({N-1, M-1}, 1);
        auto test = container_organize();
        apply_commands(test, false); 
    }
    print_grid();
    container_remove({0,2});
    container_remove({0,3});
    container_remove({0,4});
    container_remove({1,4});
    container_remove({2,4});
    print_grid();
    apply_commands(container_move(5, {N-1, M-1}), false);
    print_grid();
    apply_commands(container_make_empty({4,0}), false);
    print_grid();
    container_all_remove();
}

vector<commands> time_slice_command(commands cmds){
    vector<commands> res;
    vector<bool> visit_cmd(cmds.size(), false);
    queue<pair<int,int>> lazy_apply;
    map<int, position> position_map;
    int grid[N][M];
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            if(space[i][j] == NULL) grid[i][j] = -1;
            else {
                grid[i][j] = space[i][j]->id;
                position_map[space[i][j]->id] = {i,j};
            }
        }
    }
    while(1){
        commands tmp_cmds;
        vector<bool> tmp_visit_id(MAX_CONTAINER + 1, false);
        for(int i=0; i<cmds.size(); i++){
            if(!visit_cmd[i]){
                command cmd = cmds[i];
                int id = cmd.id;
                int dir = cmd.dir;
                int x = position_map[id].x;
                int y = position_map[id].y;
                int nx = x+dir_arr[dir][0];
                int ny = y+dir_arr[dir][1];
                if(grid[nx][ny] == -1 && tmp_visit_id[id] == false){
                    grid[nx][ny] = id;
                    visit_cmd[i] = true;
                    tmp_cmds.push_back(cmd);
                    lazy_apply.push({x,y});
                    tmp_visit_id[id] = true;
                    position_map[id].x = nx;
                    position_map[id].y = ny;
                }
            }
        }
        if(tmp_cmds.empty()) break;
        res.push_back(tmp_cmds);
        while(!lazy_apply.empty()){
            grid[lazy_apply.front().first][lazy_apply.front().second] = -1;
            lazy_apply.pop();
        }
    }
    return res;
}

void apply_commands(vector<commands> vcmds, bool debug){
    for(commands cmds : vcmds){
        for(command cmd : cmds){
            if(debug){
                printf("[%d ",cmd.id);
                switch(cmd.dir){
                    case UP:
                        printf("UP] ");
                        break;
                    case DOWN:
                        printf("DOWN] ");
                        break;
                    case LEFT:
                        printf("LEFT] ");
                        break;
                    case RIGHT:
                        printf("RIGHT] ");
                        break;
                }
            }
            container* ct = container_map[cmd.id];
            int x = ct->p->x, y = ct->p->y;
            int nx = x + dir_arr[cmd.dir][0], ny = y + dir_arr[cmd.dir][1];

            ct->p->x = nx; ct->p->y = ny;
            if(space[nx][ny] != NULL){
                ERROR("apply : conflect container");
            }
            space[x][y] = NULL;
            space[nx][ny] = ct;
        }
        if(debug){
            cout<<'\n';
            print_grid();
        }
    }
}


bool container_add(position pos, int type){
    if(container_cnt >= MAX_CONTAINER){
        WARNING("add : MAX CONTAINER");
        return false;
    }
    if(space[pos.x][pos.y] != NULL){
        WARNING("add : NOT ORGANIZE");
        return false;
    }
    container* ct = (container*)malloc(sizeof(container));
    if(ct == NULL){
        ERROR("add : memory allocation error");
        return false;
    }
    ct->id = next_id;
    ct->p = (position*)malloc(sizeof(position));
    ct->p->x = pos.x;
    ct->p->y = pos.y;
    ct->type_id = type;
    container_map[ct->id] = ct;
    space[pos.x][pos.y] = ct;
    
    if(priority_map.find(type) == priority_map.end()){
        priority_map[type] = 0;
    }

    container_cnt ++;
    while(use_id[(next_id=(next_id+1)%(MAX_CONTAINER+1))]);

    return true;
}

vector<commands> container_organize(){
    
    commands paths;
    
    // 복사
    container* copy_space[N][M];
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){ 
            copy_space[i][j] = space[i][j];
        }
    }

    for(int i=0; i<=(container_cnt-1) / N; i++){
        for(int j=0; j<M; j++){
            if(i == (container_cnt-1) / N && (container_cnt-1)%M < j) break;

            if(copy_space[i][j] == NULL){
                int min_priority = 2e8;
                container* target_container = NULL;
                commands path;

                queue<pair<position, stack<int>>> bfs;
                bfs.push({{i,j}, stack<int>()});
                bool visited[N][M] = {};
                visited[i][j] = true;
                while(!bfs.empty()){
                    int x = bfs.front().first.x;
                    int y = bfs.front().first.y;
                    stack<int> now_st = bfs.front().second;
                    bfs.pop();
                    for(int d = 0; d < 4; d++){
                        int nx = x + dir_arr[d][0];
                        int ny = y + dir_arr[d][1];
                        stack<int> st = now_st;
                        if(nx < 0 || ny < 0 || nx >= N || ny >= M || nx < i || (nx == i && ny < j) || visited[nx][ny]) continue;

                        st.push(d);
                        visited[nx][ny] = true;
                        // 빈공간이라면 탐색범위 추가
                        if(copy_space[nx][ny] == NULL){
                            bfs.push({{nx, ny}, st});
                        }
                        else{
                            container* nct = copy_space[nx][ny];
                            int np = priority_map[nct->type_id];
                            if(np < min_priority){
                                min_priority = np;
                                target_container = nct;
                                path.clear();

                                while(!st.empty()){
                                    int di = st.top();
                                    st.pop();
                                    di = di==0?1:(di==1?0:(di==2?3:2));
                                    path.push_back({nct->id, di});
                                }
                            }
                        }
                    }   
                }
                if(target_container != NULL){
                    for(command cmd : path){
                        paths.push_back(cmd);
                    }
                    copy_space[target_container->p->x][target_container->p->y] = NULL;
                    copy_space[i][j] = target_container;
                }
            }
        }
    }
    return time_slice_command(paths);
}

vector<commands> container_move(int id, position pos){
    
    commands paths;
    container* main_ct = container_map[id];
    position* main_pos = main_ct->p;
    
    // 가장 다른 컨테이너에 덜 막히는 경로 탐색
    queue<pair<position, pair<int, commands>>> bfs;
    bfs.push({*main_pos, {0, commands()}});
    vector<vector<int>> visited(N, vector<int>(M, 2e8));
    int min_val = 2e8;
    commands main_path;
    while(!bfs.empty()){
        position now_pos = bfs.front().first;
        int value = bfs.front().second.first;
        commands now_path = bfs.front().second.second;
        bfs.pop();
        if(value >= min_val) continue;
        if(now_pos.x == pos.x && now_pos.y == pos.y){
            min_val = value;
            main_path = now_path;
            continue;
        }
        for(int i=0; i<4; i++){
            position next_pos = {now_pos.x + dir_arr[i][0], now_pos.y + dir_arr[i][1]};
            if(next_pos.x < 0 || next_pos.y < 0 || next_pos.x >= N || next_pos.y >= M) continue;
            int next_value = space[next_pos.x][next_pos.y] == NULL ? value : value+1;
            if(visited[next_pos.x][next_pos.y] <= next_value) continue;
            visited[next_pos.x][next_pos.y] = next_value;
            commands tmp_path = now_path;
            tmp_path.push_back({id, i});
            
            bfs.push({next_pos, {next_value, tmp_path}});
        }
    }
    // print_cmd(vector<commands>(1, main_path));
    // main_path: main 컨테이너가 가야하는 길, 가는 길에 컨테이너 있으면 비키도록
    int x = main_pos->x;
    int y = main_pos->y;
    int grid[N][M];
    map<int, position> position_map;
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            if(space[i][j] == NULL) grid[i][j] = -1;
            else {
                grid[i][j] = space[i][j]->id;
                position_map[space[i][j]->id] = {i,j};
            }
        }
    }
    for(command mpath : main_path){
        int nx = x+dir_arr[mpath.dir][0], ny = y+dir_arr[mpath.dir][1];
        // 컨테이너 있으면, 가장 짧은 동선으로 비켜주도록
        if(grid[nx][ny] != -1){
            // {nx, ny}로부터 가장 가까운 빈칸 찾기
            int min_distance = 2e8;
            commands min_dis_blank_path;
            
            queue<pair<position, pair<int, commands>>> bfs;
            vector<vector<int>> visited(N, vector<int>(M, 2e8));
            bfs.push({{nx, ny}, {0, commands()}});
            visited[nx][ny] = -1;
            visited[x][y] = -1;
            while(!bfs.empty()){
                position now_pos = bfs.front().first;
                int distance = bfs.front().second.first;
                commands path = bfs.front().second.second;
                bfs.pop();
                if(distance >= min_distance) continue;
                if(grid[now_pos.x][now_pos.y] == -1){
                    min_distance = distance;
                    min_dis_blank_path = path;
                    continue;
                }
                for(int i=0; i<4; i++){
                    position next_pos = {now_pos.x + dir_arr[i][0], now_pos.y + dir_arr[i][1]};
                    if(next_pos.x < 0 || next_pos.y < 0 || next_pos.x >= N || next_pos.y >= M) continue;
                    int next_distance = distance+1;
                    if(visited[next_pos.x][next_pos.y] <= next_distance) continue;
                    visited[next_pos.x][next_pos.y] = next_distance;
                    commands tmp_path = path;
                    tmp_path.push_back({grid[now_pos.x][now_pos.y], i});
                    bfs.push({next_pos, {next_distance, tmp_path}});
                }
            }
            for(int i=min_dis_blank_path.size()-1; i>=0; i--){
                command mdpath = min_dis_blank_path[i];
                paths.push_back(mdpath);
                container* mdct = container_map[mdpath.id];
                int mdctx = position_map[mdpath.id].x;
                int mdcty = position_map[mdpath.id].y;
                grid[mdctx][mdcty] = -1;
                grid[mdctx + dir_arr[mdpath.dir][0]][mdcty + dir_arr[mdpath.dir][1]] = mdpath.id;
                position_map[mdpath.id] = {mdctx + dir_arr[mdpath.dir][0], mdcty + dir_arr[mdpath.dir][1]};
            }

        }
        paths.push_back(mpath);
        grid[x][y] = -1;
        grid[nx][ny] = id;
        x = nx;
        y = ny;
    }
    // print_cmd(vector<commands>(1, paths));
    return time_slice_command(paths);
}

vector<commands> container_make_empty(position pos){
    commands paths;
    // pos로부터 가장 가까운 빈칸 찾기
    int min_distance = 2e8;
    commands min_dis_blank_path;
    
    queue<pair<position, pair<int, commands>>> bfs;
    vector<vector<int>> visited(N, vector<int>(M, 2e8));

    bfs.push({pos, {0, commands()}});
    visited[pos.x][pos.y] = -1;
    while(!bfs.empty()){
        position now_pos = bfs.front().first;
        int distance = bfs.front().second.first;
        commands path = bfs.front().second.second;
        bfs.pop();
        if(distance >= min_distance) continue;
        if(space[now_pos.x][now_pos.y] == NULL){
            min_distance = distance;
            min_dis_blank_path = path;
            continue;
        }
        for(int i=0; i<4; i++){
            position next_pos = {now_pos.x + dir_arr[i][0], now_pos.y + dir_arr[i][1]};
            if(next_pos.x < 0 || next_pos.y < 0 || next_pos.x >= N || next_pos.y >= M) continue;
            int next_distance = distance+1;
            if(visited[next_pos.x][next_pos.y] <= next_distance) continue;
            visited[next_pos.x][next_pos.y] = next_distance;
            commands tmp_path = path;
            tmp_path.push_back({space[now_pos.x][now_pos.y]->id, i});
            bfs.push({next_pos, {next_distance, tmp_path}});
        }
    }
    for(int i=min_dis_blank_path.size()-1; i>=0; i--){
        command mdpath = min_dis_blank_path[i];
        paths.push_back(mdpath);
    }

    return time_slice_command(paths);
}

bool container_remove(position pos){
    if(space[pos.x][pos.y] == NULL){
        WARNING("remove : Attempted to release non-existent memory");
        return false;
    }
    container* ct = space[pos.x][pos.y];
    
    // 제거하는 컨테이너는 우선순위 상승
    priority_map[ct->type_id]++;
    
    int id = ct->id;
    container_map.erase(id);
    space[pos.x][pos.y] = NULL;
    container_cnt -= 1;
    free(ct->p);
    free(ct);

    return true;
}

void container_all_remove(){
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            if(space[i][j] != NULL){
                container_remove({i,j});
            }
        }
    }
}

vector<vector<container>> get_map(){
    vector<vector<container>> res(N, vector<container>(M));
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            res[i][j] = *space[i][j];
        }
    }
    return res;
}


void print_grid(){
    for(int i=0; i<N; i++){
        cout<<"---";
    }cout<<'\n';
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            if(space[i][j] != NULL){
                printf("%2d ", space[i][j]->id);
            }else{
                cout<<"   ";
            }
        }cout<<'\n';
    }
    for(int i=0; i<N; i++){
        cout<<"---";
    }
    cout<<'\n';
}

void print_cmd(vector<commands> cmds){
    for(int i=0; i<cmds.size(); i++){
        auto cmd = cmds[i];
        printf("t:%d\n",i); 
        for(auto c : cmd){
            printf("[%d ",c.id);
            switch(c.dir){
                case UP:
                    printf("UP] ");
                    break;
                case DOWN:
                    printf("DOWN] ");
                    break;
                case LEFT:
                    printf("LEFT] ");
                    break;
                case RIGHT:
                    printf("RIGHT] ");
                    break;
            }
        }printf("\n");
    }
}