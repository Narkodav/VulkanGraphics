#pragma once
#include "../Common.h"
#include "../Rendering/GraphicsContext.h"
#include "../Rendering/Device.h"

class CommandBuffer;

// basically works like a buffer but for images
// also stores pixel data
class Image
{
private:

    size_t m_width = 0;
    size_t m_height = 0;
    size_t m_bpp = 0;
    size_t m_capacity = 0; //image size in bytes

    stbi_uc* m_pixels = nullptr;

    vk::Image m_image = nullptr;
    vk::ImageView m_view = nullptr;
    vk::MemoryRequirements m_imageMemoryRequirements = {};
    vk::ImageLayout m_layout = vk::ImageLayout::eUndefined;

    // initialised if there is data in m_pixels
    bool m_initialized = false;
public:
    Image() : m_initialized(false) {};

    Image(const GraphicsContext& instance, const Device& device, const std::string& filepath);

    Image(Image&& other) noexcept {
        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
        m_bpp = std::exchange(other.m_bpp, 0);
        m_capacity = std::exchange(other.m_capacity, 0);
        m_pixels = std::exchange(other.m_pixels, nullptr);
        m_image = std::exchange(other.m_image, vk::Image());
        m_view = std::exchange(other.m_view, vk::ImageView());
        m_imageMemoryRequirements = std::exchange(other.m_imageMemoryRequirements, vk::MemoryRequirements());
        m_layout = std::exchange(other.m_layout, vk::ImageLayout::eUndefined);
        m_initialized = std::exchange(other.m_initialized, false);
    };

    //moving to an initialized swap chain is undefined behavior, destroy before moving
    Image& operator=(Image&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "Image::operator=() - Image already initialized");

        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
        m_bpp = std::exchange(other.m_bpp, 0);
        m_capacity = std::exchange(other.m_capacity, 0);
        m_pixels = std::exchange(other.m_pixels, nullptr);
        m_image = std::exchange(other.m_image, vk::Image());
        m_view = std::exchange(other.m_view, vk::ImageView());
        m_imageMemoryRequirements = std::exchange(other.m_imageMemoryRequirements, vk::MemoryRequirements());
        m_layout = std::exchange(other.m_layout, vk::ImageLayout::eUndefined);
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    Image(const Image& other) noexcept = delete;
    Image& operator=(const Image& other) noexcept = delete;

    ~Image() { assert(!m_initialized && "Image::~Image() - Image was not destroyed!"); };

    void load(const GraphicsContext& instance, const Device& device, const std::string& filepath);
    void createView(const GraphicsContext& instance, const Device& device);

    std::span<uint8_t> getPixelData() const { return  std::span<uint8_t>(m_pixels, m_capacity); };
    vk::Image getImage() const { return m_image; };
    size_t getCapacity() const { return m_capacity; };
    size_t getWidth() const { return m_width; };
    size_t getHeight() const { return m_height; };

    void destroy(const GraphicsContext& instance, const Device& device) {
        if (!m_initialized)
            return;

        stbi_image_free(m_pixels);
        m_width = 0;
        m_height = 0;
        m_bpp = 0;
        m_capacity = 0;
        m_pixels = nullptr;

        device.getDevice().destroyImageView(m_view, nullptr, instance.getDispatchLoader());
        device.getDevice().destroyImage(m_image, nullptr, instance.getDispatchLoader());

#ifdef _DEBUG
        std::cout << "Destroyed TextureImage" << std::endl;
#endif
        m_initialized = false;
    };

    const auto& getMemoryRequirements() const {
        assert(m_initialized && "Image::getMemoryRequirements() - Image is not initialized");
        return m_imageMemoryRequirements;
    }

    auto getLayout() const { return m_layout; };
    auto getView() const { return m_view; };
private:
    void setLayout(vk::ImageLayout layout) { m_layout = layout; };

    friend class CommandBuffer;
};

