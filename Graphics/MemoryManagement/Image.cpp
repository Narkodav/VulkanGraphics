#include "Image.h"

Image::Image(const GraphicsContext& instance, const Device& device, const std::string& filepath)
{
    load(instance, device, filepath);
}

void Image::load(const GraphicsContext& instance, const Device& device, const std::string& filepath)
{
    int texWidth, texHeight, texChannels;
    m_pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    m_width = static_cast<size_t>(texWidth);
    m_height = static_cast<size_t>(texHeight);
    m_bpp = static_cast<size_t>(texChannels);
    m_capacity = m_width * m_height * STBI_rgb_alpha;

    if (!m_pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    vk::ImageCreateInfo imageInfo{};
    imageInfo.sType = vk::StructureType::eImageCreateInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.flags = vk::ImageCreateFlags(); // Optional

    try
    {
        device.getDevice().createImage(&imageInfo, nullptr, &m_image, instance.getDispatchLoader());
    }
    catch (const vk::SystemError& err)
    {
        throw std::runtime_error("failed to create image!");
    }

    m_imageMemoryRequirements = device.getDevice().getImageMemoryRequirements(m_image, instance.getDispatchLoader());

    m_initialized = true;
}

void Image::createView(const GraphicsContext& instance, const Device& device)
{
    vk::ImageViewCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eImageViewCreateInfo;
    createInfo.image = m_image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = vk::Format::eR8G8B8A8Srgb;
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
        m_view = device.getDevice().createImageView(createInfo, nullptr, instance.getDispatchLoader());
    }
    catch (const vk::SystemError& e) {
        throw std::runtime_error("Failed to create imageView: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unexpected error creating imageView: " + std::string(e.what()));
    }
}

//void Image::transfer(const GraphicsContext& instance, const Device& device,
//    MappedMemory& memory, size_t offset = 0)
//{
//    assert(offset % memory.getAlignment() == 0 && "offset must be aligned");    
//    auto paddedSize = memory.allignSize(m_capacity);
//    auto mapping = memory.getMapping(paddedSize, offset);
//    std::copy(m_pixels, m_pixels + m_capacity, mapping.data());
//}