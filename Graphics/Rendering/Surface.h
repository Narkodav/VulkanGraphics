#pragma once
#include "../Common.h"
#include "Context.h"

namespace Graphics {

    class Surface
    {
    private:
        vk::SurfaceKHR m_surface;

        bool m_initialized = false;
    public:

        Surface() {};
        Surface(const Context& instance, GLFWwindow* window);

        Surface(Surface&& other) noexcept {
            m_surface = std::exchange(other.m_surface, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);
        };

        //moving to an initialized device is undefined behavior, destroy before moving
        Surface& operator=(Surface&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "Surface::operator=() - Surface already initialized");

            m_surface = std::exchange(other.m_surface, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        Surface(const Surface&) noexcept = delete;
        Surface& operator=(const Surface&) noexcept = delete;

        ~Surface() { assert(!m_initialized && "Surface was not destroyed!"); };

        void destroy(const Context& instance) {
            if (!m_initialized)
                return;

            instance.getInstance().destroySurfaceKHR(m_surface, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Destroyed surface" << std::endl;
#endif
            m_initialized = false;
        }

        bool isInitialized() const { return m_initialized; };

        const vk::SurfaceKHR& getSurface() const { return m_surface; };

    };

}