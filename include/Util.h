#ifndef UTIL_H
#define UTIL_H

#include <cassert>
#include <chrono>
#include <iostream>

#define assertm(expr, msg) assert(((void)(msg), (expr)))
#define DEBUG(x) std::cout << x << '\n'
#define START_TIMER(name) auto timer_##name = std::chrono::steady_clock::now()
#define END_TIMER(name)                                                      \
    {                                                                        \
        std::cout << "Time elapsed: "                                        \
                  << (std::chrono::duration_cast<std::chrono::microseconds>( \
                          std::chrono::steady_clock::now() - timer_##name)   \
                          .count()) /                                        \
                         1000000.0                                           \
                  << "\n\n";                                                 \
    }

#endif  // UTIL_H