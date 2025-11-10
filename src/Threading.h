#pragma once

#include <cstdint>

using ThreadStartRoutine = void (__cdecl *)(void *);

uintptr_t StartThread(ThreadStartRoutine routine, void *param);
void SleepMilliseconds(unsigned long milliseconds);
uint64_t HighResFrequency();
uint64_t HighResCounter();
