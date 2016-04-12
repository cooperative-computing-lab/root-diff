#include "timer.h"

Timer::Timer() {
    clock_gettime(CLOCK_REALTIME, &begin);
}

double Timer::elapsed() {
    clock_gettime(CLOCK_REALTIME, &end);
    return end.tv_sec - begin.tv_sec +
    (end.tv_nsec - begin.tv_nsec) / 1000000000.;
}

void Timer::reset() {
    clock_gettime(CLOCK_REALTIME, &begin);
}
