#include "GdiResourceManager.h"

GdiResourceManager::GdiResourceManager()
    : initialized_(false),
      note_background_brush_(NULL),
      controller_brush_(NULL),
      note_background_pen_(NULL),
      controller_font_(NULL) {}

GdiResourceManager::~GdiResourceManager() {
    Cleanup();
}

void GdiResourceManager::Initialize() {
    if (initialized_)
        return;

    CreateResources();
    initialized_ = true;
}

void GdiResourceManager::Cleanup() {
    if (!initialized_)
        return;

    ReleaseResources();
    initialized_ = false;
}

void GdiResourceManager::CreateResources() {
    const unsigned char fg = 208;
    const unsigned char bg = 224;

    note_background_pen_ = CreatePen(PS_SOLID, 1, RGB(fg, fg, fg));
    note_background_brush_ = CreateSolidBrush(RGB(fg, fg, fg));
    controller_brush_ = CreateSolidBrush(RGB(bg, bg, bg));

    controller_font_ = CreateFont(
        12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, "Arial");
}

void GdiResourceManager::ReleaseResources() {
    if (note_background_pen_) {
        DeleteObject(note_background_pen_);
        note_background_pen_ = NULL;
    }
    if (note_background_brush_) {
        DeleteObject(note_background_brush_);
        note_background_brush_ = NULL;
    }
    if (controller_brush_) {
        DeleteObject(controller_brush_);
        controller_brush_ = NULL;
    }
    if (controller_font_) {
        DeleteObject(controller_font_);
        controller_font_ = NULL;
    }
}
