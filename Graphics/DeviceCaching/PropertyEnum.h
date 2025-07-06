#pragma once

enum class DeviceProperty : size_t {
    //basic
    API_VERSION,                                // expects std::any containing uint32_t
    DRIVER_VERSION,                             // expects std::any containing uint32_t
    VENDOR_ID,                                  // expects std::any containing uint32_t
    DEVICE_ID,                                  // expects std::any containing uint32_t
    DEVICE_TYPE,                                // expects std::any containing PhysicalDeviceType
    DEVICE_NAME,                                // expects std::any containing std::string

    // Core Limits
    MAX_IMAGE_DIMENSION_2D,                     // expects std::any containing uint32_t
    MAX_IMAGE_DIMENSION_3D,                     // expects std::any containing uint32_t
    MAX_IMAGE_DIMENSION_CUBE,                   // expects std::any containing uint32_t
    MAX_IMAGE_ARRAY_LAYERS,                     // expects std::any containing uint32_t

    // Descriptor Limits
    MAX_BOUND_DESCRIPTOR_SETS,                  // expects std::any containing uint32_t
    MAX_PER_STAGE_DESCRIPTOR_SAMPLERS,          // expects std::any containing uint32_t
    MAX_PER_STAGE_DESCRIPTOR_UNIFORM_BUFFERS,   // expects std::any containing uint32_t
    MAX_PER_STAGE_DESCRIPTOR_STORAGE_BUFFERS,   // expects std::any containing uint32_t

    // Buffer Limits
    MAX_STORAGE_BUFFER_RANGE,                   // expects std::any containing uint64_t (VkDeviceSize)
    MAX_UNIFORM_BUFFER_RANGE,                   // expects std::any containing uint64_t (VkDeviceSize)
    MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT,        // expects std::any containing uint64_t (VkDeviceSize)
    MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT,        // expects std::any containing uint64_t (VkDeviceSize)

    // Viewport/Scissor
    MAX_VIEWPORT_DIMENSIONS,                    // expects std::any containing std::array<uint32_t, 2>
    MAX_VIEWPORTS,                              // expects std::any containing uint32_t
    VIEWPORT_BOUNDS_RANGE,                      // expects std::any containing std::array<float, 2>
    VIEWPORT_SUB_PIXEL_BITS,                    // expects std::any containing uint32_t

    // Sample Limits
    MAX_FRAMEBUFFER_LAYERS,                     // expects std::any containing uint32_t
    MAX_SAMPLE_MASK_WORDS,                      // expects std::any containing uint32_t
    MAX_COLOR_ATTACHMENTS,                      // expects std::any containing uint32_t

    // Memory Limits
    MAX_MEMORY_ALLOCATION_COUNT,                // expects std::any containing uint32_t
    MAX_SAMPLER_ALLOCATION_COUNT,               // expects std::any containing uint32_t
    BUFFER_IMAGE_GRANULARITY,                   // expects std::any containing uint64_t (VkDeviceSize)
    SPARSE_ADDRESS_SPACE_SIZE,                  // expects std::any containing uint64_t (VkDeviceSize)

    // Quality Settings
    MAX_SAMPLER_ANISOTROPY,                     // expects std::any containing float
    MAX_SAMPLER_LOD_BIAS,                       // expects std::any containing float
    TIMESTAMP_PERIOD,                           // expects std::any containing float

    PROPERTIES_NUM,
};

enum class PhysicalDeviceType
{
    DEVICE_OTHER = vk::PhysicalDeviceType::eOther,
    INTEGRATED_GPU = vk::PhysicalDeviceType::eIntegratedGpu,
    DISCRETE_GPU = vk::PhysicalDeviceType::eDiscreteGpu,
    VIRTUAL_GPU = vk::PhysicalDeviceType::eVirtualGpu,
    CPU = vk::PhysicalDeviceType::eCpu,
};

template<DeviceProperty P>
struct DevicePropertyTypeTrait;

// Basic properties
template<> struct DevicePropertyTypeTrait<DeviceProperty::API_VERSION> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::DRIVER_VERSION> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::VENDOR_ID> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::DEVICE_ID> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::DEVICE_TYPE> { using Type = PhysicalDeviceType; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::DEVICE_NAME> { using Type = std::string; };

// Core Limits
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_IMAGE_DIMENSION_2D> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_IMAGE_DIMENSION_3D> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_IMAGE_DIMENSION_CUBE> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_IMAGE_ARRAY_LAYERS> { using Type = uint32_t; };

// Descriptor Limits
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_BOUND_DESCRIPTOR_SETS> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_PER_STAGE_DESCRIPTOR_SAMPLERS> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_PER_STAGE_DESCRIPTOR_UNIFORM_BUFFERS> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_PER_STAGE_DESCRIPTOR_STORAGE_BUFFERS> { using Type = uint32_t; };

// Buffer Limits
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_STORAGE_BUFFER_RANGE> { using Type = uint64_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_UNIFORM_BUFFER_RANGE> { using Type = uint64_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT> { using Type = uint64_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT> { using Type = uint64_t; };

// Viewport/Scissor
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_VIEWPORT_DIMENSIONS> { using Type = std::array<uint32_t, 2>; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_VIEWPORTS> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::VIEWPORT_BOUNDS_RANGE> { using Type = std::array<float, 2>; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::VIEWPORT_SUB_PIXEL_BITS> { using Type = uint32_t; };

// Sample Limits
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_FRAMEBUFFER_LAYERS> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_SAMPLE_MASK_WORDS> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_COLOR_ATTACHMENTS> { using Type = uint32_t; };

// Memory Limits
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_MEMORY_ALLOCATION_COUNT> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_SAMPLER_ALLOCATION_COUNT> { using Type = uint32_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::BUFFER_IMAGE_GRANULARITY> { using Type = uint64_t; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::SPARSE_ADDRESS_SPACE_SIZE> { using Type = uint64_t; };

// Quality Settings
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_SAMPLER_ANISOTROPY> { using Type = float; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::MAX_SAMPLER_LOD_BIAS> { using Type = float; };
template<> struct DevicePropertyTypeTrait<DeviceProperty::TIMESTAMP_PERIOD> { using Type = float; };
