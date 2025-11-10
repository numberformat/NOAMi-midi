#pragma once

#include <windows.h>

#include <string>

class SettingsStore {
public:
    static SettingsStore &Instance();

    void Load();
    void Save();

    int midi_in_cb = 0;
    int midi_out_cb = 0;

    bool appRectSaved = false;
    bool textRectSaved = false;
    bool tracksRectSaved = false;
    bool channelsRectSaved = false;
    bool sysexRectSaved = false;
    bool genericTextRectSaved = false;

    RECT appRect{};
    RECT textRect{};
    RECT tracksRect{};
    RECT channelsRect{};
    RECT sysexRect{};
    RECT genericTextRect{};

private:
    SettingsStore();

    std::string settings_path_;
    bool loaded_ = false;

    static int ReadInt(const std::string &content, const std::string &key, int defaultValue);
    static bool ReadBool(const std::string &content, const std::string &key, bool defaultValue);
    static RECT ReadRect(const std::string &content, const std::string &prefix, const RECT &fallback);

    static void WriteInt(std::ostream &out, const char *key, int value, bool &first);
    static void WriteBool(std::ostream &out, const char *key, bool value, bool &first);
    static void WriteRect(std::ostream &out, const char *prefix, const RECT &rect, bool &first);
};
