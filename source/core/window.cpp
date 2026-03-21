#include "source/core/windows.h"

Window::Window()
{
    poolEvents();
}

void Window::poolEvents()
{
    double cssW, cssH;
    emscripten_get_element_css_size("#canvas", &cssW, &cssH);
    
    double pixelRatio = emscripten_get_device_pixel_ratio();
    
    m_width = (int)cssW;
    m_height = (int)cssH;
    
    m_fbWidth = (int)(cssW * pixelRatio);
    m_fbHeight = (int)(cssH * pixelRatio);

    emscripten_set_canvas_element_size("#canvas", m_fbWidth, m_fbHeight);
}

int Window::getWidth() const 
{ 
    return m_width; 
}

int Window::getHeight() const 
{ 
    return m_height; 
}

float Window::getAspectRatio() const 
{ 
    return (float)m_fbWidth / (float)m_fbHeight;
}

double Window::getDevicePixelRatio() const { 
    return emscripten_get_device_pixel_ratio(); 
}

