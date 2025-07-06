#include "Window.h"

Window::Window(const GraphicsContext& context, const Extent& windowExtent,
    const std::string& windowText, const Attributes& attr)
{
    init(context, windowExtent, windowText, attr);
}

void Window::init(const GraphicsContext& context, const Extent& windowExtent,
    const std::string& windowText, const Attributes& attr)
{
    PlatformContext::instance();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //window attributes
    glfwWindowHint(GLFW_RESIZABLE, attr.resizable);
    glfwWindowHint(GLFW_VISIBLE, attr.visible);
    glfwWindowHint(GLFW_DECORATED, attr.decorated);
    glfwWindowHint(GLFW_FOCUSED, attr.focused);
    glfwWindowHint(GLFW_AUTO_ICONIFY, attr.autoIconify);
    glfwWindowHint(GLFW_FLOATING, attr.floating);
    glfwWindowHint(GLFW_MAXIMIZED, attr.maximized);
    glfwWindowHint(GLFW_CENTER_CURSOR, attr.centerCursor);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, attr.transparentFramebuffer);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, attr.focusOnShow);
    glfwWindowHint(GLFW_SAMPLES, attr.samples);
    glfwWindowHint(GLFW_SRGB_CAPABLE, attr.srgbCapable);
    glfwWindowHint(GLFW_DOUBLEBUFFER, attr.doubleBuffer);

    m_window = glfwCreateWindow(m_windowExtent.width, m_windowExtent.height,
    m_windowText.c_str(), nullptr, nullptr);

    if (!m_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Window events
    glfwSetWindowSizeCallback(m_window, Window::static_windowResizeCallback);
    glfwSetWindowPosCallback(m_window, Window::static_windowMoveCallback);
    glfwSetWindowFocusCallback(m_window, Window::static_windowFocusCallback);
    glfwSetWindowIconifyCallback(m_window, Window::static_windowMinimizeCallback);
    glfwSetWindowMaximizeCallback(m_window, Window::static_windowMaximizeCallback);
    glfwSetWindowCloseCallback(m_window, Window::static_windowCloseCallback);
    glfwSetWindowRefreshCallback(m_window, Window::static_windowRefreshCallback);
    glfwSetWindowContentScaleCallback(m_window, Window::static_windowScaleCallback);
    glfwSetFramebufferSizeCallback(m_window, Window::static_framebufferResizeCallback);

    // IO events
    glfwSetKeyCallback(m_window, Window::static_keyCallback);
    glfwSetCharCallback(m_window, Window::static_charCallback);
    glfwSetMouseButtonCallback(m_window, Window::static_mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, Window::static_cursorPosCallback);
    glfwSetScrollCallback(m_window, Window::static_scrollCallback);
    glfwSetCursorEnterCallback(m_window, Window::static_cursorEnterCallback);

    glfwSetInputMode(m_window, GLFW_CURSOR, static_cast<int>(attr.cursorMode));     

    VkSurfaceKHR surfaceTmp;
    if (glfwCreateWindowSurface(context.getInstance(), m_window,
        nullptr, &surfaceTmp) != VK_SUCCESS) {
        glfwDestroyWindow(m_window);
        throw std::runtime_error("Failed to create window surface");
    }
    m_surface = vk::SurfaceKHR(surfaceTmp);

    m_frameBufferExtent = Extent::getFrameBufferExtent(m_window);
    m_initialized = true;
}

void Window::destroy(const GraphicsContext& context)
{
    if (!m_initialized)
        return;

    m_platformEvents.clear();

    context.getInstance().destroySurfaceKHR(m_surface, nullptr, context.getDispatchLoader());
    glfwDestroyWindow(m_window);
#ifdef _DEBUG
    std::cout << "Destroyed Window" << std::endl;
#endif
    m_initialized = false;
}

void Window::static_framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_frameBufferExtent.width = width;
    self->m_frameBufferExtent.height = height;
    self->m_platformEvents.emit<WindowEvents::FRAME_BUFFER_RESIZED>(width, height);
}

// Static window callbacks
void Window::static_windowResizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_windowExtent.width = width;
    self->m_windowExtent.height = height;
    self->m_platformEvents.emit<WindowEvents::WINDOW_RESIZED>(width, height);
}

void Window::static_windowMoveCallback(GLFWwindow* window, int x, int y) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_MOVED>(x, y);
}

void Window::static_windowFocusCallback(GLFWwindow* window, int focused) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_FOCUSED>(focused == GLFW_TRUE);
}

void Window::static_windowMinimizeCallback(GLFWwindow* window, int minimized) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_MINIMIZED>(minimized == GLFW_TRUE);
}

void Window::static_windowMaximizeCallback(GLFWwindow* window, int maximized) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_MAXIMIZED>(maximized == GLFW_TRUE);
}

void Window::static_windowCloseCallback(GLFWwindow* window) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_CLOSED>();
}

void Window::static_windowRefreshCallback(GLFWwindow* window) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_REFRESH>();
}

void Window::static_windowScaleCallback(GLFWwindow* window, float xscale, float yscale) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<WindowEvents::WINDOW_CONTENT_SCALE_CHANGED>(xscale, yscale);
}

// Static IO callbacks
void Window::static_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    switch (action) {
    case GLFW_PRESS:
        self->m_platformEvents.emit<IOEvents::KEY_PRESSED>(key, scancode, mods);
        break;
    case GLFW_RELEASE:
        self->m_platformEvents.emit<IOEvents::KEY_RELEASED>(key, scancode, mods);
        break;
    case GLFW_REPEAT:
        self->m_platformEvents.emit<IOEvents::KEY_REPEATED>(key, scancode, mods);
        break;
    }
}

void Window::static_charCallback(GLFWwindow* window, unsigned int codepoint) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<IOEvents::CHAR_INPUT>(codepoint);
}

void Window::static_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
        self->m_platformEvents.emit<IOEvents::MOUSE_BUTTON_PRESSED>(button, mods);
    }
    else {
        self->m_platformEvents.emit<IOEvents::MOUSE_BUTTON_RELEASED>(button, mods);
    }
}

void Window::static_cursorPosCallback(GLFWwindow* window, double x, double y) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<IOEvents::MOUSE_MOVED>(x, y);
}

void Window::static_scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<IOEvents::MOUSE_SCROLLED>(xoffset, yoffset);
}

void Window::static_cursorEnterCallback(GLFWwindow* window, int entered) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_platformEvents.emit<IOEvents::MOUSE_ENTERED>(entered == GLFW_TRUE);
}