
// timing
double frames[10000];
struct timespec start_game;
clock_gettime(CLOCK_MONOTONIC_RAW, &start_game);
int hour, minute, second;

struct timespec dt_start;
struct timespec dt_end;
uint64_t dt; // in microseconds
clock_gettime(CLOCK_MONOTONIC_RAW, &dt_start);
clock_gettime(CLOCK_MONOTONIC_RAW, &dt_end);
Uint64 ticks = 0;
