#pragma once
#include "MultiThreading/EventPolicy.h"

enum class WindowEvents
{
    WindowResized,
    FrameBufferResized,
    WindowMoved,
    WindowFocused,
    WindowMinimized,
    WindowMaximized,
    WindowClosed,
    WindowRefresh,
    WindowContentScaleChanged,
    Num
};

struct WindowEventPolicy : MT::EventPolicy<WindowEvents, static_cast<size_t>(WindowEvents::Num)> {};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowResized> {
    using Signature = void(int width, int height);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::FrameBufferResized> {
    using Signature = void(int width, int height);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowMoved> {
    using Signature = void(int x, int y);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowFocused> {
    using Signature = void(bool focused);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowMinimized> {
    using Signature = void(bool minimized);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowMaximized> {
    using Signature = void(bool maximized);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowClosed> {
    using Signature = void();
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowRefresh> {
    using Signature = void();
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WindowContentScaleChanged> {
    using Signature = void(float xscale, float yscale);
};