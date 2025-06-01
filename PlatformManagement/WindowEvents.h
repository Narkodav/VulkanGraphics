#pragma once
#include "MultiThreading/EventPolicy.h"

enum class WindowEvents
{
    WINDOW_RESIZED,
    FRAME_BUFFER_RESIZED,
    WINDOW_MOVED,
    WINDOW_FOCUSED,
    WINDOW_MINIMIZED,
    WINDOW_MAXIMIZED,
    WINDOW_CLOSED,
    WINDOW_REFRESH,
    WINDOW_CONTENT_SCALE_CHANGED,
    NUM
};

struct WindowEventPolicy : MT::EventPolicy<WindowEvents, static_cast<size_t>(WindowEvents::NUM)> {};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_RESIZED> {
    using Signature = void(int width, int height);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::FRAME_BUFFER_RESIZED> {
    using Signature = void(int width, int height);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_MOVED> {
    using Signature = void(int x, int y);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_FOCUSED> {
    using Signature = void(bool focused);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_MINIMIZED> {
    using Signature = void(bool minimized);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_MAXIMIZED> {
    using Signature = void(bool maximized);
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_CLOSED> {
    using Signature = void();
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_REFRESH> {
    using Signature = void();
};

template<>
template<>
struct WindowEventPolicy::Traits<WindowEvents::WINDOW_CONTENT_SCALE_CHANGED> {
    using Signature = void(float xscale, float yscale);
};