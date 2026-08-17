#include "edgeinfer/runtime.hpp"
#include <sys/resource.h>
#include <time.h>

namespace edgeinfer {
double monotonic_time_ms() {
    struct timespec ts {};
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec) * 1000.0 +
           static_cast<double>(ts.tv_nsec) / 1000000.0;
}

long max_rss_kb() {
    struct rusage usage {};
    if (::getrusage(RUSAGE_SELF, &usage) != 0) return -1;
    return usage.ru_maxrss;
}
}
