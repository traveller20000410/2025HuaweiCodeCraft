#include <cstdio>
#include <cmath>
#include <vector>
#include <unordered_set>
#include "globals.h"

void read_action() {
    static int jumpCounter = 0;
    static std::unordered_set<int> readed_object_id;
    static std::vector<int> completed_requests;

    int n_read;
    scanf("%d", &n_read);
    for (int i = 1; i <= n_read; i++) {
        int req_id, obj_id;
        scanf("%d%d", &req_id, &obj_id);
        request[req_id].object_id = obj_id;
        request[req_id].prev_id = object[obj_id].last_request_point;
        object[obj_id].last_request_point = req_id;
        request[req_id].is_done = false;
        request[req_id].timestamp = current_timestamp;
    }

    // … （这里继续粘贴原 read_action 内容，并去掉一切全局声明部分）
    // 注意：里面的 get_next_timestamp_size、disk_state、disk、object、request 等都在 globals.h 中声明

    // 最后输出 completed_requests 或 “0”
    // fflush(stdout)
}
