#include "Threading.h"

#include <process.h>
#include <windows.h>

uintptr_t StartThread(ThreadStartRoutine routine, void *param) {
    return _beginthread(routine, 0, param);
}

void SleepMilliseconds(unsigned long milliseconds) {
    Sleep(milliseconds);
}

uint64_t HighResFrequency() {
    static uint64_t frequency = 0;
    if (!frequency) {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        frequency = static_cast<uint64_t>(li.QuadPart);
    }
    return frequency;
}

uint64_t HighResCounter() {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return static_cast<uint64_t>(li.QuadPart);
}
