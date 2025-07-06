#pragma once
#include "MultiThreading/EventPolicy.h"

enum class IOEvents
{
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_REPEATED,
    CHAR_INPUT,
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_RELEASED,
    MOUSE_MOVED,
    MOUSE_SCROLLED,
    MOUSE_ENTERED,
    NUM
};

struct IOEventPolicy : MT::EventPolicy<IOEvents, static_cast<size_t>(IOEvents::NUM)> {};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::KEY_PRESSED> {
    using Signature = void(int key, int scancode, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::KEY_RELEASED> {
    using Signature = void(int key, int scancode, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::KEY_REPEATED> {
    using Signature = void(int key, int scancode, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::CHAR_INPUT> {
    using Signature = void(unsigned int codepoint);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MOUSE_BUTTON_PRESSED> {
    using Signature = void(int button, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MOUSE_BUTTON_RELEASED> {
    using Signature = void(int button, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MOUSE_MOVED> {
    using Signature = void(double x, double y);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MOUSE_SCROLLED> {
    using Signature = void(double xoffset, double yoffset);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MOUSE_ENTERED> {
    using Signature = void(bool entered);
};
