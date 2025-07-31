#pragma once
#include "../Common.h"

// a convenience wrapper for storing pixel data
// since it is not instance local we can implement RAII safely
namespace Graphics
{
	class PixelData
	{
	private:
		size_t m_width = 0;
		size_t m_height = 0;
		size_t m_bpp = 0;
		size_t m_capacity = 0; //image size in bytes

		stbi_uc* m_pixels = nullptr;

	public:
        PixelData() : m_pixels(nullptr) {};

        PixelData(const std::string& filepath);

        PixelData(PixelData&& other) noexcept {
            m_width = std::exchange(other.m_width, 0);
            m_height = std::exchange(other.m_height, 0);
            m_bpp = std::exchange(other.m_bpp, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_pixels = std::exchange(other.m_pixels, nullptr);
        };

        //moving to an initialized swap chain is undefined behavior, destroy before moving
        PixelData& operator=(PixelData&& other) noexcept
        {
            if (this == &other)
                return *this;

            destroy();

            m_width = std::exchange(other.m_width, 0);
            m_height = std::exchange(other.m_height, 0);
            m_bpp = std::exchange(other.m_bpp, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_pixels = std::exchange(other.m_pixels, nullptr);

            return *this;
        };

        PixelData(const PixelData& other) noexcept = delete;
        PixelData& operator=(const PixelData& other) noexcept = delete;

        ~PixelData() { destroy(); };

        void load(const std::string& filepath);

        std::span<uint8_t> getPixelData() const { return  std::span<uint8_t>(m_pixels, m_capacity); };
        size_t getCapacity() const { return m_capacity; };
        size_t getWidth() const { return m_width; };
        size_t getHeight() const { return m_height; };
        Extent3D getExtent3D() const { return Extent3D(m_width, m_height, 1); };

        void destroy() {
            if (!m_pixels)
                return;

            stbi_image_free(m_pixels);
            m_width = 0;
            m_height = 0;
            m_bpp = 0;
            m_capacity = 0;
            m_pixels = nullptr;

#ifdef _DEBUG
            std::cout << "Destroyed PixelData" << std::endl;
#endif
        };

	};
}

