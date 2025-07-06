#pragma once
#include "../Common.h"
#include "GraphicsContext.h"
#include "Device.h"
#include "../BufferDataLayouts.h"

class DescriptorSetLayout
{
private:
	vk::DescriptorSetLayout m_layout = nullptr;
    vk::DescriptorSetLayoutCreateInfo m_layoutInfo;
    std::vector<vk::DescriptorSetLayoutBinding> m_bindings;

	bool m_initialized = false;

public:

	DescriptorSetLayout() = default;

    template <DescriptorDefinition... DescriptorDefs>
    DescriptorSetLayout(const GraphicsContext& instance, const Device& device,
        const DescriptorDefinitions<DescriptorDefs...>& descriptors,
        DescriptorSetLayoutCreate::Flags flags = DescriptorSetLayoutCreate::Bits::NONE) {

        m_bindings = descriptors.enumerateDescriptors();
        m_layoutInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
        m_layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        m_layoutInfo.pBindings = m_bindings.data();
        vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
        std::vector<vk::DescriptorBindingFlags> bindingFlags;

        if(flags != DescriptorSetLayoutCreate::Bits::NONE)
        {            
            bindingFlagsInfo.sType = vk::StructureType::eDescriptorSetLayoutBindingFlagsCreateInfo;
            bindingFlagsInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());

            bindingFlags = descriptors.enumerateBindingFlags();
            bindingFlagsInfo.pBindingFlags = bindingFlags.data();

            m_layoutInfo.pNext = &bindingFlagsInfo;
            m_layoutInfo.flags = flags;
        }

        try {
            m_layout = device.getDevice().
                createDescriptorSetLayout(m_layoutInfo, nullptr, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("Failed to create descriptor set layout: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error when creating a descriptor set layout: " + std::string(e.what()));
        }

        m_initialized = true;
    };

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept {
        m_layout = std::exchange(other.m_layout, nullptr);
        m_bindings = std::exchange(other.m_bindings, std::vector<vk::DescriptorSetLayoutBinding>{});
        m_layoutInfo = std::exchange(other.m_layoutInfo, vk::DescriptorSetLayoutCreateInfo{});
        m_initialized = std::exchange(other.m_initialized, false);
    };

    //moving to an initialized DescriptorSetLayout is undefined behavior, destroy before moving
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized &&
            "DescriptorSetLayout::operator=() - DescriptorSetLayout already initialized");

        m_layout = std::exchange(other.m_layout, nullptr);
        m_bindings = std::exchange(other.m_bindings, std::vector<vk::DescriptorSetLayoutBinding>{});
        m_layoutInfo = std::exchange(other.m_layoutInfo, vk::DescriptorSetLayoutCreateInfo{});
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    DescriptorSetLayout(const DescriptorSetLayout& other) noexcept = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout& other) noexcept = delete;

    ~DescriptorSetLayout() { assert(!m_initialized && "DescriptorSetLayout was not destroyed!"); };

    void destroy(const GraphicsContext& instance, const Device& device) {
        if (!m_initialized)
            return;

        device.getDevice().destroyDescriptorSetLayout(
            m_layout, nullptr, instance.getDispatchLoader());

#ifdef _DEBUG
        std::cout << "Destroyed DescriptorSetLayout" << std::endl;
#endif
        m_initialized = false;
    }

    const vk::DescriptorSetLayout& getLayout() const { return m_layout; };
    const vk::DescriptorSetLayoutCreateInfo& getLayoutInfo() const { return m_layoutInfo; };
};

