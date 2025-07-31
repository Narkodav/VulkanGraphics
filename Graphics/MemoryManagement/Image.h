#pragma once
#include "../Common.h"
#include "../Rendering/Context.h"
#include "../Rendering/Device.h"

namespace Graphics {

    class CommandBuffer;

    // basically works like a buffer but for images
    // also stores pixel data
    class Image
    {
    private:

        vk::Image m_image = nullptr;
        vk::ImageView m_view = nullptr;
        vk::MemoryRequirements m_imageMemoryRequirements = {};
        vk::ImageLayout m_layout = vk::ImageLayout::eUndefined;

        // initialised if there is data in m_pixels
        bool m_initialized = false;
    public:
        Image() : m_initialized(false) {};

        Image(const Context& instance, const Device& device,
            size_t width, size_t height);

        Image(Image&& other) noexcept {
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
                
        void createView(const Context& instance, const Device& device);

        vk::Image getImage() const { return m_image; };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;

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

}