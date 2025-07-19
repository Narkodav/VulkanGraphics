#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"

namespace Graphics {

    class Fence
    {
    private:
        vk::Fence m_fence;

        bool m_initialized = false;
    public:

        Fence() {};
        Fence(const Context& instance, const Device& device, bool createSignaled = false);

        Fence(Fence&& other) noexcept {

            m_fence = std::exchange(other.m_fence, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

        };

        //moving to an initialized device is undefined behavior, destroy before moving
        Fence& operator=(Fence&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "Fence::operator=() - Fence already initialized");

            m_fence = std::exchange(other.m_fence, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        Fence(const Fence&) noexcept = delete;
        Fence& operator=(const Fence&) noexcept = delete;

        ~Fence() { assert(!m_initialized && "Fence was not destroyed!"); };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;

            device.getDevice().destroyFence(m_fence, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Destroyed fence" << std::endl;
#endif
            m_initialized = false;
        }

        bool isInitialized() const { return m_initialized; };

        void wait(const Context& instance, const Device& device);
        void reset(const Context& instance, const Device& device);

        vk::Fence getFence() const { return m_fence; };

    };

}