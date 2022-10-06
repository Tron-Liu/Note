#include "time_wheel_timer.h"

TimeWheelTimer::TimeWheelTimer(int rot, int ts)
    : next(NULL), prev(NULL), rotation(rot), time_slot(ts) {}
