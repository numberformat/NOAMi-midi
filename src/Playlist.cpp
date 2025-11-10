#include "Playlist.h"

Playlist::Playlist() : entries_(), current_(entries_.end()) {}

bool Playlist::Empty() const {
    return entries_.empty();
}

std::size_t Playlist::Size() const {
    return entries_.size();
}

void Playlist::Clear() {
    entries_.clear();
    current_ = entries_.end();
}

void Playlist::Add(const std::string &filename) {
    entries_.emplace_back(filename);
    if (entries_.size() == 1) {
        current_ = entries_.begin();
    }
}

void Playlist::AddMany(const std::vector<std::string> &filenames) {
    for (const auto &name : filenames) {
        Add(name);
    }
}

void Playlist::ResetCurrentToHead() {
    current_ = entries_.begin();
}

bool Playlist::HasCurrent() const {
    return current_ != entries_.end();
}

const std::string *Playlist::CurrentFilename() const {
    if (!HasCurrent()) {
        return nullptr;
    }
    return &current_->filename;
}

bool Playlist::AdvanceCurrent() {
    if (!HasCurrent()) {
        return false;
    }
    ++current_;
    return HasCurrent();
}
