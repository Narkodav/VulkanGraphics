#include "SwapChain.h"

namespace Graphics {

    SwapChain::SwapChain(const Context& instance, const Device& device, const Surface& surface,
        const RenderPass& renderPass, const SwapChainFormat& format,
        uint32_t presentQueueIndex, uint32_t workerQueueIndex) {
        init(instance, device, surface, renderPass, format, presentQueueIndex, workerQueueIndex);
    }

    void SwapChain::init(const Context& instance, const Device& device, const Surface& surface,
        const RenderPass& renderPass, const SwapChainFormat& format,
        uint32_t presentQueueIndex, uint32_t workerQueueIndex)
    {
        m_activeSwapChainFormat = format;
        auto supportDetails = device.getPhysicalDevice().getSwapChainSupportDetails(instance, surface);

        m_imageCount = supportDetails.capabilities.minImageCount + 1;
        auto d = supportDetails.capabilities.currentTransform;
        if (supportDetails.capabilities.maxImageCount > 0 && m_imageCount > supportDetails.capabilities.maxImageCount) {
            m_imageCount = supportDetails.capabilities.maxImageCount;
        }

        m_swapChain = createSwapChain(instance, device, supportDetails, surface,
            m_activeSwapChainFormat, m_imageCount, presentQueueIndex, workerQueueIndex);
        m_swapChainImages = getSwapChainImages(instance, device, m_swapChain, m_imageCount);
        m_swapChainImageViews = createImageViews(instance, device, m_activeSwapChainFormat, m_swapChainImages);

        if (m_activeSwapChainFormat.getDepthFormat() != vk::Format::eUndefined)
        {
            initDepthImage(instance, device);
            m_swapChainFrameBuffers = createFrameBuffers(instance, device, renderPass,
                m_swapChainImageViews, m_depthImageView, m_activeSwapChainFormat);
        }
        else m_swapChainFrameBuffers = createFrameBuffers(instance, device, renderPass,
            m_swapChainImageViews, m_activeSwapChainFormat);
        m_initialized = true;
    }

    void SwapChain::recreate(const Context& instance, const Device& device, const Surface& surface,
        const RenderPass& renderPass, const SwapChainFormat& format, uint32_t presentQueueIndex, uint32_t workerQueueIndex)
    {
        destroy(instance, device);
        init(instance, device, surface, renderPass, format, presentQueueIndex, workerQueueIndex);
    }

    vk::SwapchainKHR SwapChain::createSwapChain(const Context& instance,
        const Device& device, const SwapChainSupportDetails& swapChainSupportDetails,
        const Surface& surface, const SwapChainFormat& activeSwapChainFormat, uint32_t imageCount,
        uint32_t presentQueueIndex, uint32_t workerQueueIndex)
    {
        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        createInfo.surface = surface.getSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = activeSwapChainFormat.getSurfaceFormat().format;
        createInfo.imageColorSpace = activeSwapChainFormat.getSurfaceFormat().colorSpace;
        createInfo.imageExtent = activeSwapChainFormat.getSwapChainExtent();
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        std::array<uint32_t, 2> queueFamilyIndices = { presentQueueIndex, workerQueueIndex };
        if (presentQueueIndex != workerQueueIndex) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        }
        else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }
        createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = activeSwapChainFormat.getPresentMode();
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        vk::SwapchainKHR swapChain;
        try {
            swapChain = device.getDevice().createSwapchainKHR(createInfo, nullptr, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to create swap chain!");
        }
        return swapChain;
    }

    std::vector<vk::Image> SwapChain::getSwapChainImages(const Context& instance,
        const Device& device, const vk::SwapchainKHR& swapChain, uint32_t& imageCount)
    {
        std::vector<vk::Image> swapChainImages(imageCount);

        try {
            swapChainImages = device.getDevice().getSwapchainImagesKHR(swapChain, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("Failed to get swap chain images: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error getting swap chain images: " + std::string(e.what()));
        }

        return swapChainImages;
    }

    std::vector<vk::ImageView> SwapChain::createImageViews(const Context& instance,
        const Device& device, const SwapChainFormat& activeSwapChainFormat,
        const std::vector<vk::Image>& swapChainImages)
    {
        std::vector<vk::ImageView> imageViews(swapChainImages.size());

        for (int i = 0; i < swapChainImages.size(); i++)
        {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.sType = vk::StructureType::eImageViewCreateInfo;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format = activeSwapChainFormat.getSurfaceFormat().format;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            try {
                imageViews[i] = device.getDevice().createImageView(createInfo, nullptr, instance.getDispatchLoader());
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error("Failed to create imageView: " + std::string(e.what()));
            }
            catch (const std::exception& e) {
                throw std::runtime_error("Unexpected error creating imageView: " + std::string(e.what()));
            }
        }

        return imageViews;
    }

    std::vector<vk::Framebuffer> SwapChain::createFrameBuffers(const Context& instance,
        const Device& device, const RenderPass& renderPass,
        const std::vector<vk::ImageView>& imageViews, const SwapChainFormat& format)
    {
        std::vector<vk::Framebuffer> framebuffers(imageViews.size());

        for (size_t i = 0; i < framebuffers.size(); i++) {
            vk::ImageView attachments[] = {
                imageViews[i]
            };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
            framebufferInfo.renderPass = renderPass.getRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = format.getSwapChainExtent().width;
            framebufferInfo.height = format.getSwapChainExtent().height;
            framebufferInfo.layers = 1;

            try {
                framebuffers[i] = device.getDevice().createFramebuffer(framebufferInfo, nullptr, instance.getDispatchLoader());
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error("Failed to create frameBuffer: " + std::string(e.what()));
            }
            catch (const std::exception& e) {
                throw std::runtime_error("Unexpected error creating frameBuffer: " + std::string(e.what()));
            }
        }
        return framebuffers;
    }

    std::vector<vk::Framebuffer> SwapChain::createFrameBuffers(const Context& instance,
        const Device& device, const RenderPass& renderPass,
        const std::vector<vk::ImageView>& imageViews, vk::ImageView depthImageView,
        const SwapChainFormat& format)
    {
        std::vector<vk::Framebuffer> framebuffers(imageViews.size());

        for (size_t i = 0; i < framebuffers.size(); i++) {
            vk::ImageView attachments[] = {
                imageViews[i], depthImageView
            };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
            framebufferInfo.renderPass = renderPass.getRenderPass();
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = format.getSwapChainExtent().width;
            framebufferInfo.height = format.getSwapChainExtent().height;
            framebufferInfo.layers = 1;

            try {
                framebuffers[i] = device.getDevice().createFramebuffer(framebufferInfo, nullptr, instance.getDispatchLoader());
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error("Failed to create frameBuffer: " + std::string(e.what()));
            }
            catch (const std::exception& e) {
                throw std::runtime_error("Unexpected error creating frameBuffer: " + std::string(e.what()));
            }
        }
        return framebuffers;
    }

    void SwapChain::initDepthImage(const Context& instance,
        const Device& device)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.sType = vk::StructureType::eImageCreateInfo;
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = m_activeSwapChainFormat.getSwapChainExtent().width;
        imageInfo.extent.height = m_activeSwapChainFormat.getSwapChainExtent().height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_activeSwapChainFormat.getDepthFormat();
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.samples = vk::SampleCountFlagBits::e1;
        imageInfo.flags = vk::ImageCreateFlags(); // Optional

        try
        {
            device.getDevice().createImage(&imageInfo, nullptr, &m_depthImage, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& err)
        {
            throw std::runtime_error("failed to create image!");
        }

        auto imageMemoryRequirements = device.getDevice()
            .getImageMemoryRequirements(m_depthImage, instance.getDispatchLoader());
        m_depthImageMemory = Memory(instance, device, imageMemoryRequirements,
            MemoryProperty::Bits::DeviceLocal, imageMemoryRequirements.size);
        device.getDevice().bindImageMemory(m_depthImage, m_depthImageMemory.getMemory(), 0, instance.getDispatchLoader());

        vk::ImageViewCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eImageViewCreateInfo;
        createInfo.image = m_depthImage;
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = m_activeSwapChainFormat.getDepthFormat();
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        if (m_activeSwapChainFormat.getDepthFormat() == vk::Format::eD32SfloatS8Uint
            || m_activeSwapChainFormat.getDepthFormat() == vk::Format::eD24UnormS8Uint)
            createInfo.subresourceRange.aspectMask =
            vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
        else createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        try {
            m_depthImageView = device.getDevice().createImageView(createInfo, nullptr, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("Failed to create imageView: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error creating imageView: " + std::string(e.what()));
        }

    }

}