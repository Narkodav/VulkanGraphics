#pragma once
#include "Common.h"
#include "Rendering/EngineContext.h"
#include "MultiThreading/EventSuperSystem.h"
#include "WindowEvents.h"
#include "IOEvents.h"
#include "PlatformContext.h"

class Window
{
public:
    using EventManager = MT::EventSuperSystem<WindowEventPolicy, IOEventPolicy>;

    enum class CursorMode {
        NORMAL = GLFW_CURSOR_NORMAL,
        HIDDEN = GLFW_CURSOR_HIDDEN,
        DISABLED = GLFW_CURSOR_DISABLED  // For raw motion input
    };

    struct Attributes
    {
        enum class Type {
            // Window behavior
            RESIZABLE,                  // bool
            VISIBLE,                    // bool
            DECORATED,                  // bool
            FOCUSED,                    // bool
            AUTO_ICONIFY,               // bool
            FLOATING,                   // bool
            MAXIMIZED,                  // bool
            CENTER_CURSOR,              // bool
            TRANSPARENT_FRAMEBUFFER,    // bool
            FOCUS_ON_SHOW,              // bool

            // Framebuffer settings
            SRGB_CAPABLE,               // bool
            DOUBLE_BUFFER,              // bool
            SAMPLES,                    // int

            // Input mode
            CURSOR_MODE,                // CursorMode enum
        };

        // Window behavior
        bool resizable = true;                  // GLFW_RESIZABLE
        bool visible = true;                    // GLFW_VISIBLE
        bool decorated = true;                  // GLFW_DECORATED (title bar, borders)
        bool focused = true;                    // GLFW_FOCUSED
        bool autoIconify = true;                // GLFW_AUTO_ICONIFY
        bool floating = false;                  // GLFW_FLOATING (always on top)
        bool maximized = false;                 // GLFW_MAXIMIZED
        bool centerCursor = false;              // GLFW_CENTER_CURSOR
        bool transparentFramebuffer = false;    // GLFW_TRANSPARENT_FRAMEBUFFER
        bool focusOnShow = true;                // GLFW_FOCUS_ON_SHOW

        CursorMode cursorMode = CursorMode::NORMAL;

        // Framebuffer settings
        int samples = 0;                 // GLFW_SAMPLES (MSAA)
        bool srgbCapable = false;        // GLFW_SRGB_CAPABLE
        bool doubleBuffer = true;        // GLFW_DOUBLEBUFFER

        static Attributes defaultAtr() {
            return Attributes();
        }

        static Attributes firstPersonGameAtr() {
            Attributes atr;
            atr.centerCursor = true;
            atr.cursorMode = CursorMode::DISABLED;
            atr.maximized = true;
            return atr;
        }
    };

    using WindowEventSubscription = MT::EventSystem<WindowEventPolicy>::Subscription;
    using IOEventSubscription = MT::EventSystem<IOEventPolicy>::Subscription;

private:
    vk::SurfaceKHR m_surface;
    GLFWwindow* m_window = nullptr;
    Extent m_windowExtent; //extent in window coordinates
    Extent m_frameBufferExtent; //extent in pixels, represents actual physical window size, use this for rendering
    std::string m_windowText;

    Attributes m_attributes;
    EventManager m_platformEvents;

    bool m_initialized = false;

public:
    Window() : m_surface(nullptr), m_window(nullptr),
        m_windowExtent({ 0, 0 }), m_windowText(""),
        m_initialized(false) {
    };

    Window(const EngineContext& context, const Extent& windowExtent,
        const std::string& windowText, const Attributes& attr);

    void init(const EngineContext& context, const Extent& windowExtent,
        const std::string& windowText, const Attributes& attr);

    Window(Window&& other) noexcept {
        m_surface = std::exchange(other.m_surface, nullptr);
        m_window = std::exchange(other.m_window, nullptr);
        m_windowExtent = std::exchange(other.m_windowExtent, Extent{ 0, 0 });
        m_frameBufferExtent = std::exchange(other.m_frameBufferExtent, Extent{ 0, 0 });
        m_windowText = std::exchange(other.m_windowText, "");

        m_attributes = std::exchange(other.m_attributes, Attributes());
        m_platformEvents = std::exchange(other.m_platformEvents, EventManager());

        m_initialized = std::exchange(other.m_initialized, false);
        if (m_window) {
            glfwSetWindowUserPointer(m_window, this);
        }
    };

    Window& operator=(Window&& other) noexcept
    {
        if (this != &other) {
            assert(!m_initialized && "Cannot move to an initialised window");

            m_surface = std::exchange(other.m_surface, nullptr);
            m_window = std::exchange(other.m_window, nullptr);
            m_windowExtent = std::exchange(other.m_windowExtent, Extent{ 0, 0 });
            m_frameBufferExtent = std::exchange(other.m_frameBufferExtent, Extent{ 0, 0 });
            m_windowText = std::exchange(other.m_windowText, "");

            m_attributes = std::exchange(other.m_attributes, Attributes());
            m_platformEvents = std::exchange(other.m_platformEvents, EventManager());

            m_initialized = std::exchange(other.m_initialized, false);
            if (m_window) {
                glfwSetWindowUserPointer(m_window, this);
            }
        }
        return *this;
    };

    Window(const Window& other) noexcept = delete;
    Window& operator=(const Window& other) noexcept = delete;

    ~Window() { assert(!m_initialized && "Window was not destroyed!"); };

    const vk::SurfaceKHR& getSurface() const { return m_surface; };
    const Extent& getWindowExtent() const { return m_windowExtent; };
    const Extent& getFrameBufferExtent() const { return m_frameBufferExtent; };
    const std::string& getWindowText() const { return m_windowText; };
    GLFWwindow* getWindowHandle() const { return m_window; };
    void destroy(const EngineContext& context);

    bool shouldClose()
    {
        return glfwWindowShouldClose(m_window) == GLFW_TRUE;
    }

    static void pollEvents()
    {
        glfwPollEvents();
    }

    void swapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    void makeContextCurrent()
    {
        glfwMakeContextCurrent(m_window);
    }

    glm::ivec2 getCursorPos()
    {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        return glm::ivec2(x, y);
    }

    static void swapInterval(int interval)
    {
        glfwSwapInterval(interval);
    }

    bool isMinimised()
    {
        return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) == GLFW_TRUE;
    }

    template<auto E>
    auto registerCallback(auto callback)
    {
        return m_platformEvents.subscribe<E>(std::move(callback));
    }

private:
    template<Attributes::Type>
    static constexpr bool always_false = false;

public:
    template<Attributes::Type type, typename T>
    Window& setAttribute(T value) {
        static_assert(always_false<type>, "This attribute can only be set during window creation!");
        return *this;
    };

    template<>
    Window& setAttribute<Attributes::Type::RESIZABLE>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, value);
        m_attributes.resizable = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::VISIBLE>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_VISIBLE, value);
        m_attributes.visible = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::DECORATED>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_DECORATED, value);
        m_attributes.decorated = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::FOCUSED>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_FOCUSED, value);
        m_attributes.focused = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::AUTO_ICONIFY>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_AUTO_ICONIFY, value);
        m_attributes.autoIconify = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::FLOATING>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_FLOATING, value);
        m_attributes.floating = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::MAXIMIZED>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_MAXIMIZED, value);
        m_attributes.maximized = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::CENTER_CURSOR>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_CENTER_CURSOR, value);
        m_attributes.centerCursor = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::TRANSPARENT_FRAMEBUFFER>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_TRANSPARENT_FRAMEBUFFER, value);
        m_attributes.transparentFramebuffer = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::FOCUS_ON_SHOW>(bool value) {
        glfwSetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW, value);
        m_attributes.focusOnShow = value;
        return *this;
    }

    template<>
    Window& setAttribute<Attributes::Type::CURSOR_MODE>(CursorMode value) {
        glfwSetInputMode(m_window, GLFW_CURSOR, static_cast<int>(value));
        m_attributes.cursorMode = value;
        return *this;
    }

private:
    // Static window callbacks
    static void static_windowResizeCallback(GLFWwindow* window, int width, int height);
    static void static_windowMoveCallback(GLFWwindow* window, int x, int y);
    static void static_windowFocusCallback(GLFWwindow* window, int focused);
    static void static_windowMinimizeCallback(GLFWwindow* window, int minimized);
    static void static_windowMaximizeCallback(GLFWwindow* window, int maximized);
    static void static_windowCloseCallback(GLFWwindow* window);
    static void static_windowRefreshCallback(GLFWwindow* window);
    static void static_windowScaleCallback(GLFWwindow* window, float xscale, float yscale);
    static void static_framebufferResizeCallback(GLFWwindow* window, int width, int height);

    // Static IO callbacks
    static void static_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void static_charCallback(GLFWwindow* window, unsigned int codepoint);
    static void static_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void static_cursorPosCallback(GLFWwindow* window, double x, double y);
    static void static_scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void static_cursorEnterCallback(GLFWwindow* window, int entered);

};