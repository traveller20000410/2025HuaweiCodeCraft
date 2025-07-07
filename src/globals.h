#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <unordered_set>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#define MAX_DISK_NUM (10 + 1)
#define MAX_DISK_SIZE (16384 + 1)
#define MAX_REQUEST_NUM (30000000 + 1)
#define MAX_OBJECT_NUM (100000 + 1)
#define REP_NUM (3)
#define FRE_PER_SLICING (1800)
#define EXTRA_TIME (105)
#define MAX_TIME_SLICES (50)

extern int next_timestamp[MAX_TIME_SLICES];
int get_next_timestamp_size(int timestamp);

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
    int position;
    int remain_token;
    int is_last_action_read;
} DiskState;

// 全局变量
extern DiskState disk_state[MAX_DISK_NUM];
extern Request request[MAX_REQUEST_NUM];
extern Object object[MAX_OBJECT_NUM];

extern int T, M, N, V, G;
extern int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
extern int disk_point[MAX_DISK_NUM];
extern int max_index_arr[MAX_DISK_NUM];
extern int maxallpos[4];
extern int current_timestamp;

#endif // GLOBALS_H
