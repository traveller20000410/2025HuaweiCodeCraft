#include "globals.h"

// 时间片长度表
int next_timestamp[MAX_TIME_SLICING] = {
    0, 35, 40, 40, 26, 28, 26, 23, 26, 30,
    29, 23, 15, 35, 29, 18, 15, 15, 30, 28,
    30, 20, 18, 18, 23, 40, 30, 32, 32, 31, 
    30, 23, 30, 30, 30, 30, 30, 30, 30, 30, 
    30, 31, 31, 20, 40, 28, 32, 20, 35, 31, 
};

int get_next_timestamp_size(int timestamp) {
    int index = (timestamp - 0.1) / FRE_PER_SLICING + 1;
    return next_timestamp[index];
}

// 全局变量定义
DiskState disk_state[MAX_DISK_NUM];
Request request[MAX_REQUEST_NUM];
Object object[MAX_OBJECT_NUM];

int T, M, N, V, G;
int disk[MAX_DISK_NUM][MAX_DISK_SIZE];
int disk_point[MAX_DISK_NUM];
int max_index_arr[MAX_DISK_NUM] = {0};
int maxallpos[4];
int current_timestamp;
