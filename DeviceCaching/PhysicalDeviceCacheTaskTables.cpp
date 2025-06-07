#include "PhysicalDeviceCache.h"
#include "Rendering/Device.h"

static inline void* getRequiredVkStorage(vk::PhysicalDeviceFeatures2& vkStorage2,
    vk::StructureType sType)
{
    void* next = vkStorage2.pNext;
    while (next != nullptr)
    {
        vk::BaseOutStructure* header = static_cast<vk::BaseOutStructure*>(next);
        if (header->sType == sType)
            return next;
        next = header->pNext;
    }

    throw std::runtime_error("Requested structure type not found in feature chain");
}

const std::array<Device::FeatureSetFunc,
    static_cast<size_t>(DeviceFeature::FEATURES_NUM)> Device::featureSetTaskTable = {
           
    //// Shader Pipeline Features
    //GEOMETRY_SHADER = 0,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.geometryShader = std::any_cast<bool>(required);
    },

    //TESSELLATION_SHADER,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.tessellationShader = std::any_cast<bool>(required);
    },

    //SHADER_FLOAT64,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.shaderFloat64 = std::any_cast<bool>(required);
    },

    //// Sampling and Texture Features
    //SAMPLER_ANISOTROPY,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.samplerAnisotropy = std::any_cast<bool>(required);
    },
    //FILL_MODE_NON_SOLID,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.fillModeNonSolid = std::any_cast<bool>(required);
    },
    //TEXTURE_COMPRESSION_BC,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.textureCompressionBC = std::any_cast<bool>(required);
    },
    //TEXTURE_COMPRESSION_ETC2,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.textureCompressionETC2 = std::any_cast<bool>(required);
    },

    //// Drawing Features
    //MULTI_DRAW_INDIRECT,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.multiDrawIndirect = std::any_cast<bool>(required);
    },
    //WIDE_LINES,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.wideLines = std::any_cast<bool>(required);
    },
    //LARGE_POINTS,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.largePoints = std::any_cast<bool>(required);
    },
    //MULTI_VIEWPORT,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.multiViewport = std::any_cast<bool>(required);
    },

    //// Depth and Blend Features
    //DEPTH_CLAMP,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.depthClamp = std::any_cast<bool>(required);
    },
    //DEPTH_BIAS_CLAMP,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.depthBiasClamp = std::any_cast<bool>(required);
    },
    //DUAL_SRC_BLEND,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.dualSrcBlend = std::any_cast<bool>(required);
    },

    //// Storage and Atomic Features
    //VERTEX_PIPELINE_STORES_AND_ATOMICS,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.vertexPipelineStoresAndAtomics = std::any_cast<bool>(required);
    },
    //FRAGMENT_STORES_AND_ATOMICS,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        vkStorage.features.fragmentStoresAndAtomics = std::any_cast<bool>(required);
    },

    //// Vulkan 1.2 Features
    //SHADER_BUFFER_INT64_ATOMICS,    // Requires VkPhysicalDeviceShaderAtomicInt64Features
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceShaderAtomicInt64Features*>
        (getRequiredVkStorage(vkStorage,
        vk::StructureType::ePhysicalDeviceShaderAtomicInt64Features));
        vkStorageSpecific.shaderBufferInt64Atomics = std::any_cast<bool>(required);
    },
    //SHADER_SHARED_INT64_ATOMICS,
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceShaderAtomicInt64Features*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceShaderAtomicInt64Features));
        vkStorageSpecific.shaderSharedInt64Atomics = std::any_cast<bool>(required);
    },
    //SHADER_INT8,    // Requires VkPhysicalDeviceShaderFloat16Int8Features
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceShaderFloat16Int8Features*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceShaderFloat16Int8Features));
        vkStorageSpecific.shaderInt8 = std::any_cast<bool>(required);
    },

    //// Bindless Texture Features (Vulkan 1.2 - Descriptor Indexing)
    //DESCRIPTOR_BINDING_PARTIALLY_BOUND,         // Allow partially bound descriptor arrays
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
        vkStorageSpecific.descriptorBindingPartiallyBound = std::any_cast<bool>(required);
    },
    //DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING,  // Allow updating unused descriptors while others are in use
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
        vkStorageSpecific.descriptorBindingUpdateUnusedWhilePending = std::any_cast<bool>(required);
    },
    //DESCRIPTOR_BINDING_UNIFORM_BUFFER_UPDATE_AFTER_BIND,  // Allow updating uniform buffers after binding
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
        vkStorageSpecific.descriptorBindingUniformBufferUpdateAfterBind = std::any_cast<bool>(required);
    },
    //DESCRIPTOR_BINDING_SAMPLED_IMAGE_UPDATE_AFTER_BIND,   // Allow updating sampled images after binding
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
        vkStorageSpecific.descriptorBindingSampledImageUpdateAfterBind = std::any_cast<bool>(required);
    },
    //DESCRIPTOR_BINDING_STORAGE_IMAGE_UPDATE_AFTER_BIND,   // Allow updating storage images after binding
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
        vkStorageSpecific.descriptorBindingStorageImageUpdateAfterBind = std::any_cast<bool>(required);
    },
    //RUNTIME_DESCRIPTOR_ARRAY,                   // Allow variable-sized descriptor arrays
        [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
    {
        auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
            (getRequiredVkStorage(vkStorage,
                vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
        vkStorageSpecific.runtimeDescriptorArray = std::any_cast<bool>(required);
    },
    };

const std::array<PhysicalDeviceCache::TaskTableSignature,
    static_cast<size_t>(DeviceFeature::FEATURES_NUM)> PhysicalDeviceCache::featureChecks = {
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
        [](const std::any& required, const std::any& available)
    { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
};


const std::array<PhysicalDeviceCache::TaskTableSignature,
    static_cast<size_t>(DeviceProperty::PROPERTIES_NUM)> PhysicalDeviceCache::propertyChecks = {

        [](const std::any& required, const std::any& available) //API_VERSION
    { return std::any_cast<const uint32_t&>(required) == 
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //DRIVER_VERSION
    { return std::any_cast<const uint32_t&>(required) ==
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //VENDOR_ID
    { return std::any_cast<const uint32_t&>(required) ==
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //DEVICE_ID
    { return std::any_cast<const uint32_t&>(required) ==
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //DEVICE_TYPE
    { return std::any_cast<const PhysicalDeviceType&>(required) ==
    std::any_cast<const PhysicalDeviceType&>(available); },
        [](const std::any& required, const std::any& available) //DEVICE_NAME
    { return std::any_cast<const std::string&>(required) ==
    std::any_cast<const std::string&>(available); },

        [](const std::any& required, const std::any& available) //MAX_IMAGE_DIMENSION_2D
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_IMAGE_DIMENSION_3D
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_IMAGE_DIMENSION_CUBE
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_IMAGE_ARRAY_LAYERS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },

        [](const std::any& required, const std::any& available) //MAX_BOUND_DESCRIPTOR_SETS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_PER_STAGE_DESCRIPTOR_SAMPLERS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_PER_STAGE_DESCRIPTOR_UNIFORM_BUFFERS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_PER_STAGE_DESCRIPTOR_STORAGE_BUFFERS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },

        [](const std::any& required, const std::any& available) //MAX_STORAGE_BUFFER_RANGE
    { return std::any_cast<const uint64_t&>(required) <=
    std::any_cast<const uint64_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_UNIFORM_BUFFER_RANGE
    { return std::any_cast<const uint64_t&>(required) <=
    std::any_cast<const uint64_t&>(available); },
        [](const std::any& required, const std::any& available) //MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT
    { return std::any_cast<const uint64_t&>(required) <=
    std::any_cast<const uint64_t&>(available); },
        [](const std::any& required, const std::any& available) //MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT
    { return std::any_cast<const uint64_t&>(required) <=
    std::any_cast<const uint64_t&>(available); },

        [](const std::any& required, const std::any& available) //MAX_VIEWPORT_DIMENSIONS
    {   const auto& requiredDims = std::any_cast<const std::array<uint32_t, 2>&>(required);        
        const auto& availableDims = std::any_cast<const std::array<uint32_t, 2>&>(available);
        return requiredDims[0] <= availableDims[0] && requiredDims[1] <= availableDims[1]; },
        [](const std::any& required, const std::any& available) //MAX_VIEWPORTS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //VIEWPORT_BOUNDS_RANGE
    {   const auto& requiredRange = std::any_cast<const std::array<float, 2>&>(required);
        const auto& availableRange = std::any_cast<const std::array<float, 2>&>(available);
        return requiredRange[0] <= availableRange[0] && requiredRange[1] <= availableRange[1]; },
        [](const std::any& required, const std::any& available) //VIEWPORT_SUB_PIXEL_BITS
    { return std::any_cast<const uint32_t&>(required) ==
    std::any_cast<const uint32_t&>(available); },

        [](const std::any& required, const std::any& available) //MAX_FRAMEBUFFER_LAYERS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_SAMPLE_MASK_WORDS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_COLOR_ATTACHMENTS
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },

        [](const std::any& required, const std::any& available) //MAX_MEMORY_ALLOCATION_COUNT
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //MAX_SAMPLER_ALLOCATION_COUNT
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },
        [](const std::any& required, const std::any& available) //BUFFER_IMAGE_GRANULARITY
    { return std::any_cast<const uint64_t&>(required) <=
    std::any_cast<const uint64_t&>(available); },
        [](const std::any& required, const std::any& available) //SPARSE_ADDRESS_SPACE_SIZE
    { return std::any_cast<const uint64_t&>(required) <=
    std::any_cast<const uint64_t&>(available); },

        [](const std::any& required, const std::any& available) //MAX_SAMPLER_ANISOTROPY
    { return std::any_cast<const float&>(required) <=
    std::any_cast<const float&>(available); },
        [](const std::any& required, const std::any& available) //MAX_SAMPLER_LOD_BIAS
    { return std::any_cast<const float&>(required) <=
    std::any_cast<const float&>(available); },
        [](const std::any& required, const std::any& available) //TIMESTAMP_PERIOD
    { return std::any_cast<const float&>(required) ==
    std::any_cast<const float&>(available); },
};

const std::array<PhysicalDeviceCache::TaskTableSignature,
    static_cast<size_t>(QueueProperty::PROPERTIES_NUM)> PhysicalDeviceCache::queuePropertyChecks = {
    [](const std::any& required, const std::any& available)
    { return (std::any_cast<QueueFlags::Flags>(required) &
            std::any_cast<QueueFlags::Flags>(available)) != 0; },

    [](const std::any& required, const std::any& available)
    { return std::any_cast<const uint32_t&>(required) <=
    std::any_cast<const uint32_t&>(available); },

    [](const std::any& required, const std::any& available)
    { return std::any_cast<const uint32_t&>(required) ==
    std::any_cast<const uint32_t&>(available); },

    [](const std::any& required, const std::any& available)
    {
        const std::array<uint32_t, 3>& requiredGranularity =
            std::any_cast<const std::array<uint32_t, 3>&>(required);
        const std::array<uint32_t, 3>& availableGranularity =
            std::any_cast<const std::array<uint32_t, 3>&>(available);
        return requiredGranularity[0] == availableGranularity[0] &&
            requiredGranularity[1] == availableGranularity[1] &&
            requiredGranularity[2] == availableGranularity[2]; },
};