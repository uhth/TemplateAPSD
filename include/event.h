#ifndef EVENT_H
#define EVENT_H

#include <cstdint>

enum EVENT : uint_fast8_t {
    EXIT, PAUSE, NONE, RESET, NEXT, PREVIOUS
};

#endif