#pragma once

enum class DeviceFeature : size_t {
    // Vulkan 1.0 Features

    // Shader Pipeline Features
    GEOMETRY_SHADER = 0,
    TESSELLATION_SHADER,
    SHADER_FLOAT64,

    // Sampling and Texture Features
    SAMPLER_ANISOTROPY,
    FILL_MODE_NON_SOLID,
    TEXTURE_COMPRESSION_BC,
    TEXTURE_COMPRESSION_ETC2,

    // Drawing Features
    MULTI_DRAW_INDIRECT,
    WIDE_LINES,
    LARGE_POINTS,
    MULTI_VIEWPORT,

    // Depth and Blend Features
    DEPTH_CLAMP,
    DEPTH_BIAS_CLAMP,
    DUAL_SRC_BLEND,

    // Storage and Atomic Features
    VERTEX_PIPELINE_STORES_AND_ATOMICS,
    FRAGMENT_STORES_AND_ATOMICS,

    // Vulkan 1.2 Features
    SHADER_BUFFER_INT64_ATOMICS,    // Requires VkPhysicalDeviceShaderAtomicInt64Features
    SHADER_SHARED_INT64_ATOMICS,
    SHADER_INT8,    // Requires VkPhysicalDeviceShaderFloat16Int8Features

    // Bindless Texture Features (Vulkan 1.2 - Descriptor Indexing)
    DESCRIPTOR_BINDING_PARTIALLY_BOUND,         // Allow partially bound descriptor arrays
    DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING,  // Allow updating unused descriptors while others are in use
    DESCRIPTOR_BINDING_UNIFORM_BUFFER_UPDATE_AFTER_BIND,  // Allow updating uniform buffers after binding
    DESCRIPTOR_BINDING_SAMPLED_IMAGE_UPDATE_AFTER_BIND,   // Allow updating sampled images after binding
    DESCRIPTOR_BINDING_STORAGE_IMAGE_UPDATE_AFTER_BIND,   // Allow updating storage images after binding
    RUNTIME_DESCRIPTOR_ARRAY,                   // Allow variable-sized descriptor arrays

    FEATURES_NUM
};

template<DeviceFeature F>
struct DeviceFeatureTypeTrait;

// Shader Pipeline Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::GEOMETRY_SHADER> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::TESSELLATION_SHADER> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::SHADER_FLOAT64> { using Type = bool; };

// Sampling and Texture Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::SAMPLER_ANISOTROPY> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::FILL_MODE_NON_SOLID> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::TEXTURE_COMPRESSION_BC> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::TEXTURE_COMPRESSION_ETC2> { using Type = bool; };

// Drawing Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::MULTI_DRAW_INDIRECT> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::WIDE_LINES> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::LARGE_POINTS> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::MULTI_VIEWPORT> { using Type = bool; };

// Depth and Blend Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DEPTH_CLAMP> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DEPTH_BIAS_CLAMP> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DUAL_SRC_BLEND> { using Type = bool; };

// Storage and Atomic Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::VERTEX_PIPELINE_STORES_AND_ATOMICS> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::FRAGMENT_STORES_AND_ATOMICS> { using Type = bool; };

// Vulkan 1.2 Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::SHADER_BUFFER_INT64_ATOMICS> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::SHADER_SHARED_INT64_ATOMICS> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::SHADER_INT8> { using Type = bool; };

// Bindless Texture Features
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DESCRIPTOR_BINDING_PARTIALLY_BOUND> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DESCRIPTOR_BINDING_UNIFORM_BUFFER_UPDATE_AFTER_BIND> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DESCRIPTOR_BINDING_SAMPLED_IMAGE_UPDATE_AFTER_BIND> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::DESCRIPTOR_BINDING_STORAGE_IMAGE_UPDATE_AFTER_BIND> { using Type = bool; };
template<> struct DeviceFeatureTypeTrait<DeviceFeature::RUNTIME_DESCRIPTOR_ARRAY> { using Type = bool; };