#include "SettingsStore.h"

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace {
int ClampRectCoord(long value) {
    return static_cast<int>(value);
}
}

SettingsStore &SettingsStore::Instance() {
    static SettingsStore instance;
    return instance;
}

SettingsStore::SettingsStore() {
#ifdef _WIN32
    const char *userProfile = std::getenv("USERPROFILE");
    fs::path base = userProfile ? fs::path(userProfile) / "AppData" / "Local" / "NOAMi-MIDI"
                                : fs::temp_directory_path() / "NOAMi-MIDI";
#else
    const char *home = std::getenv("HOME");
    fs::path base = home ? fs::path(home) / ".config" / "noami-midi"
                         : fs::temp_directory_path() / "noami-midi";
#endif
    settings_path_ = (base / "settings.json").string();
}

void SettingsStore::Load() {
    if (loaded_)
        return;

    std::ifstream in(settings_path_);
    if (!in.is_open()) {
        loaded_ = true;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    midi_in_cb = ReadInt(content, "midi_in_cb", midi_in_cb);
    midi_out_cb = ReadInt(content, "midi_out_cb", midi_out_cb);

    appRectSaved = ReadBool(content, "appRectSaved", appRectSaved);
    textRectSaved = ReadBool(content, "textRectSaved", textRectSaved);
    tracksRectSaved = ReadBool(content, "tracksRectSaved", tracksRectSaved);
    channelsRectSaved = ReadBool(content, "channelsRectSaved", channelsRectSaved);
    sysexRectSaved = ReadBool(content, "sysexRectSaved", sysexRectSaved);
    genericTextRectSaved = ReadBool(content, "genericTextRectSaved", genericTextRectSaved);

    appRect = ReadRect(content, "appRect", appRect);
    textRect = ReadRect(content, "textRect", textRect);
    tracksRect = ReadRect(content, "tracksRect", tracksRect);
    channelsRect = ReadRect(content, "channelsRect", channelsRect);
    sysexRect = ReadRect(content, "sysexRect", sysexRect);
    genericTextRect = ReadRect(content, "genericTextRect", genericTextRect);

    loaded_ = true;
}

void SettingsStore::Save() {
    fs::path path(settings_path_);
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);

    std::ofstream out(settings_path_, std::ios::trunc);
    if (!out.is_open())
        return;

    out << "{\n";
    bool first = true;

    WriteInt(out, "midi_in_cb", midi_in_cb, first);
    WriteInt(out, "midi_out_cb", midi_out_cb, first);

    WriteBool(out, "appRectSaved", appRectSaved, first);
    WriteBool(out, "textRectSaved", textRectSaved, first);
    WriteBool(out, "tracksRectSaved", tracksRectSaved, first);
    WriteBool(out, "channelsRectSaved", channelsRectSaved, first);
    WriteBool(out, "sysexRectSaved", sysexRectSaved, first);
    WriteBool(out, "genericTextRectSaved", genericTextRectSaved, first);

    WriteRect(out, "appRect", appRect, first);
    WriteRect(out, "textRect", textRect, first);
    WriteRect(out, "tracksRect", tracksRect, first);
    WriteRect(out, "channelsRect", channelsRect, first);
    WriteRect(out, "sysexRect", sysexRect, first);
    WriteRect(out, "genericTextRect", genericTextRect, first);

    out << "\n}\n";
}

int SettingsStore::ReadInt(const std::string &content, const std::string &key, int defaultValue) {
    const std::string needle = "\"" + key + "\"";
    const size_t pos = content.find(needle);
    if (pos == std::string::npos)
        return defaultValue;
    size_t colon = content.find(':', pos + needle.size());
    if (colon == std::string::npos)
        return defaultValue;
    colon++;
    while (colon < content.size() && std::isspace(static_cast<unsigned char>(content[colon])))
        ++colon;
    size_t end = colon;
    while (end < content.size() && (std::isdigit(static_cast<unsigned char>(content[end])) || content[end] == '-' ))
        ++end;
    try {
        return std::stoi(content.substr(colon, end - colon));
    } catch (...) {
        return defaultValue;
    }
}

bool SettingsStore::ReadBool(const std::string &content, const std::string &key, bool defaultValue) {
    return ReadInt(content, key, defaultValue ? 1 : 0) != 0;
}

RECT SettingsStore::ReadRect(const std::string &content, const std::string &prefix, const RECT &fallback) {
    RECT rect = fallback;
    rect.left = ReadInt(content, prefix + "_left", rect.left);
    rect.top = ReadInt(content, prefix + "_top", rect.top);
    rect.right = ReadInt(content, prefix + "_right", rect.right);
    rect.bottom = ReadInt(content, prefix + "_bottom", rect.bottom);
    return rect;
}

void SettingsStore::WriteInt(std::ostream &out, const char *key, int value, bool &first) {
    if (!first)
        out << ",\n";
    first = false;
    out << "  \"" << key << "\": " << value;
}

void SettingsStore::WriteBool(std::ostream &out, const char *key, bool value, bool &first) {
    WriteInt(out, key, value ? 1 : 0, first);
}

void SettingsStore::WriteRect(std::ostream &out, const char *prefix, const RECT &rect, bool &first) {
    WriteInt(out, (std::string(prefix) + "_left").c_str(), ClampRectCoord(rect.left), first);
    WriteInt(out, (std::string(prefix) + "_top").c_str(), ClampRectCoord(rect.top), first);
    WriteInt(out, (std::string(prefix) + "_right").c_str(), ClampRectCoord(rect.right), first);
    WriteInt(out, (std::string(prefix) + "_bottom").c_str(), ClampRectCoord(rect.bottom), first);
}
