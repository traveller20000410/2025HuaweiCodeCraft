

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <unordered_set>
using namespace std;

#define MAX_DISK_NUM (10 + 1)
#define MAX_DISK_SIZE (16384 + 1)
#define MAX_REQUEST_NUM (30000000 + 1)
#define MAX_OBJECT_NUM (100000 + 1)
#define REP_NUM (3)
#define FRE_PER_SLICING (1800)
#define EXTRA_TIME (105)
#define MAX_TIME_SLICES (50)
int next_timestamp[MAX_TIME_SLICES] = {
0, 35, 40, 40, 26, 28, 26, 23, 26, 30,
29, 23, 15, 35, 29, 18, 15, 15, 30, 28,
30, 20, 18, 18, 23, 40, 30, 32, 32, 31, 
30, 23, 30, 30, 30, 30, 30, 30, 30, 30, 
30, 31, 31, 20, 40, 28, 32, 20, 35, 31, 
};
int get_next_timestamp_size(int timestamp) {
    int index = (timestamp- 0.1) / FRE_PER_SLICING + 1;
    return next_timestamp[index];
}

typedef struct Request_ {
    int object_id;
    int prev_id;
    bool is_done;
    int timestamp;
} Request;

typedef struct Object_ {
    int replica[REP_NUM + 1];
    int* unit[REP_NUM + 1];
    int size;
    int last_request_point;
    bool is_delete;
    int* read_flag;    
} Object;

typedef struct DiskState_ {
    int position = 1;
    int remain_token;
    int is_last_action_read;
}DiskState;

DiskState disk_state[MAX_DISK_NUM];
Request request[MAX_REQUEST_NUM];
Object object[MAX_OBJECT_NUM];

int T, M, N, V, G;
int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
int disk_point[MAX_DISK_NUM];

int max_index_arr[MAX_DISK_NUM] = {0};
int maxallpos[4];

int current_timestamp;
void timestamp_action()
{
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);
    current_timestamp = timestamp;
    fflush(stdout);
}

void do_object_delete(const int* object_unit, int* disk_unit, int size)
{
    for (int i = 1; i <= size; i++) {
        disk_unit[object_unit[i]] = 0;
    }
}

void delete_action()
{
    int n_delete;
    int abort_num = 0;
    static int _id[MAX_OBJECT_NUM];

    scanf("%d", &n_delete);
    for (int i = 1; i <= n_delete; i++) {
        scanf("%d", &_id[i]);
    }

    for (int i = 1; i <= n_delete; i++) {
        int id = _id[i];
        int current_id = object[id].last_request_point;
        while (current_id != 0) {
            if (request[current_id].is_done == false) {
                abort_num++;
            }
            current_id = request[current_id].prev_id;
        }
    }

    printf("%d\n", abort_num);
    for (int i = 1; i <= n_delete; i++) {
        int id = _id[i];
        int current_id = object[id].last_request_point;
        while (current_id != 0) {
            if (request[current_id].is_done == false) {
                printf("%d\n", current_id);
            }
            current_id = request[current_id].prev_id;
        }
        for (int j = 1; j <= REP_NUM; j++) {
            do_object_delete(object[id].unit[j], disk[object[id].replica[j]], object[id].size);
        }
        object[id].is_delete = true;
    }

    fflush(stdout);
}

void do_object_write(int* object_unit, int* disk_unit, int size, int object_id, int j)
{
    int start_pos = -1;
    int consecutive_free = 0;
    
    for (int i = (j - 1) * V / 3 + 1 - 80 * (j - 1); i <= V; i++) {
        if (disk_unit[i] == 0) {
            consecutive_free++;
            if (consecutive_free == 1) {
                start_pos = i;  
            }
            if (consecutive_free == size) {
                break;  
            }
        } else {
            consecutive_free = 0;  
            start_pos = -1;
        }
    }

    if (consecutive_free < size) {
        start_pos = -1;
        int current_write_point = 0;
        for (int i = (j - 1) * V / 3 + 1 - 80 * (j - 1); i <= V; i++) {
            if (disk_unit[i] == 0) {
                disk_unit[i] = object_id;
                object_unit[++current_write_point] = i;
                int disk_id = (object_id + 0) % N + 1;
                if(i > maxallpos[j])
                {
                    maxallpos[j] = i;
                }
                if (current_write_point == size) {
                    break;
                }
            }
        }
        assert(current_write_point == size);
    } else {
        for (int i = 0; i < size; i++) {
            disk_unit[start_pos + i] = object_id;
            object_unit[i + 1] = start_pos + i;
            int disk_id = (object_id + 0) % N + 1;
            if(start_pos + i > maxallpos[j]){
                maxallpos[j] = start_pos + i;
            }
        }
    }       
}

void write_action()
{
    int n_write;
    scanf("%d", &n_write);
    for (int i = 1; i <= n_write; i++) {
        int id, size, tag;
        scanf("%d%d%d", &id, &size, &tag);
        object[id].last_request_point = 0;
        for (int j = 1; j <= REP_NUM; j++) {
            object[id].replica[j] = (id + j) % N + 1;
            object[id].unit[j] = static_cast<int*>(malloc(sizeof(int) * (size + 1)));
            object[id].size = size;
            object[id].is_delete = false;
            object[id].read_flag = static_cast<int*>(malloc(sizeof(int) * (size + 1)));
            for (int k = 0; k <= size; k++) {
                object[id].read_flag[k] = 0;
            }
            do_object_write(object[id].unit[j], disk[object[id].replica[j]], size, id, j);
        }

        printf("%d\n", id);
        for (int j = 1; j <= REP_NUM; j++) {
            printf("%d", object[id].replica[j]);
            for (int k = 1; k <= size; k++) {
                printf(" %d", object[id].unit[j][k]);
            }
            printf("\n");
        }
    }

    fflush(stdout);
}

int jumpCounter = 0;
unordered_set<int> readed_object_id;
vector<int> completed_requests;
void read_action()
{
    int n_read;
    int request_id, object_id;
    scanf("%d", &n_read);
    for (int i = 1; i <= n_read; i++) {
        scanf("%d%d", &request_id, &object_id);
        request[request_id].object_id = object_id;
        request[request_id].prev_id = object[object_id].last_request_point;
        object[object_id].last_request_point = request_id;
        request[request_id].is_done = false;
        request[request_id].timestamp = current_timestamp;
    }

    for(int i = 1; i <= N; i++) {
        disk_state[i].remain_token = G;
        if(i == 1)
        {
            while(disk[i][maxallpos[3]] == 0 && disk[i + 1][maxallpos[3]] == 0 && disk[i + 2][maxallpos[3]] == 0 && disk[i + 3][maxallpos[3]] == 0 && disk[i + 4][maxallpos[3]] == 0 && disk[i + 5][maxallpos[3]] == 0 && disk[i + 6][maxallpos[3]] == 0 && disk[i + 7][maxallpos[3]] == 0 && disk[i + 8][maxallpos[3]] == 0 && disk[i + 9][maxallpos[3]] == 0){
                maxallpos[3]--;
            }
            bool ok1 = true;
            bool ok2 = true;
            bool ok3 = true;
            if(disk_state[i].position > maxallpos[1] && disk_state[i].position < V / 3 + 1 - 90){
                ok1 = true;
                ok2 = false;
                ok3 = false;
            }
            else if(disk_state[i].position > maxallpos[2] && disk_state[i].position < 2 * V / 3 + 1 - 190)
            {
                ok1 = false;
                ok2 = true;
                ok3 = false;               
            }
            else if(disk_state[i].position > maxallpos[3])
            {
                ok1 = false;
                ok2 = false;
                ok3 = true;               
            }
            else{
                ok1 = false;
                ok2 = false;
                ok3 = false;
            }
            if(ok1)
            {
                for(int i = 1; i <= N; i++){
                    printf("j %d\n", V / 3 + 1 - 80);               
                    disk_state[i].position = V / 3 + 1 - 80;
                    disk_state[i].is_last_action_read = 0;
                    disk_state[i].remain_token = 0;
                    jumpCounter++;
                }
                break;
            }
            else if(ok2)
            {
                for(int i = 1; i <= N; i++){
                    printf("j %d\n", 2 * V / 3 + 1 - 160);               
                    disk_state[i].position = 2 * V / 3 + 1 - 160;
                    disk_state[i].is_last_action_read = 0;
                    disk_state[i].remain_token = 0;
                    jumpCounter++;
                }
                break;
            }
            else if(ok3)
            {
                for(int i = 1; i <= N; i++){
                    printf("j %d\n", 1);  
                    disk_state[i].position = 1;
                    disk_state[i].is_last_action_read = 0;
                    disk_state[i].remain_token = 0;         
                    jumpCounter++;
                }
                break;
            }
        }

        //int next_timestamp = get_next_timestamp_size(current_timestamp);
        int next_timestamp = 160;
        int requenst_count = 0;
        for (int j = 0; j < next_timestamp; j++) {
            int pos = ((disk_state[i].position + j - 1) % V) + 1;
            if(disk[i][pos] == 0){
                continue;
            }
            if(!request[object[disk[i][pos]].last_request_point].is_done){
                if(request[object[disk[i][pos]].last_request_point].timestamp > current_timestamp - 105){
                    requenst_count++;
                    if(requenst_count == 2){
                        break;
                    }
                }
            }
        }

        if (requenst_count  < 1 && disk_state[i].remain_token >= next_timestamp) {
            for (int j = 0; j < next_timestamp; j++) {
                printf("p");
                disk_state[i].remain_token -= 1;
                disk_state[i].position = (disk_state[i].position % V) + 1;
                disk_state[i].is_last_action_read = 0;
            }
        }

        while(disk_state[i].remain_token > 0) {
            int cost = 64;
            if(disk_state[i].is_last_action_read){
                cost = max(16, (int)ceil(disk_state[i].is_last_action_read * 0.8));
            }
            if(disk_state[i].remain_token >= cost){
                printf("r");
                if(disk[i][disk_state[i].position]){
                    int objectid = disk[i][disk_state[i].position];
                    for(int j = 1; j <= REP_NUM; j++){
                        if(i == object[objectid].replica[j]){
                            for(int k = 1; k <= object[objectid].size; k++){
                                if(object[objectid].unit[j][k] == disk_state[i].position){
                                    object[objectid].read_flag[k] = current_timestamp;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    readed_object_id.insert(objectid);
                }
                disk_state[i].remain_token -= cost;
                disk_state[i].is_last_action_read = cost;
                disk_state[i].position = disk_state[i].position% V + 1;
                if(disk_state[i].remain_token == 0){
                    printf("#\n");
                }
            }
            else{
                printf("#\n");
                break;
            }
        }
    }
    for(auto it = readed_object_id.begin(); it != readed_object_id.end();){
        int objectid = *it;
        bool complete = true;
        for(int i = 1; i <= object[objectid].size; i++){
            int temp1 = object[objectid].read_flag[i];
            if(object[objectid].read_flag[i]){
                continue;
            }
            else{
                complete = false;
                break;
            }
        }
        if(complete){
            if(!object[objectid].is_delete){
                int mints = current_timestamp;
                for(int i = 1; i <= object[objectid].size; i++){
                    mints = min(mints, object[objectid].read_flag[i]);
                    object[objectid].read_flag[i] = 0;
                }

                if(object[objectid].last_request_point){
                    int request_id = object[objectid].last_request_point;
                    while(request[request_id].timestamp <= mints && request[request_id].timestamp > current_timestamp - 105){
                        if(request[request_id].is_done == false){
                            completed_requests.push_back(request_id);
                        }
                        if(request[request_id].prev_id == 0) {
                            break;
                        }
                        request_id = request[request_id].prev_id;
                    }
                }
            }
            it = readed_object_id.erase(it);
        }
        else{
            it++;
        }
    }
    if(!completed_requests.empty()){
        printf("%d\n", completed_requests.size());
        for(int i = 0; i < completed_requests.size(); i++){
            request[completed_requests[i]].is_done = true;
            printf("%d\n", completed_requests[i]);
        }
    }
    else{
        printf("0\n");
    }
    completed_requests.clear();
    fflush(stdout);
}

void clean()
{
    for (auto& obj : object) {
        for (int i = 1; i <= REP_NUM; i++) {
            if (obj.unit[i] == nullptr)
                continue;
            free(obj.unit[i]);
            obj.unit[i] = nullptr;
        }
    }
}

int main()
{
    scanf("%d%d%d%d%d", &T, &M, &N, &V, &G);

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }

    printf("OK\n");
    fflush(stdout);

    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
    }
    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        timestamp_action();
        delete_action();
        write_action();
        read_action();
    }
    clean();

    return 0;
}
