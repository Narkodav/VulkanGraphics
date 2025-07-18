#include "Memory.h"

namespace Graphics {

    Memory::Memory(const Context& instance, const Device& device,
        const vk::MemoryRequirements& memRequirements, MemoryProperty::Flags memoryProperties,
        size_t capacity) :
        m_memoryProperties(memoryProperties)
    {
        m_capacity = capacity;
        m_alignment = memRequirements.alignment;
        m_memoryTypeIndex = device.findMemoryType(instance, memRequirements.memoryTypeBits, memoryProperties);
        m_memory = device.allocateMemory(instance, m_capacity, m_memoryTypeIndex);
        m_initialized = true;
    }

    bool Memory::bindBuffer(const Context& instance, const Device& device, const Buffer& buffer, size_t offset) {
        if (!m_initialized)
            return false;

        device.getDevice().bindBufferMemory(buffer.getBuffer(), m_memory, offset, instance.getDispatchLoader());
#ifdef _DEBUG
        std::cout << "Bound buffer memory" << std::endl;
#endif
        return true;
    }

    bool Memory::bindImage(const Context& instance, const Device& device, const Image& image, size_t offset /*= 0*/)
    {
        if (!m_initialized)
            return false;

        device.getDevice().bindImageMemory(image.getImage(), m_memory, offset, instance.getDispatchLoader());

#ifdef _DEBUG
        std::cout << "Bound image memory" << std::endl;
#endif

        return true;
    }

}