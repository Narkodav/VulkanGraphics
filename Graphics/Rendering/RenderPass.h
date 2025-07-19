#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"
#include "SwapChainFormat.h"

namespace Graphics {

    class RenderPass
    {
    private:
        vk::RenderPass m_renderPass;

        bool m_initialized = false;
    public:

        RenderPass() : m_renderPass(nullptr) {};

        RenderPass(const Context& instance, const Device& device, const SwapChainFormat& format);

        RenderPass(RenderPass&& other) noexcept {
            m_renderPass = std::exchange(other.m_renderPass, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);
        };

        //moving to an initialized swap chain is undefined behavior, destroy before moving
        RenderPass& operator=(RenderPass&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "RenderPass::operator=() - RenderPass already initialized");

            m_renderPass = std::exchange(other.m_renderPass, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        RenderPass(const RenderPass& other) noexcept = delete;
        RenderPass& operator=(const RenderPass& other) noexcept = delete;

        ~RenderPass() { assert(!m_initialized && "RenderPass was not destroyed!"); };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;

            device.getDevice().destroyRenderPass(m_renderPass, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Destroyed RenderPass" << std::endl;
#endif
            m_initialized = false;
        };

        const vk::RenderPass& getRenderPass() const { return m_renderPass; };
    };

}