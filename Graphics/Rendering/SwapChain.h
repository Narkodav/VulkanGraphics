#pragma once
#include "../Common.h"
#include "Device.h"
#include "SwapChainFormat.h"
#include "RenderPass.h"
#include "Semaphore.h"
#include "../MemoryManagement/Memory.h"

class SwapChain {
private:
    vk::SwapchainKHR m_swapChain;
    SwapChainFormat m_activeSwapChainFormat;

    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::ImageView> m_swapChainImageViews;
    std::vector<vk::Framebuffer> m_swapChainFrameBuffers;

    vk::Image m_depthImage;
    vk::ImageView m_depthImageView;
    Memory m_depthImageMemory;
    bool m_depthImageInitialized = false;

    uint32_t m_imageCount;

    bool m_initialized = false;
public:
    SwapChain() : m_swapChain(nullptr), m_activeSwapChainFormat(), m_imageCount(0), m_initialized(false) {};

    SwapChain(const GraphicsContext& instance, const Device& device, const Window& window,
        const RenderPass& renderPass, const SwapChainFormat& format, 
        uint32_t presentQueueIndex, uint32_t workerQueueIndex);

    SwapChain(SwapChain&& other) noexcept {
        m_swapChain = std::exchange(other.m_swapChain, nullptr);
        m_activeSwapChainFormat = std::exchange(other.m_activeSwapChainFormat, {});
        m_imageCount = std::exchange(other.m_imageCount, 0);
        m_swapChainImages = std::exchange(other.m_swapChainImages, std::vector<vk::Image>());
        m_swapChainImageViews = std::exchange(other.m_swapChainImageViews, std::vector<vk::ImageView>());
        m_swapChainFrameBuffers = std::exchange(other.m_swapChainFrameBuffers, std::vector<vk::Framebuffer>());

        m_depthImage = std::exchange(other.m_depthImage, vk::Image());
        m_depthImageView = std::exchange(other.m_depthImageView, vk::ImageView());
        m_depthImageMemory = std::exchange(other.m_depthImageMemory, Memory());

        m_initialized = std::exchange(other.m_initialized, false);
    };

    //moving to an initialized swap chain is undefined behavior, destroy before moving
    SwapChain& operator=(SwapChain&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "SwapChain::operator=() - SwapChain already initialized");

        m_swapChain = std::exchange(other.m_swapChain, nullptr);
        m_activeSwapChainFormat = std::exchange(other.m_activeSwapChainFormat, {});
        m_imageCount = std::exchange(other.m_imageCount, 0);
        m_swapChainImages = std::exchange(other.m_swapChainImages, std::vector<vk::Image>());
        m_swapChainImageViews = std::exchange(other.m_swapChainImageViews, std::vector<vk::ImageView>());
        m_swapChainFrameBuffers = std::exchange(other.m_swapChainFrameBuffers, std::vector<vk::Framebuffer>());

        m_depthImage = std::exchange(other.m_depthImage, vk::Image());
        m_depthImageView = std::exchange(other.m_depthImageView, vk::ImageView());
        m_depthImageMemory = std::exchange(other.m_depthImageMemory, Memory());

        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    SwapChain(const SwapChain& other) noexcept = delete;
    SwapChain& operator=(const SwapChain& other) noexcept = delete;

    ~SwapChain() { assert(!m_initialized && "SwapChain was not destroyed!"); };

    void init(const GraphicsContext& instance, const Device& device, const Window& window,
        const RenderPass& renderPass, const SwapChainFormat& format, uint32_t presentQueueIndex, uint32_t workerQueueIndex);

    void initDepthImage(const GraphicsContext& instance, const Device& device);

    void recreate(const GraphicsContext& instance, const Device& device, const Window& window,
        const RenderPass& renderPass, const SwapChainFormat& format, uint32_t presentQueueIndex, uint32_t workerQueueIndex);
     
    void destroy(const GraphicsContext& instance, const Device& device) {
        if (!m_initialized)
            return;

        for (auto framebuffer : m_swapChainFrameBuffers) {
            device.getDevice().destroyFramebuffer(framebuffer, nullptr, instance.getDispatchLoader());
        }

        for (auto& imageView : m_swapChainImageViews)
            device.getDevice().destroyImageView(imageView, nullptr, instance.getDispatchLoader());

        m_swapChainFrameBuffers.clear();
        m_swapChainImageViews.clear();
        m_swapChainImages.clear();

        if (m_activeSwapChainFormat.getDepthFormat() != vk::Format::eUndefined)
        {
            device.getDevice().destroyImageView(m_depthImageView, nullptr, instance.getDispatchLoader());
            m_depthImageMemory.destroy(instance, device);
            device.getDevice().destroyImage(m_depthImage);
        }

        if (m_swapChain) {
            device.getDevice().destroySwapchainKHR(m_swapChain, nullptr, instance.getDispatchLoader());
            m_swapChain = nullptr;
        }
#ifdef _DEBUG
        std::cout << "Destroyed SwapChain" << std::endl;
#endif
        m_initialized = false;
    };

    static vk::SwapchainKHR createSwapChain(const GraphicsContext& instance,
        const Device& device, const SwapChainSupportDetails& swapChainSupportDetails,
        const Window& window, const SwapChainFormat& activeSwapChainFormat, uint32_t imageCount,
        uint32_t presentQueueIndex, uint32_t workerQueueIndex);

    static std::vector<vk::Image> getSwapChainImages(const GraphicsContext& instance,
        const Device& device, const vk::SwapchainKHR& swapChain, uint32_t& imageCount);

    static std::vector<vk::ImageView> createImageViews(const GraphicsContext& instance,
        const Device& device, const SwapChainFormat& activeSwapChainFormat,
        const std::vector<vk::Image>& swapChainImages);

    static std::vector<vk::Framebuffer> createFrameBuffers(const GraphicsContext& instance,
        const Device& device, const RenderPass& renderPass,
        const std::vector<vk::ImageView>& imageViews, const SwapChainFormat& format);

    static std::vector<vk::Framebuffer> createFrameBuffers(const GraphicsContext& instance,
        const Device& device, const RenderPass& renderPass,
        const std::vector<vk::ImageView>& imageViews, vk::ImageView depthImageView, 
        const SwapChainFormat& format);

    const SwapChainFormat& getActiveSwapChainFormat() const { return m_activeSwapChainFormat; };

    const std::vector<vk::Framebuffer>& getFrameBuffers() const { return m_swapChainFrameBuffers; };

    bool acquireNextImage(const GraphicsContext& instance, const Device& device, 
    const Semaphore& semaphore, uint32_t& imageIndex) const {
        vk::Result result = device.getDevice().acquireNextImageKHR(
            m_swapChain, UINT64_MAX, semaphore.getSemaphore(), nullptr, &imageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
            return false;
        else if (result != vk::Result::eSuccess)
            throw std::runtime_error("Error acquiring next image");
        return true;
    };

    const vk::SwapchainKHR& getSwapChain() const { return m_swapChain; };
    const size_t& getImageCount() const { return m_imageCount; };
};

