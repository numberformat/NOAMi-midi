#include "Mt32State.h"

#include <cstring>

namespace {
constexpr int kPatchCount = 128;
constexpr int kMemoryCount = 64;
constexpr int kTimbreNameLength = 10;
constexpr char kEmptyName[] = "";
}

Mt32State::Mt32State() {
    Reset();
}

void Mt32State::Reset() {
    for (int i = 0; i < kPatchCount; ++i) {
        patch_groups_[i] = (i < 64) ? 0 : 1;
        patch_programs_[i] = static_cast<unsigned char>(i);
    }
    for (int i = 0; i < kMemoryCount; ++i) {
        memory_names_[i][0] = '\0';
    }
}

void Mt32State::InterpretPatchMemory(const unsigned char *data, int length) {
    if (!data || length <= 9)
        return;

    int addr = (data[5] << 7) | data[6];
    const unsigned char *p = data + 7;
    const unsigned char *end = data + length - 9;

    while (p < end) {
        int patch = addr / 8;
        unsigned char group = *p++;
        unsigned char num = *p++;
        if (patch >= 0 && patch < kPatchCount) {
            patch_groups_[patch] = group;
            patch_programs_[patch] = (group == 1) ? static_cast<unsigned char>(num + 64) : num;
        }
        p += 6; // Skip remaining fields we do not currently surface
        addr += 8;
    }
}

void Mt32State::InterpretTimbreMemory(const unsigned char *data, int length) {
    if (!data || length <= 7)
        return;

    int timbre = data[5] / 2;
    if (timbre < 0 || timbre >= kMemoryCount)
        return;

    std::memset(memory_names_[timbre], 0, sizeof(memory_names_[timbre]));
    int copy_len = length - 7;
    if (copy_len > kTimbreNameLength)
        copy_len = kTimbreNameLength;
    if (copy_len > 0)
        std::memcpy(memory_names_[timbre], reinterpret_cast<const char *>(&data[7]), copy_len);
    memory_names_[timbre][copy_len] = '\0';
}

unsigned char Mt32State::PatchGroup(unsigned char program) const {
    return patch_groups_[program % kPatchCount];
}

unsigned char Mt32State::PatchProgram(unsigned char program) const {
    return patch_programs_[program % kPatchCount];
}

const char *Mt32State::MemoryName(unsigned char index) const {
    if (index < kMemoryCount)
        return memory_names_[index];
    return kEmptyName;
}
