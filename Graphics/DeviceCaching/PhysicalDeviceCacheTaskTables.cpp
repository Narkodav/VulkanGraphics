#include "PhysicalDeviceCache.h"
#include "../Rendering/Device.h"

namespace Graphics {

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
        static_cast<size_t>(DeviceFeature::FeaturesNum)> Device::featureSetTaskTable = {

        //// Shader Pipeline Features
        //GeometryShader = 0,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.geometryShader = std::any_cast<bool>(required);
        },

        //TessellationShader,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.tessellationShader = std::any_cast<bool>(required);
        },

        //ShaderFloat64,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.shaderFloat64 = std::any_cast<bool>(required);
        },

        //// Sampling and Texture Features
        //SamplerAnisotropy,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.samplerAnisotropy = std::any_cast<bool>(required);
        },
        //FillModeNonSolid,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.fillModeNonSolid = std::any_cast<bool>(required);
        },
        //TextureCompressionBc,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.textureCompressionBC = std::any_cast<bool>(required);
        },
        //TextureCompressionEtc2,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.textureCompressionETC2 = std::any_cast<bool>(required);
        },

        //// Drawing Features
        //MultiDrawIndirect,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.multiDrawIndirect = std::any_cast<bool>(required);
        },
        //WideLines,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.wideLines = std::any_cast<bool>(required);
        },
        //LargePoints,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.largePoints = std::any_cast<bool>(required);
        },
        //MultiViewport,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.multiViewport = std::any_cast<bool>(required);
        },

        //// Depth and Blend Features
        //DepthClamp,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.depthClamp = std::any_cast<bool>(required);
        },
        //DepthBiasClamp,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.depthBiasClamp = std::any_cast<bool>(required);
        },
        //DualSrcBlend,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.dualSrcBlend = std::any_cast<bool>(required);
        },

        //// Storage and Atomic Features
        //VertexPipelineStoresAndAtomics,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.vertexPipelineStoresAndAtomics = std::any_cast<bool>(required);
        },
        //FragmentStoresAndAtomics,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            vkStorage.features.fragmentStoresAndAtomics = std::any_cast<bool>(required);
        },

        //// Vulkan 1.2 Features
        //ShaderBufferInt64Atomics,    // Requires VkPhysicalDeviceShaderAtomicInt64Features
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceShaderAtomicInt64Features*>
            (getRequiredVkStorage(vkStorage,
            vk::StructureType::ePhysicalDeviceShaderAtomicInt64Features));
            vkStorageSpecific.shaderBufferInt64Atomics = std::any_cast<bool>(required);
        },
        //ShaderSharedInt64Atomics,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceShaderAtomicInt64Features*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceShaderAtomicInt64Features));
            vkStorageSpecific.shaderSharedInt64Atomics = std::any_cast<bool>(required);
        },
        //ShaderInt8,    // Requires VkPhysicalDeviceShaderFloat16Int8Features
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceShaderFloat16Int8Features*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceShaderFloat16Int8Features));
            vkStorageSpecific.shaderInt8 = std::any_cast<bool>(required);
        },

        //// Bindless Texture Features (Vulkan 1.2 - Descriptor Indexing)
        //DescriptorBindingPartiallyBound,         // Allow partially bound descriptor arrays
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.descriptorBindingPartiallyBound = std::any_cast<bool>(required);
        },
        //DescriptorBindingUpdateUnusedWhilePending,  // Allow updating unused descriptors while others are in use
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.descriptorBindingUpdateUnusedWhilePending = std::any_cast<bool>(required);
        },
        //DescriptorBindingUniformBufferUpdateAfterBind,  // Allow updating uniform buffers after binding
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.descriptorBindingUniformBufferUpdateAfterBind = std::any_cast<bool>(required);
        },
        //DescriptorBindingSampledImageUpdateAfterBind,   // Allow updating sampled images after binding
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.descriptorBindingSampledImageUpdateAfterBind = std::any_cast<bool>(required);
        },
        //DescriptorBindingStorageImageUpdateAfterBind,   // Allow updating storage images after binding
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.descriptorBindingStorageImageUpdateAfterBind = std::any_cast<bool>(required);
        },
        //DescriptorBindingVariableDescriptorCount,
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.descriptorBindingVariableDescriptorCount = std::any_cast<bool>(required);
        },

        //ShaderSampledImageArrayNonUniformIndexing
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.shaderSampledImageArrayNonUniformIndexing = std::any_cast<bool>(required);
        },

        //RuntimeDescriptorArray,                   // Allow variable-sized descriptor arrays
            [](vk::PhysicalDeviceFeatures2& vkStorage, const std::any& required)
        {
            auto& vkStorageSpecific = *static_cast<vk::PhysicalDeviceDescriptorIndexingFeatures*>
                (getRequiredVkStorage(vkStorage,
                    vk::StructureType::ePhysicalDeviceDescriptorIndexingFeatures));
            vkStorageSpecific.runtimeDescriptorArray = std::any_cast<bool>(required);
        },
    };

    const std::array<PhysicalDeviceCache::TaskTableSignature,
        static_cast<size_t>(DeviceFeature::FeaturesNum)> PhysicalDeviceCache::featureChecks = {
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
            [](const std::any& required, const std::any& available)
        { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
            [](const std::any& required, const std::any& available)
        { return std::any_cast<bool>(required) == std::any_cast<bool>(available); },
    };


    const std::array<PhysicalDeviceCache::TaskTableSignature,
        static_cast<size_t>(DeviceProperty::PropertiesNum)> PhysicalDeviceCache::propertyChecks = {

            [](const std::any& required, const std::any& available) //ApiVersion
        { return std::any_cast<const uint32_t&>(required) ==
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //DriverVersion
        { return std::any_cast<const uint32_t&>(required) ==
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //VendorId
        { return std::any_cast<const uint32_t&>(required) ==
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //PhysicalDeviceId
        { return std::any_cast<const uint32_t&>(required) ==
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //PhysicalDeviceType
        { return std::any_cast<const PhysicalDeviceType&>(required) ==
        std::any_cast<const PhysicalDeviceType&>(available); },
            [](const std::any& required, const std::any& available) //PhysicalDeviceName
        { return std::any_cast<const std::string&>(required) ==
        std::any_cast<const std::string&>(available); },

            [](const std::any& required, const std::any& available) //MaxImageDimension2d
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxImageDimension3d
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxImageDimensionCube
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxImageArrayLayers
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },

            [](const std::any& required, const std::any& available) //MaxBoundDescriptorSets
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxPerStageDescriptorSamplers
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxPerStageDescriptorUniformBuffers
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxPerStageDescriptorStorageBuffers
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },

            [](const std::any& required, const std::any& available) //MaxStorageBufferRange
        { return std::any_cast<const uint64_t&>(required) <=
        std::any_cast<const uint64_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxUniformBufferRange
        { return std::any_cast<const uint64_t&>(required) <=
        std::any_cast<const uint64_t&>(available); },
            [](const std::any& required, const std::any& available) //MinUniformBufferOffsetAlignment
        { return std::any_cast<const uint64_t&>(required) <=
        std::any_cast<const uint64_t&>(available); },
            [](const std::any& required, const std::any& available) //MinStorageBufferOffsetAlignment
        { return std::any_cast<const uint64_t&>(required) <=
        std::any_cast<const uint64_t&>(available); },

            [](const std::any& required, const std::any& available) //MaxViewportDimensions
        {   const auto& requiredDims = std::any_cast<const std::array<uint32_t, 2>&>(required);
            const auto& availableDims = std::any_cast<const std::array<uint32_t, 2>&>(available);
            return requiredDims[0] <= availableDims[0] && requiredDims[1] <= availableDims[1]; },
            [](const std::any& required, const std::any& available) //MaxViewports
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //ViewportBoundsRange
        {   const auto& requiredRange = std::any_cast<const std::array<float, 2>&>(required);
            const auto& availableRange = std::any_cast<const std::array<float, 2>&>(available);
            return requiredRange[0] <= availableRange[0] && requiredRange[1] <= availableRange[1]; },
            [](const std::any& required, const std::any& available) //ViewportSubPixelBits
        { return std::any_cast<const uint32_t&>(required) ==
        std::any_cast<const uint32_t&>(available); },

            [](const std::any& required, const std::any& available) //MaxFramebufferLayers
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxSampleMaskWords
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxColorAttachments
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },

            [](const std::any& required, const std::any& available) //MaxMemoryAllocationCount
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //MaxSamplerAllocationCount
        { return std::any_cast<const uint32_t&>(required) <=
        std::any_cast<const uint32_t&>(available); },
            [](const std::any& required, const std::any& available) //BufferImageGranularity
        { return std::any_cast<const uint64_t&>(required) <=
        std::any_cast<const uint64_t&>(available); },
            [](const std::any& required, const std::any& available) //SparseAddressSpaceSize
        { return std::any_cast<const uint64_t&>(required) <=
        std::any_cast<const uint64_t&>(available); },

            [](const std::any& required, const std::any& available) //MaxSamplerAnisotropy
        { return std::any_cast<const float&>(required) <=
        std::any_cast<const float&>(available); },
            [](const std::any& required, const std::any& available) //MaxSamplerLodBias
        { return std::any_cast<const float&>(required) <=
        std::any_cast<const float&>(available); },
            [](const std::any& required, const std::any& available) //TimestampPeriod
        { return std::any_cast<const float&>(required) ==
        std::any_cast<const float&>(available); },
    };

    const std::array<PhysicalDeviceCache::TaskTableSignature,
        static_cast<size_t>(QueueProperty::PropertiesNum)> PhysicalDeviceCache::queuePropertyChecks = {
        [](const std::any& required, const std::any& available)
        {
            return std::any_cast<QueueFlags::Flags>(available)
            .hasFlags(std::any_cast<QueueFlags::Flags>(required)); },

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

}