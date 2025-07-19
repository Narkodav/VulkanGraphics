#pragma once
#include "../Common.h"
#include "../Rendering/Flags.h"
#include "../Rendering/Device.h"
#include "../Rendering/Context.h"
#include "Buffer.h"
#include "Image.h"

namespace Graphics {

    class Memory
    {
    private:
        vk::DeviceMemory m_memory;
        MemoryProperty::Flags m_memoryProperties;
        size_t m_capacity = 0;
        size_t m_alignment = 0;
        uint32_t m_memoryTypeIndex = 0;

        bool m_initialized = false;
    public:

        Memory() {};

        Memory(const Context& instance, const Device& device,
            const vk::MemoryRequirements& memRequirements, MemoryProperty::Flags memoryProperties,
            size_t capacity);

        Memory(Memory&& other) noexcept {
            m_memory = std::exchange(other.m_memory, nullptr);
            m_memoryProperties = std::exchange(other.m_memoryProperties, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_alignment = std::exchange(other.m_alignment, 0);
            m_memoryTypeIndex = std::exchange(other.m_memoryTypeIndex, 0);
            m_initialized = std::exchange(other.m_initialized, false);
        };

        Memory& operator=(Memory&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "Memory::operator=() - Memory already initialized");

            m_memory = std::exchange(other.m_memory, nullptr);
            m_memoryProperties = std::exchange(other.m_memoryProperties, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_alignment = std::exchange(other.m_alignment, 0);
            m_memoryTypeIndex = std::exchange(other.m_memoryTypeIndex, 0);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        Memory(const Memory&) noexcept = delete;
        Memory& operator=(const Memory&) noexcept = delete;

        ~Memory() { assert(!m_initialized && "Memory was not destroyed!"); };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;

            device.getDevice().freeMemory(m_memory, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Freed memory" << std::endl;
#endif
            m_initialized = false;
        }

        bool bindBuffer(const Context& instance, const Device& device, const Buffer& buffer, size_t offset = 0);
        bool bindImage(const Context& instance, const Device& device, const Image& image, size_t offset = 0);
        vk::DeviceMemory getMemory() { return m_memory; };

        size_t getCapacity() const { return m_capacity; };
        size_t getAlignment() const { return m_alignment; };
    };

}