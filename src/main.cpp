#include <cstdio>
#include "globals.h"

extern void timestamp_action();
extern void delete_action();
extern void write_action();
extern void read_action();
extern void clean();

int main() {
    scanf("%d%d%d%d%d", &T, &M, &N, &V, &G);
    // 跳过前三块预读
    for (int i = 0; i < 3; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }
    printf("OK\n"); fflush(stdout);

    // 初始化
    for (int i = 1; i <= N; i++) disk_point[i] = 1;
    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        timestamp_action();
        delete_action();
        write_action();
        read_action();
    }
    clean();
    return 0;
}
