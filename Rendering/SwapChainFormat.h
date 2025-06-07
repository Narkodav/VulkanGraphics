#pragma once
#include "Common.h"
#include "PlatformManagement/Window.h"
#include "Device.h"
#include "EngineContext.h"

class SwapChainFormat
{
private:
    vk::SurfaceFormatKHR m_surfaceFormat;
    vk::PresentModeKHR m_presentMode;
    vk::Extent2D m_swapChainExtent;

    //optional
    vk::Format m_depthFormat = vk::Format::eUndefined;

public:

    SwapChainFormat(
        vk::SurfaceFormatKHR format = {},
        vk::PresentModeKHR mode = vk::PresentModeKHR::eImmediate,
        vk::Extent2D extent = { 0, 0 }
    ) : m_surfaceFormat(format), m_presentMode(mode),
        m_swapChainExtent(extent) {};

    SwapChainFormat(
        const EngineContext& instance,
        const Device& device,
        vk::SurfaceFormatKHR format = {},
        vk::PresentModeKHR mode = vk::PresentModeKHR::eImmediate,
        vk::Extent2D extent = { 0, 0 }
    ) : m_surfaceFormat(format), m_presentMode(mode),
        m_swapChainExtent(extent) {
        m_depthFormat = device.findSupportedFormat(instance,
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);        
        };

    SwapChainFormat(const SwapChainFormat&) = default;
    SwapChainFormat& operator=(const SwapChainFormat&) = default;

    SwapChainFormat(SwapChainFormat&&) = default;
    SwapChainFormat& operator=(SwapChainFormat&&) = default;

    void setSurfaceFormat(vk::SurfaceFormatKHR surfaceFormat) { this->m_surfaceFormat = surfaceFormat; };
    void setPresentMode(vk::PresentModeKHR presentMode) { this->m_presentMode = presentMode; };
    void setSwapChainExtent(vk::Extent2D swapChainExtent) { this->m_swapChainExtent = swapChainExtent; };

    vk::SurfaceFormatKHR getSurfaceFormat() const { return m_surfaceFormat; };
    vk::PresentModeKHR getPresentMode() const { return m_presentMode; };
    vk::Extent2D getSwapChainExtent() const { return m_swapChainExtent; };
    vk::Format getDepthFormat() const { return m_depthFormat; };

    static vk::SurfaceFormatKHR chooseSurfaceFormat(const SwapChainSupportDetails& supportDetails);
    static vk::PresentModeKHR choosePresentMode(const SwapChainSupportDetails& supportDetails);
    static vk::Extent2D chooseExtent(const SwapChainSupportDetails& supportDetails, const Window& window);

    static SwapChainFormat create(
        const EngineContext& instance,
        const Device& device,
        const Window& window
    ) {
        SwapChainSupportDetails supports = device.getPhysicalDevice()
            .getSwapChainSupportDetails(instance, window);

        return SwapChainFormat(instance, device,
            chooseSurfaceFormat(supports),
            choosePresentMode(supports),
            chooseExtent(supports, window)
        );
    };

    bool isValid() const {
        return m_swapChainExtent.width > 0 &&
            m_swapChainExtent.height > 0;
    }
};

