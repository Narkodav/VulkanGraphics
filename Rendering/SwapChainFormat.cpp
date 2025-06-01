#include "SwapChainFormat.h"

vk::SurfaceFormatKHR SwapChainFormat::chooseSurfaceFormat(const EngineDevice& device)
{
    auto supportDetails = device.getSwapChainSupportDetails();

    for (const auto& availableFormat : supportDetails.formats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return supportDetails.formats[0]; //might implement ranking for formats later
}

vk::PresentModeKHR SwapChainFormat::choosePresentMode(const EngineDevice& device)
{
    auto supportDetails = device.getSwapChainSupportDetails();

    for (const auto& availablePresentMode : supportDetails.presentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChainFormat::chooseExtent(const EngineDevice& device, const Window& window)
{
    auto supportDetails = device.getSwapChainSupportDetails();

    if (supportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return supportDetails.capabilities.currentExtent;
    }
    else {
        auto extent = window.getFrameBufferExtent();

        extent.width = std::clamp(extent.width, supportDetails.capabilities.minImageExtent.width,
            supportDetails.capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, supportDetails.capabilities.minImageExtent.height,
            supportDetails.capabilities.maxImageExtent.height);

        return extent;
    }
}
