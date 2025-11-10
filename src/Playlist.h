#pragma once

#include <cstddef>
#include <list>
#include <string>
#include <utility>
#include <vector>

class Playlist {
public:
    Playlist();

    bool Empty() const;
    std::size_t Size() const;

    void Clear();
    void Add(const std::string &filename);
    void AddMany(const std::vector<std::string> &filenames);

    void ResetCurrentToHead();
    bool HasCurrent() const;
    const std::string *CurrentFilename() const;
    bool AdvanceCurrent();

private:
    struct Entry {
        explicit Entry(std::string f) : filename(std::move(f)) {}
        std::string filename;
    };

    using EntryList = std::list<Entry>;

    EntryList entries_;
    EntryList::iterator current_;
};
