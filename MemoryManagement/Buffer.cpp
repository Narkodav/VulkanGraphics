#include "Buffer.h"


Buffer::Buffer(const EngineContext& instance, const Device& device,
	size_t byteSize, BufferUsage::Flags usageFlags, bool allowConcurrentAccess /*= false*/) : m_capacity(byteSize)
{
    m_descriptor = Descriptor(byteSize, usageFlags, allowConcurrentAccess);

    try {
        m_buffer = device.getDevice().createBuffer(m_descriptor, nullptr, instance.getDispatchLoader());
        device.getDevice().getBufferMemoryRequirements(m_buffer, &m_memRequirements, instance.getDispatchLoader());
    }
    catch (const vk::SystemError& e) {
        throw std::runtime_error("failed to create a buffer: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unexpected error creating a buffer: " + std::string(e.what()));
    }

	m_initialized = true;
}

Buffer::Buffer(const EngineContext& instance, const Device& device, Descriptor descriptor) :
    m_descriptor(descriptor), m_capacity(m_descriptor.getBufferInfo().size)
{
    try {
        m_buffer = device.getDevice().createBuffer(m_descriptor, nullptr, instance.getDispatchLoader());
        device.getDevice().getBufferMemoryRequirements(m_buffer, &m_memRequirements, instance.getDispatchLoader());
    }
    catch (const vk::SystemError& e) {
        throw std::runtime_error("failed to create a buffer: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unexpected error creating a buffer: " + std::string(e.what()));
    }

    m_initialized = true;
}

//void Buffer::allocateMemory(const EngineContext& instance, const Device& device,
//    MemoryPropertyFlags memoryPropertyFlags)
//{
//    m_memory = device.allocateMemory(instance, m_buffer, memoryPropertyFlags);
//
//    try {
//        device.getDevice().bindBufferMemory(m_buffer, m_memory, 0, instance.getDispatchLoader());
//    }
//    catch (const vk::SystemError& e) {
//        throw std::runtime_error("failed to bind memory to a buffer: " + std::string(e.what()));
//    }
//    catch (const std::exception& e) {
//        throw std::runtime_error("Unexpected error binding memory to a buffer: " + std::string(e.what()));
//    }
//}