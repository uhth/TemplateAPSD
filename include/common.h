#ifndef COMMON_H
#define COMMON_H

#include <cstdint>

enum EVENT : uint_fast8_t {
    EXIT, PAUSE, NONE, RESET, NEXT, PREVIOUS
};

#endif