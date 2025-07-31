#include "PixelData.h"

namespace Graphics {

    PixelData::PixelData(const std::string& filepath)
    {
        load(filepath);
    }

    void PixelData::load(const std::string& filepath)
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
    }
}