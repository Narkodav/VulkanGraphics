#pragma once
#include "../Common.h"
#include "../Rendering/Flags.h"
#include "../Rendering/Device.h"
#include "../Rendering/Context.h"
#include "Buffer.h"
#include "Image.h"

namespace Graphics {

    //automatically created with host visible and coherent flags and allows mappings
    class MappedMemory
    {
    private:
        vk::DeviceMemory m_memory;
        MemoryProperty::Flags m_memoryProperties;
        size_t m_capacity = 0;
        size_t m_alignment = 0;
        uint32_t m_memoryTypeIndex = 0;
        void* m_data = nullptr;

        bool m_initialized = false;
    public:

        MappedMemory() {};

        // Automatically adds HOST_VISIBLE | HOST_COHERENT to `memoryProperties`.
        MappedMemory(const Context& instance, const Device& device,
            const vk::MemoryRequirements& memRequirements, MemoryProperty::Flags memoryProperties,
            size_t capacity);

        MappedMemory(MappedMemory&& other) noexcept {
            m_memory = std::exchange(other.m_memory, nullptr);
            m_memoryProperties = std::exchange(other.m_memoryProperties, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_alignment = std::exchange(other.m_alignment, 0);
            m_memoryTypeIndex = std::exchange(other.m_memoryTypeIndex, 0);
            m_data = std::exchange(other.m_data, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

        };

        MappedMemory& operator=(MappedMemory&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "MappedMemory::operator=() - MappedMemory already initialized");

            m_memory = std::exchange(other.m_memory, nullptr);
            m_memoryProperties = std::exchange(other.m_memoryProperties, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_alignment = std::exchange(other.m_alignment, 0);
            m_data = std::exchange(other.m_data, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        MappedMemory(const MappedMemory&) noexcept = delete;
        MappedMemory& operator=(const MappedMemory&) noexcept = delete;

        ~MappedMemory() { assert(!m_initialized && "MappedMemory was not destroyed!"); };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;

            device.getDevice().unmapMemory(m_memory, instance.getDispatchLoader());
            device.getDevice().freeMemory(m_memory, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Freed MappedMemory" << std::endl;
#endif
            m_initialized = false;
        }

        bool bindBuffer(const Context& instance, const Device& device,
            const Buffer& buffer, size_t offset = 0);

        bool bindImage(const Context& instance, const Device& device,
            const Image& image, size_t offset = 0);

        // size in T objects, offset in bytes, size * sizeof(T) and offset must be multiples of alignment
        template<typename T = uint8_t>
        std::span<T> getMapping(size_t size, size_t offset = 0)
        {
            assert(m_initialized && "MappedMemory not initialized!");
            assert(m_data && "MappedMemory has no valid mapping!");
            assert(offset % m_alignment == 0 && "MappedMemory::getMapping() - Offset * sizeof(T) must be a multiple of alignment");
            assert((size * sizeof(T)) % m_alignment == 0 && "MappedMemory::getMapping() - size * sizeof(T) must be a multiple of alignment");
            assert(offset + size * sizeof(T) <= m_capacity && "MappedMemory::getMapping() - Mapping exceeds buffer capacity");
            void* offsettedData = static_cast<void*>(static_cast<uint8_t*>(m_data) + offset);
            return std::span<T>(static_cast<T*>(offsettedData), size);
        }

        // offset in bytes
        template<typename T = uint8_t>
        std::span<const T> getMapping(size_t size, size_t offset = 0) const
        {
            assert(m_initialized && "MappedMemory not initialized!");
            assert(m_data && "MappedMemory has no valid mapping!");
            assert(offset % m_alignment == 0 && "MappedMemory::getMapping() - Offset * sizeof(T) must be a multiple of alignment");
            assert((size * sizeof(T)) % m_alignment == 0 && "MappedMemory::getMapping() - size * sizeof(T) must be a multiple of alignment");
            assert(offset + size * sizeof(T) <= m_capacity && "MappedMemory::getMapping() - Mapping exceeds buffer capacity");
            void* ofsettedData = static_cast<void*>(static_cast<uint8_t*>(m_data) + offset);
            return std::span<const T>(static_cast<const T*>(ofsettedData), size);
        }

        // return the amount of T objects that can fit in the buffer
        // Note: doesn't check for alignment compliance
        template<typename T = uint8_t>
        size_t getMaxElements()
        {
            return m_capacity / sizeof(T);
        }

        size_t getCapacity() const { return m_capacity; };
        size_t getAlignment() const { return m_alignment; };

        // adds padding to meet alignment requirements, default T = uint8_t
        template<typename T = uint8_t>
        size_t allignSize(size_t size)
        {
            size_t padding = 0;
            while (((size + padding) * sizeof(T)) % m_alignment != 0)
            {
                padding++;
            }
            return size + padding;
        }

        // specialization for bytes
        template<>
        size_t allignSize<uint8_t>(size_t size)
        {
            size_t alignedBytes = ((size + m_alignment - 1) / m_alignment) * m_alignment;
            return alignedBytes;
        }

        template<typename T = uint8_t>
        void printBytes(size_t size) const
        {
            std::cout << "MappedMemory: " << std::endl;

            for (int i = 0; i < size; i++)
            {
                std::cout << static_cast<T*>(m_data)[i] << " ";
            }
            std::cout << std::endl;
        }
    };

}