#include "MappedMemory.h"

MappedMemory::MappedMemory(const EngineContext& instance, const EngineDevice& device,
    const vk::MemoryRequirements& memRequirements, MemoryProperty::Flags memoryProperties,
    size_t capacity) :
    m_memoryProperties(memoryProperties), m_capacity(capacity)
{
    m_memoryProperties |= MemoryProperty::Bits::HOST_VISIBLE_COHERENT;
    m_capacity = capacity;
    m_alignment = memRequirements.alignment;
    m_memoryTypeIndex = device.findMemoryType(instance, memRequirements.memoryTypeBits, memoryProperties);
    m_memory = device.allocateMemory(instance, m_capacity, m_memoryTypeIndex);
    m_data = device.getDevice().mapMemory(
        m_memory, 0, m_capacity, vk::MemoryMapFlags(), instance.getDispatchLoader());

#ifdef _DEBUG
    std::cout << "Allocated MappedMemory (" << capacity << " bytes)" << std::endl;
#endif

    m_initialized = true;
}

bool MappedMemory::bindBuffer(const EngineContext& instance, const EngineDevice& device, const Buffer& buffer, size_t offset) {
    if (!m_initialized)
        return false;

    device.getDevice().bindBufferMemory(buffer.getBuffer(), m_memory, offset, instance.getDispatchLoader());
#ifdef _DEBUG
    std::cout << "Bound buffer memory" << std::endl;
#endif
    return true;
}

bool MappedMemory::bindImage(const EngineContext& instance, const EngineDevice& device,
    const Image& image, size_t offset /*= 0*/)
{
    if (!m_initialized)
        return false;

    device.getDevice().bindImageMemory(image.getImage(), m_memory, offset, instance.getDispatchLoader());

#ifdef _DEBUG
    std::cout << "Bound image memory" << std::endl;
#endif

    return true;
}