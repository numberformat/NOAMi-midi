#pragma once

#include <cstddef>

class Mt32State {
public:
    Mt32State();

    void Reset();
    void InterpretPatchMemory(const unsigned char *data, int length);
    void InterpretTimbreMemory(const unsigned char *data, int length);

    unsigned char PatchGroup(unsigned char program) const;
    unsigned char PatchProgram(unsigned char program) const;
    const char *MemoryName(unsigned char index) const;

private:
    unsigned char patch_groups_[128];
    unsigned char patch_programs_[128];
    char memory_names_[64][11];
};
