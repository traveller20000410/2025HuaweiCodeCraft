#include <cstdio>
#include <cassert>
#include "globals.h"

static void do_object_delete(const int* object_unit, int* disk_unit, int size) {
    for (int i = 1; i <= size; i++) {
        disk_unit[object_unit[i]] = 0;
    }
}

void delete_action() {
    int n_delete, abort_num = 0;
    static int _id[MAX_OBJECT_NUM];

    scanf("%d", &n_delete);
    for (int i = 1; i <= n_delete; i++) {
        scanf("%d", &_id[i]);
    }
    // 统计未完成的请求
    for (int i = 1; i <= n_delete; i++) {
        int id = _id[i];
        int cur = object[id].last_request_point;
        while (cur) {
            if (!request[cur].is_done) abort_num++;
            cur = request[cur].prev_id;
        }
    }
    printf("%d\n", abort_num);

    // 真正删除
    for (int i = 1; i <= n_delete; i++) {
        int id = _id[i];
        int cur = object[id].last_request_point;
        while (cur) {
            if (!request[cur].is_done) {
                printf("%d\n", cur);
            }
            cur = request[cur].prev_id;
        }
        for (int j = 1; j <= REP_NUM; j++) {
            do_object_delete(object[id].unit[j],
                             disk[object[id].replica[j]],
                             object[id].size);
        }
        object[id].is_delete = true;
    }
    fflush(stdout);
}
