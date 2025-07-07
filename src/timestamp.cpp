#include <cstdio>
#include "globals.h"

void timestamp_action() {
    int timestamp;
    scanf("%*s%d", &timestamp);
    printf("TIMESTAMP %d\n", timestamp);
    fflush(stdout);
    current_timestamp = timestamp;
}
