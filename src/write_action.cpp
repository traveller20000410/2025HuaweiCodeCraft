#include <cstdio>
#include <cassert>
#include <cstdlib>
#include "globals.h"

static void do_object_write(int* object_unit, int* disk_unit,
                            int size, int object_id, int j) {
    int start_pos = -1, consecutive_free = 0;
    // 先尝试连续空闲块
    for (int i = (j - 1) * V / 3 + 1 - 80 * (j - 1); i <= V; i++) {
        if (!disk_unit[i]) {
            if (++consecutive_free == 1) start_pos = i;
            if (consecutive_free == size) break;
        } else {
            consecutive_free = 0;
            start_pos = -1;
        }
    }
    // 如果没找到再零散写入
    if (consecutive_free < size) {
        int cur = 0;
        for (int i = (j - 1) * V / 3 + 1 - 80 * (j - 1); i <= V; i++) {
            if (!disk_unit[i]) {
                disk_unit[i] = object_id;
                object_unit[++cur] = i;
                maxallpos[j] = max(maxallpos[j], i);
                if (cur == size) break;
            }
        }
        assert(cur == size);
    } else {
        for (int k = 0; k < size; k++) {
            disk_unit[start_pos + k] = object_id;
            object_unit[k + 1] = start_pos + k;
            maxallpos[j] = max(maxallpos[j], start_pos + k);
        }
    }
}

void write_action() {
    int n_write; 
    scanf("%d", &n_write);
    for (int i = 1; i <= n_write; i++) {
        int id, size, tag;
        scanf("%d%d%d", &id, &size, &tag);
        object[id].last_request_point = 0;
        for (int j = 1; j <= REP_NUM; j++) {
            object[id].replica[j] = (id + j) % N + 1;
            object[id].unit[j] = (int*)malloc(sizeof(int)*(size+1));
            object[id].size = size;
            object[id].is_delete = false;
            object[id].read_flag = (int*)malloc(sizeof(int)*(size+1));
            for (int k = 0; k <= size; k++) object[id].read_flag[k]=0;
            do_object_write(object[id].unit[j],
                            disk[object[id].replica[j]],
                            size, id, j);
        }
        // 输出结果
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
