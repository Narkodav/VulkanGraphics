#pragma once
#include "../Common.h"
#include "../Rendering/Context.h"
#include "../Rendering/Device.h"
#include "../PlatformManagement/Window.h"
#include "Buffer.h"
#include "Image.h"
#include "../Rendering/Sampler.h"

namespace Graphics {

    class DescriptorPool;

    class DescriptorSet
    {
    private:
        vk::DescriptorSet m_set;
        std::unordered_map<size_t, vk::DescriptorSetLayoutBinding> m_layout; // layouts mapped to binding numbers

        bool m_initialized = false;

        DescriptorSet(vk::DescriptorSet set, vk::DescriptorSetLayoutCreateInfo layout);

    public:

        DescriptorSet() : m_set(nullptr), m_initialized(false) {};

        DescriptorSet(DescriptorSet&& other) noexcept {
            m_set = std::exchange(other.m_set, nullptr);
            m_layout = std::exchange(other.m_layout, {});
            m_initialized = std::exchange(other.m_initialized, false);
        };

        //moving to an initialized swap chain is undefined behavior, destroy before moving
        DescriptorSet& operator=(DescriptorSet&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "DescriptorSet::operator=() - DescriptorSet already initialized");

            m_set = std::exchange(other.m_set, nullptr);
            m_layout = std::exchange(other.m_layout, {});
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        DescriptorSet(const DescriptorSet&) noexcept = delete;
        DescriptorSet& operator=(const DescriptorSet&) noexcept = delete;

        ~DescriptorSet() {
            assert(!m_initialized && "DescriptorSet was not destroyed!");
            //assert(m_allocatedBuffers.size() == 0 && "DescriptorPool has allocated buffers!");
        };

        void write(const Context& instance, const Device& device,
            const Buffer& buffer, uint32_t binding,
            size_t offset, size_t range);

        void write(const Context& instance, const Device& device,
            const std::vector<Buffer>& buffers, uint32_t binding,
            const std::vector<size_t>& offsets, const std::vector<size_t>& ranges);

        void write(const Context& instance, const Device& device,
            const Image& image, const Sampler& sampler, uint32_t binding);

        void write(const Context& instance, const Device& device,
            const std::vector<Image>& images, const std::vector<const Sampler*>& samplers, uint32_t binding);

        vk::DescriptorSet getSet() const { return m_set; };

        friend class DescriptorPool;
    };

}