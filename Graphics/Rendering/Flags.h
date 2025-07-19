#pragma once
#include "../Common.h"

#include <type_traits>

namespace Graphics
{
    template <typename Definition>
    class Flags
    {
    protected:
        uint32_t m_flags;

    public:
        using Bits = Definition::Bits;
        using VulkanFlags = Definition::VulkanFlags;

        Flags() : m_flags(0) {};
        Flags(typename Bits bit) : m_flags(static_cast<uint32_t>(bit)) {}
        Flags(uint32_t flags) : m_flags(flags) {}
        Flags(typename VulkanFlags flags) : m_flags(static_cast<uint32_t>(flags)) {};

        operator VulkanFlags() const { return VulkanFlags(this->m_flags); }
        operator uint32_t() const { return m_flags; }

        inline Flags operator|(const Bits& other) const {
            return static_cast<Flags>(m_flags | static_cast<uint32_t>(other));
        }

        inline Flags operator|(const Flags& other) const {
            return static_cast<Flags>(m_flags | other.m_flags);
        }

        inline Flags& operator|=(const Bits& other) {
            m_flags |= static_cast<uint32_t>(other);
            return *this;
        }

        inline Flags& operator|=(const Flags& other) {
            m_flags |= static_cast<uint32_t>(other);
            return *this;
        }

        inline Flags operator&(const Bits& other) const {
            return static_cast<Flags>(m_flags & static_cast<uint32_t>(other));
        }

        inline Flags operator&(const Flags& other) const {
            return static_cast<Flags>(m_flags & other.m_flags);
        }

        inline Flags& operator&=(const Bits& other) {
            m_flags &= static_cast<uint32_t>(other);
            return *this;
        }

        inline Flags& operator&=(const Flags& other) {
            m_flags &= static_cast<uint32_t>(other);
            return *this;
        }

        inline bool operator==(const Flags& other) const {
            return m_flags == static_cast<uint32_t>(other);
        }

        inline bool operator!=(const Flags& other) const {
            return m_flags != static_cast<uint32_t>(other);
        }

        inline bool operator==(const Bits& other) const {
            return m_flags == static_cast<uint32_t>(other);
        }

        inline bool operator!=(const Bits& other) const {
            return m_flags != static_cast<uint32_t>(other);
        }

        inline bool operator==(const uint32_t& other) const {
            return m_flags == other;
        }

        inline bool operator!=(const uint32_t& other) const {
            return m_flags != other;
        }

        bool hasFlag(const Bits& flag) const {
            return (m_flags & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
        }

        bool hasFlags(const Flags& flags) const {
            return (m_flags & flags.m_flags) == flags.m_flags;
        }

        void clear() { m_flags = 0; };
    };

    template<typename T>
    struct BitTraits;

    template <typename Bits>
    inline Flags<typename BitTraits<Bits>::ParentType> operator|(const Bits& a, const Bits& b) {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline Flags<typename BitTraits<Bits>::ParentType> operator&(const Bits& a, const Bits& b) {
        return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator==(const Bits& a, const Flags<typename BitTraits<Bits>::ParentType>& b) {
        return static_cast<uint32_t>(a) == static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator!=(const Bits& a, const Flags<typename BitTraits<Bits>::ParentType>& b) {
        return static_cast<uint32_t>(a) != static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator==(const uint32_t& a, const Flags<typename BitTraits<Bits>::ParentType>& b) {
        return a == static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator!=(const uint32_t& a, const Flags<typename BitTraits<Bits>::ParentType>& b) {
        return a != static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator==(const uint32_t& a, const Bits& b) {
        return a == static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator!=(const uint32_t& a, const Bits& b) {
        return a != static_cast<uint32_t>(b);
    }

    template <typename Bits>
    inline bool operator==(const Bits& a, const uint32_t& b) {
        return b == static_cast<uint32_t>(a);
    }

    template <typename Bits>
    inline bool operator!=(const Bits& a, const uint32_t& b) {
        return b != static_cast<uint32_t>(a);
    }

    struct MemoryProperty
    {
        enum class Bits : uint32_t {
            None = 0,
            DeviceLocal = vk::MemoryPropertyFlagBits::eDeviceLocal,
            HostVisible = vk::MemoryPropertyFlagBits::eHostVisible,
            HostCoherent = vk::MemoryPropertyFlagBits::eHostCoherent,
            HostCached = vk::MemoryPropertyFlagBits::eHostCached,
            LazilyAllocated = vk::MemoryPropertyFlagBits::eLazilyAllocated,
            Protected = vk::MemoryPropertyFlagBits::eProtected,

            HostVisibleCoherent = static_cast<uint32_t>(HostVisible) | static_cast<uint32_t>(HostCoherent),
            DeviceLocalVisible = static_cast<uint32_t>(DeviceLocal) | static_cast<uint32_t>(HostVisible),
        };

        using VulkanFlags = vk::MemoryPropertyFlags;
        using Flags = Flags<MemoryProperty>;
    };

    template<>
    struct BitTraits<MemoryProperty::Bits> {
        using ParentType = MemoryProperty;
    };

    struct BufferUsage
    {
        enum class Bits : uint32_t
        {
            None = 0,
            Vertex = vk::BufferUsageFlagBits::eVertexBuffer,
            Index = vk::BufferUsageFlagBits::eIndexBuffer,
            Uniform = vk::BufferUsageFlagBits::eUniformBuffer,
            Storage = vk::BufferUsageFlagBits::eStorageBuffer,
            TransferSrc = vk::BufferUsageFlagBits::eTransferSrc,    // Buffer can be used as source in transfer commands
            TransferDst = vk::BufferUsageFlagBits::eTransferDst,    // Buffer can be used as destination in transfer commands
            Indirect = vk::BufferUsageFlagBits::eIndirectBuffer,     // For indirect drawing commands
            ShaderDeviceAddress = vk::BufferUsageFlagBits::eShaderDeviceAddress,  // Buffer can be used to get device address
            StorageTexelBuffer = vk::BufferUsageFlagBits::eStorageTexelBuffer,    // Buffer can be used as a storage texel buffer
            UniformTexelBuffer = vk::BufferUsageFlagBits::eUniformTexelBuffer,    // Buffer can be used as a uniform texel buffer

            VertexIndex = static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(Index),
            VertexUniform = static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(Uniform),
        };

        using VulkanFlags = vk::BufferUsageFlags;
        using Flags = Flags<BufferUsage>;
    };

    template<>
    struct BitTraits<BufferUsage::Bits> {
        using ParentType = BufferUsage;
    };

    struct CommandBufferUsage
    {
        enum class Bits : uint32_t
        {
            None = 0,
            OneTimeSubmit = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
            SimultaneousUse = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
            RenderPassContinue = vk::CommandBufferUsageFlagBits::eRenderPassContinue,
            SimultaneousUseRenderPassContinue =
            static_cast<uint32_t>(SimultaneousUse) | static_cast<uint32_t>(RenderPassContinue),
        };

        using VulkanFlags = vk::CommandBufferUsageFlags;
        using Flags = Flags<CommandBufferUsage>;
    };

    template<>
    struct BitTraits<CommandBufferUsage::Bits> {
        using ParentType = CommandBufferUsage;
    };

    struct DebugMessageSeverity
    {
        enum class Bits : uint32_t
        {
            None = 0,
            Verbose = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
            Info = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
            Warning = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
            Error = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            All = Verbose | Info | Warning | Error
        };

        using VulkanFlags = vk::DebugUtilsMessageSeverityFlagsEXT;
        using Flags = Flags<DebugMessageSeverity>;
    };

    template<>
    struct BitTraits<DebugMessageSeverity::Bits> {
        using ParentType = DebugMessageSeverity;
    };

    struct DescriptorSetLayoutCreate
    {
        enum class Bits : uint32_t
        {
            None = 0,
            UpdateAfterBind = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
        };

        using VulkanFlags = vk::DescriptorSetLayoutCreateFlags;
        using Flags = Flags<DescriptorSetLayoutCreate>;
    };

    template<>
    struct BitTraits<DescriptorSetLayoutCreate::Bits> {
        using ParentType = DescriptorSetLayoutCreate;
    };

    struct QueueFlags
    {
        enum class Bits : uint32_t {
            None = 0,
            Graphics = vk::QueueFlagBits::eGraphics,
            Transfer = vk::QueueFlagBits::eTransfer,
            Compute = vk::QueueFlagBits::eCompute,
            SparseBinding = vk::QueueFlagBits::eSparseBinding,
            Protected = vk::QueueFlagBits::eProtected,
            VideoDecode = vk::QueueFlagBits::eVideoDecodeKHR,
            VideoEncode = vk::QueueFlagBits::eVideoEncodeKHR,
            OpticalFlowNv = vk::QueueFlagBits::eOpticalFlowNV,
        };

        using VulkanFlags = vk::QueueFlags;
        using Flags = Flags<QueueFlags>;
    };

    template<>
    struct BitTraits<QueueFlags::Bits> {
        using ParentType = QueueFlags;
    };

    struct DescriptorPoolCreateFlags
    {
        enum class Bits : uint32_t {
            None = 0,
            UpdateAfterBind = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
        };

        using VulkanFlags = vk::DescriptorPoolCreateFlags;
        using Flags = Flags<DescriptorPoolCreateFlags>;
    };

    template<>
    struct BitTraits<DescriptorPoolCreateFlags::Bits> {
        using ParentType = DescriptorPoolCreateFlags;
    };

}