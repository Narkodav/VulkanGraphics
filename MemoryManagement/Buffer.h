#pragma once
#include "Common.h"
#include "Rendering/Flags.h"
#include "Rendering/EngineContext.h"
#include "Rendering/EngineDevice.h"
#include "BufferDataLayouts.h"

class Buffer
{
public:
    class Descriptor
    {
    private:
        vk::BufferCreateInfo m_bufferInfo;

    public:

        operator vk::BufferCreateInfo() const { return m_bufferInfo; };

        Descriptor() {};
        Descriptor(size_t byteSize, BufferUsage::Flags usageFlags, bool allowConcurrentAccess = false)
        {
            m_bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
            m_bufferInfo.size = byteSize;
            m_bufferInfo.usage = usageFlags;
            m_bufferInfo.sharingMode = allowConcurrentAccess ?
                vk::SharingMode::eConcurrent :
                vk::SharingMode::eExclusive;
        }

        Descriptor(Descriptor&&) noexcept = default;
        Descriptor& operator=(Descriptor&&) noexcept = default;

        Descriptor(const Descriptor&) noexcept = default;
        Descriptor& operator=(const Descriptor&) noexcept = default;

        ~Descriptor() = default;

        vk::BufferCreateInfo getBufferInfo() const { return m_bufferInfo; };
    };

private:
    vk::Buffer m_buffer = nullptr;
    vk::MemoryRequirements m_memRequirements;
    Descriptor m_descriptor;
    size_t m_capacity;
    size_t m_offset;

    bool m_initialized = false;
    bool m_bound = false;
public:

    Buffer() {};
    Buffer(const EngineContext& instance, const EngineDevice& device,
        size_t byteSize, BufferUsage::Flags usageFlags, bool allowConcurrentAccess = false);

    Buffer(const EngineContext& instance, const EngineDevice& device, Descriptor descriptor);

    Buffer(Buffer&& other) noexcept {

        m_buffer = std::exchange(other.m_buffer, nullptr);
        m_memRequirements = std::exchange(other.m_memRequirements, vk::MemoryRequirements());
        m_descriptor = std::exchange(other.m_descriptor, {});
        m_initialized = std::exchange(other.m_initialized, false);

    };

    //moving to an initialized device is undefined behavior, destroy before moving
    Buffer& operator=(Buffer&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "Buffer::operator=() - Buffer already initialized");

        m_buffer = std::exchange(other.m_buffer, nullptr);
        m_memRequirements = std::exchange(other.m_memRequirements, vk::MemoryRequirements());
        m_descriptor = std::exchange(other.m_descriptor, {});
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    Buffer(const Buffer&) noexcept = delete;
    Buffer& operator=(const Buffer&) noexcept = delete;

    ~Buffer() { assert(!m_initialized && "Buffer was not destroyed!"); };

    void destroy(const EngineContext& instance, const EngineDevice& device) {
        if (!m_initialized)
            return;

        device.getDevice().destroyBuffer(m_buffer, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
        std::cout << "Destroyed buffer" << std::endl;
#endif
        m_initialized = false;
    }

    Descriptor getDescriptor() const { return m_descriptor; };

    const vk::Buffer& getBuffer() const { return m_buffer; };
    const vk::MemoryRequirements& getMemoryRequirements() const { return m_memRequirements; };

    bool isInitialized() const { return m_initialized; };

};

