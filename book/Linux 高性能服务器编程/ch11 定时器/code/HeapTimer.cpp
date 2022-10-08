#include "HeapTimer.h"

HeapTimer::HeapTimer(int delay) { expire = time(NULL) + delay; }
