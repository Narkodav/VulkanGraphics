#pragma once
#include "Common.h"
#include "EngineContext.h"
#include "Device.h"

class Sampler
{
public:
    enum class Filter : uint32_t
    {
        LINEAR = vk::Filter::eLinear,
        NEAREST = vk::Filter::eNearest,
        CUBIC_EXT = vk::Filter::eCubicEXT,
        CUBIC_IMG = vk::Filter::eCubicIMG,
    };

    enum class AddressMode : uint32_t
    {
        REPEAT = vk::SamplerAddressMode::eRepeat,
        CLAMP_TO_BORDER = vk::SamplerAddressMode::eClampToBorder,
        CLAMP_TO_EDGE = vk::SamplerAddressMode::eClampToEdge,
        MIRROR_CLAMP_TO_EDGE = vk::SamplerAddressMode::eMirrorClampToEdge,
        MIRRORED_REPEAT = vk::SamplerAddressMode::eMirroredRepeat,
    };

    class Descriptor
    {
    private:
        vk::SamplerCreateInfo m_samplerInfo;

    public:
        operator vk::SamplerCreateInfo() const { return m_samplerInfo; };

        Descriptor() {};
        Descriptor(const EngineContext& instance, const Device& device,
            Filter magFilter = Filter::LINEAR, Filter minFilter = Filter::LINEAR,
            AddressMode modeU = AddressMode::REPEAT,
            AddressMode modeV = AddressMode::REPEAT,
            AddressMode modeW = AddressMode::REPEAT)
        {
            m_samplerInfo.sType = vk::StructureType::eSamplerCreateInfo;
            m_samplerInfo.magFilter = static_cast<vk::Filter>(magFilter);
            m_samplerInfo.minFilter = static_cast<vk::Filter>(minFilter);
            m_samplerInfo.addressModeU = static_cast<vk::SamplerAddressMode>(modeU);
            m_samplerInfo.addressModeV = static_cast<vk::SamplerAddressMode>(modeV);
            m_samplerInfo.addressModeW = static_cast<vk::SamplerAddressMode>(modeW);

            m_samplerInfo.anisotropyEnable = VK_TRUE;
            m_samplerInfo.maxAnisotropy = 1.0f;

            vk::PhysicalDeviceProperties properties{};
            properties = device.getPhysicalDevice().getHandle().getProperties(instance.getDispatchLoader());
            m_samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

            m_samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
            m_samplerInfo.unnormalizedCoordinates = VK_FALSE;
            m_samplerInfo.compareEnable = VK_FALSE;
            m_samplerInfo.compareOp = vk::CompareOp::eAlways;

            m_samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
            m_samplerInfo.mipLodBias = 0.0f;
            m_samplerInfo.minLod = 0.0f;
            m_samplerInfo.maxLod = 0.0f;            
        }

        Descriptor(Descriptor&&) noexcept = default;
        Descriptor& operator=(Descriptor&&) noexcept = default;

        Descriptor(const Descriptor&) noexcept = default;
        Descriptor& operator=(const Descriptor&) noexcept = default;

        ~Descriptor() = default;

        vk::SamplerCreateInfo getSamplerInfo() const { return m_samplerInfo; };
    };

private:
    vk::Sampler m_sampler;
    Descriptor m_descriptor;

    bool m_initialized = false;
public:

    Sampler() {};
    Sampler(const EngineContext& instance, const Device& device, const Descriptor& descriptor);

    Sampler(Sampler&& other) noexcept {

        m_sampler = std::exchange(other.m_sampler, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);

    };

    //moving to an initialized device is undefined behavior, destroy before moving
    Sampler& operator=(Sampler&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "Sampler::operator=() - Sampler already initialized");

        m_sampler = std::exchange(other.m_sampler, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    Sampler(const Sampler&) noexcept = delete;
    Sampler& operator=(const Sampler&) noexcept = delete;

    ~Sampler() { assert(!m_initialized && "Sampler was not destroyed!"); };

    void destroy(const EngineContext& instance, const Device& device) {
        if (!m_initialized)
            return;

        device.getDevice().destroySampler(m_sampler, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
        std::cout << "Destroyed Sampler" << std::endl;
#endif
        m_initialized = false;
    }

    bool isInitialized() const { return m_initialized; };

    const vk::Sampler& getSampler() const { return m_sampler; };


};

