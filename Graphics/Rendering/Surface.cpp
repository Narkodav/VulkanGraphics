#include "Surface.h"

namespace Graphics {

    Surface::Surface(const Context& instance, GLFWwindow* window)
    {
        VkSurfaceKHR surfaceTmp;
        if (glfwCreateWindowSurface(instance.getInstance(), window,
            nullptr, &surfaceTmp) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
        m_surface = surfaceTmp;
    }

}