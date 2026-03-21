#pragma once

#include <emscripten/html5.h>

class Window 
{
public:
    Window();

    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;
    int getFramebufferWidth() const { return m_fbWidth; }
    int getFramebufferHeight() const { return m_fbHeight; }

    void poolEvents();
    double getDevicePixelRatio() const;

private:
    int m_width = 0;
    int m_height = 0;
    int m_fbWidth = 0; // Реальні пікселі
    int m_fbHeight = 0;
};