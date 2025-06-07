#pragma once
#include "Common.h"

#include <type_traits>

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

    inline Flags operator|(Bits other) const {
        return static_cast<Flags>(m_flags | static_cast<uint32_t>(other));
    }

    inline Flags operator|(Flags other) const {
        return static_cast<Flags>(m_flags | other.m_flags);
    }

    inline Flags& operator|=(Bits other) {
        m_flags |= static_cast<uint32_t>(other);
        return *this;
    }

    inline Flags& operator|=(Flags other) {
        m_flags |= static_cast<uint32_t>(other);
        return *this;
    }

    inline Flags operator&(Bits other) const {
        return static_cast<Flags>(m_flags & static_cast<uint32_t>(other));
    }

    inline Flags operator&(Flags other) const {
        return static_cast<Flags>(m_flags & other.m_flags);
    }

    inline Flags& operator&=(Bits other) {
        m_flags &= static_cast<uint32_t>(other);
        return *this;
    }

    inline Flags& operator&=(Flags other) {
        m_flags &= static_cast<uint32_t>(other);
        return *this;
    }

    bool hasFlag(Bits flag) const {
        return (m_flags & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
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

struct MemoryProperty
{
    enum class Bits : uint32_t {
        DEVICE_LOCAL = vk::MemoryPropertyFlagBits::eDeviceLocal,
        HOST_VISIBLE = vk::MemoryPropertyFlagBits::eHostVisible,
        HOST_COHERENT = vk::MemoryPropertyFlagBits::eHostCoherent,
        HOST_CACHED = vk::MemoryPropertyFlagBits::eHostCached,
        LAZILY_ALLOCATED = vk::MemoryPropertyFlagBits::eLazilyAllocated,
        PROTECTED = vk::MemoryPropertyFlagBits::eProtected,

        HOST_VISIBLE_COHERENT = static_cast<uint32_t>(HOST_VISIBLE) | static_cast<uint32_t>(HOST_COHERENT),
        DEVICE_LOCAL_VISIBLE = static_cast<uint32_t>(DEVICE_LOCAL) | static_cast<uint32_t>(HOST_VISIBLE),
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
        VERTEX = vk::BufferUsageFlagBits::eVertexBuffer,
        INDEX = vk::BufferUsageFlagBits::eIndexBuffer,
        UNIFORM = vk::BufferUsageFlagBits::eUniformBuffer,
        STORAGE = vk::BufferUsageFlagBits::eStorageBuffer,
        TRANSFER_SRC = vk::BufferUsageFlagBits::eTransferSrc,    // Buffer can be used as source in transfer commands
        TRANSFER_DST = vk::BufferUsageFlagBits::eTransferDst,    // Buffer can be used as destination in transfer commands
        INDIRECT = vk::BufferUsageFlagBits::eIndirectBuffer,     // For indirect drawing commands
        SHADER_DEVICE_ADDRESS = vk::BufferUsageFlagBits::eShaderDeviceAddress,  // Buffer can be used to get device address

        VERTEX_INDEX = static_cast<uint32_t>(VERTEX) | static_cast<uint32_t>(INDEX),
        VERTEX_UNIFORM = static_cast<uint32_t>(VERTEX) | static_cast<uint32_t>(UNIFORM),
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
    enum class Bits
    {
        ONE_TIME_SUBMIT = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        SIMULTANEOUS_USE = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
        RENDER_PASS_CONTINUE = vk::CommandBufferUsageFlagBits::eRenderPassContinue,
        SIMULTANEOUS_USE_RENDER_PASS_CONTINUE = 
        static_cast<uint32_t>(SIMULTANEOUS_USE) | static_cast<uint32_t>(RENDER_PASS_CONTINUE),
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
    enum class Bits
    {
        NONE = 0,
        VERBOSE = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
        INFO = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
        WARNING = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        ERROR = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        ALL = VERBOSE | INFO | WARNING | ERROR
    };

    using VulkanFlags = vk::DebugUtilsMessageSeverityFlagsEXT;
    using Flags = Flags<DebugMessageSeverity>;
};

template<>
struct BitTraits<DebugMessageSeverity::Bits> {
    using ParentType = DebugMessageSeverity;
};