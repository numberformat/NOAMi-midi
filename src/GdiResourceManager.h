#pragma once

#include <windows.h>

class GdiResourceManager {
public:
    GdiResourceManager();
    ~GdiResourceManager();

    void Initialize();
    void Cleanup();

    HBRUSH NoteBackgroundBrush() const { return note_background_brush_; }
    HBRUSH ControllerBrush() const { return controller_brush_; }
    HPEN NoteBackgroundPen() const { return note_background_pen_; }
    HFONT ControllerFont() const { return controller_font_; }

private:
    void CreateResources();
    void ReleaseResources();

    bool initialized_;
    HBRUSH note_background_brush_;
    HBRUSH controller_brush_;
    HPEN note_background_pen_;
    HFONT controller_font_;
};
