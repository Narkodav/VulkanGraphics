#pragma once
#include "MultiThreading/EventPolicy.h"

enum class IOEvents
{
    KeyPressed,
    KeyReleased,
    KeyRepeated,
    CharInput,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    MouseEntered,
    Num
};

struct IOEventPolicy : MT::EventPolicy<IOEvents, static_cast<size_t>(IOEvents::Num)> {};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::KeyPressed> {
    using Signature = void(int key, int scancode, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::KeyReleased> {
    using Signature = void(int key, int scancode, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::KeyRepeated> {
    using Signature = void(int key, int scancode, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::CharInput> {
    using Signature = void(unsigned int codepoint);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MouseButtonPressed> {
    using Signature = void(int button, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MouseButtonReleased> {
    using Signature = void(int button, int mods);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MouseMoved> {
    using Signature = void(double x, double y);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MouseScrolled> {
    using Signature = void(double xoffset, double yoffset);
};

template<>
template<>
struct IOEventPolicy::Traits<IOEvents::MouseEntered> {
    using Signature = void(bool entered);
};
