#include "PhysicalDevice.h"

namespace Graphics {

    //adding support will expand this list
    void PhysicalDevice::enumerateFeatures(const Context& instance)
    {
        vk::PhysicalDeviceFeatures2 features2;

        // Vulkan 1.1 features
        vk::PhysicalDevice16BitStorageFeatures bit16StorageFeatures;
        vk::PhysicalDeviceMultiviewFeatures multiviewFeatures;
        vk::PhysicalDeviceVariablePointersFeatures variablePointersFeatures;
        vk::PhysicalDeviceProtectedMemoryFeatures protectedMemoryFeatures;
        vk::PhysicalDeviceSamplerYcbcrConversionFeatures samplerYcbcrFeatures;
        vk::PhysicalDeviceShaderDrawParametersFeatures shaderDrawFeatures;

        // Vulkan 1.2 features
        vk::PhysicalDevice8BitStorageFeatures bit8StorageFeatures;
        vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures;
        vk::PhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures;
        vk::PhysicalDeviceHostQueryResetFeatures hostQueryResetFeatures;
        vk::PhysicalDeviceImagelessFramebufferFeatures imagelessFramebufferFeatures;
        vk::PhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayoutFeatures;
        vk::PhysicalDeviceSeparateDepthStencilLayoutsFeatures separateDepthStencilFeatures;
        vk::PhysicalDeviceShaderAtomicInt64Features shaderAtomicInt64Features;
        vk::PhysicalDeviceShaderFloat16Int8Features shaderFloat16Int8Features;
        vk::PhysicalDeviceShaderSubgroupExtendedTypesFeatures shaderSubgroupFeatures;
        vk::PhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures;
        vk::PhysicalDeviceUniformBufferStandardLayoutFeatures uniformBufferFeatures;
        vk::PhysicalDeviceVulkanMemoryModelFeatures vulkanMemoryFeatures;

        // Vulkan 1.3 features
        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
        vk::PhysicalDeviceImageRobustnessFeatures imageRobustnessFeatures;
        vk::PhysicalDeviceInlineUniformBlockFeatures inlineUniformBlockFeatures;
        vk::PhysicalDeviceMaintenance4Features maintenance4Features;
        vk::PhysicalDevicePipelineCreationCacheControlFeatures pipelineCacheFeatures;
        vk::PhysicalDevicePrivateDataFeatures privateDataFeatures;
        vk::PhysicalDeviceShaderDemoteToHelperInvocationFeatures shaderDemoteFeatures;
        vk::PhysicalDeviceShaderIntegerDotProductFeatures shaderIntegerDotFeatures;
        vk::PhysicalDeviceShaderTerminateInvocationFeatures shaderTerminateFeatures;
        vk::PhysicalDeviceSynchronization2Features sync2Features;
        vk::PhysicalDeviceTextureCompressionASTCHDRFeatures astcHdrFeatures;
        //vk::PhysicalDeviceVertexInputDynamicStateFeatures vertexInputDynamicFeatures;
        vk::PhysicalDeviceZeroInitializeWorkgroupMemoryFeatures zeroInitializeFeatures;

        // Chain all features
        features2.pNext = &bit16StorageFeatures;
        bit16StorageFeatures.pNext = &multiviewFeatures;
        multiviewFeatures.pNext = &variablePointersFeatures;
        variablePointersFeatures.pNext = &protectedMemoryFeatures;
        protectedMemoryFeatures.pNext = &samplerYcbcrFeatures;
        samplerYcbcrFeatures.pNext = &shaderDrawFeatures;

        shaderDrawFeatures.pNext = &bit8StorageFeatures;
        bit8StorageFeatures.pNext = &bufferDeviceAddressFeatures;
        bufferDeviceAddressFeatures.pNext = &descriptorIndexingFeatures;
        descriptorIndexingFeatures.pNext = &hostQueryResetFeatures;
        hostQueryResetFeatures.pNext = &imagelessFramebufferFeatures;
        imagelessFramebufferFeatures.pNext = &scalarBlockLayoutFeatures;
        scalarBlockLayoutFeatures.pNext = &separateDepthStencilFeatures;
        separateDepthStencilFeatures.pNext = &shaderAtomicInt64Features;
        shaderAtomicInt64Features.pNext = &shaderFloat16Int8Features;
        shaderFloat16Int8Features.pNext = &shaderSubgroupFeatures;
        shaderSubgroupFeatures.pNext = &timelineSemaphoreFeatures;
        timelineSemaphoreFeatures.pNext = &uniformBufferFeatures;
        uniformBufferFeatures.pNext = &vulkanMemoryFeatures;

        vulkanMemoryFeatures.pNext = &dynamicRenderingFeatures;
        dynamicRenderingFeatures.pNext = &imageRobustnessFeatures;
        imageRobustnessFeatures.pNext = &inlineUniformBlockFeatures;
        inlineUniformBlockFeatures.pNext = &maintenance4Features;
        maintenance4Features.pNext = &pipelineCacheFeatures;
        pipelineCacheFeatures.pNext = &privateDataFeatures;
        privateDataFeatures.pNext = &shaderDemoteFeatures;
        shaderDemoteFeatures.pNext = &shaderIntegerDotFeatures;
        shaderIntegerDotFeatures.pNext = &shaderTerminateFeatures;
        shaderTerminateFeatures.pNext = &sync2Features;
        sync2Features.pNext = &astcHdrFeatures;
        astcHdrFeatures.pNext = &zeroInitializeFeatures;
        //vertexInputDynamicFeatures.pNext = &zeroInitializeFeatures;
        zeroInitializeFeatures.pNext = nullptr;  // End of chain

        // Query all features
        m_physicalDevice.getFeatures2(&features2, instance.getDispatchLoader());

        const auto& baseFeatures = features2.features;

        // Store Vulkan 1.0 features (from baseFeatures)
        {
            // Geometry and Tessellation
            storeFeature(DeviceFeature::GeometryShader,
                static_cast<bool>(baseFeatures.geometryShader));
            storeFeature(DeviceFeature::TessellationShader,
                static_cast<bool>(baseFeatures.tessellationShader));

            // Sampling and Fill modes
            storeFeature(DeviceFeature::SamplerAnisotropy,
                static_cast<bool>(baseFeatures.samplerAnisotropy));
            storeFeature(DeviceFeature::FillModeNonSolid,
                static_cast<bool>(baseFeatures.fillModeNonSolid));
            storeFeature(DeviceFeature::TextureCompressionBc,
                static_cast<bool>(baseFeatures.textureCompressionBC));
            storeFeature(DeviceFeature::TextureCompressionEtc2,
                static_cast<bool>(baseFeatures.textureCompressionETC2));

            // Base shader features
            storeFeature(DeviceFeature::ShaderFloat64,
                static_cast<bool>(baseFeatures.shaderFloat64));

            // Drawing features
            storeFeature(DeviceFeature::MultiDrawIndirect,
                static_cast<bool>(baseFeatures.multiDrawIndirect));
            storeFeature(DeviceFeature::WideLines,
                static_cast<bool>(baseFeatures.wideLines));
            storeFeature(DeviceFeature::LargePoints,
                static_cast<bool>(baseFeatures.largePoints));
            storeFeature(DeviceFeature::MultiViewport,
                static_cast<bool>(baseFeatures.multiViewport));

            // Depth and blending
            storeFeature(DeviceFeature::DepthClamp,
                static_cast<bool>(baseFeatures.depthClamp));
            storeFeature(DeviceFeature::DepthBiasClamp,
                static_cast<bool>(baseFeatures.depthBiasClamp));
            storeFeature(DeviceFeature::DualSrcBlend,
                static_cast<bool>(baseFeatures.dualSrcBlend));

            // Storage and atomics
            storeFeature(DeviceFeature::VertexPipelineStoresAndAtomics,
                static_cast<bool>(baseFeatures.vertexPipelineStoresAndAtomics));
            storeFeature(DeviceFeature::FragmentStoresAndAtomics,
                static_cast<bool>(baseFeatures.fragmentStoresAndAtomics));
        }

        // Store Vulkan 1.2 features
        {
            // Advanced shader features
            storeFeature(DeviceFeature::ShaderBufferInt64Atomics,
                static_cast<bool>(shaderAtomicInt64Features.shaderBufferInt64Atomics));
            storeFeature(DeviceFeature::ShaderSharedInt64Atomics,
                static_cast<bool>(shaderAtomicInt64Features.shaderSharedInt64Atomics));
            storeFeature(DeviceFeature::ShaderInt8,
                static_cast<bool>(shaderFloat16Int8Features.shaderInt8));
        }

        // Store Vulkan 1.2 indexing features
        {
            // Advanced shader features
            storeFeature(DeviceFeature::DescriptorBindingPartiallyBound,
                static_cast<bool>(descriptorIndexingFeatures.descriptorBindingPartiallyBound));
            storeFeature(DeviceFeature::DescriptorBindingUpdateUnusedWhilePending,
                static_cast<bool>(descriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending));
            storeFeature(DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
                static_cast<bool>(descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind));
            storeFeature(DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
                static_cast<bool>(descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind));
            storeFeature(DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
                static_cast<bool>(descriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind));
            storeFeature(DeviceFeature::DescriptorBindingVariableDescriptorCount,
                static_cast<bool>(descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount));
            storeFeature(DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
                static_cast<bool>(descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing));
            storeFeature(DeviceFeature::RuntimeDescriptorArray,
                static_cast<bool>(descriptorIndexingFeatures.runtimeDescriptorArray));
        }
    }

    void PhysicalDevice::enumerateProperties(const Context& instance)
    {
        vk::PhysicalDeviceProperties2 properties2;

        // Vulkan 1.1 properties
        vk::PhysicalDeviceSubgroupProperties subgroupProperties;
        vk::PhysicalDevicePointClippingProperties pointClippingProperties;
        vk::PhysicalDeviceMultiviewProperties multiviewProperties;
        vk::PhysicalDeviceProtectedMemoryProperties protectedMemoryProperties;
        vk::PhysicalDeviceMaintenance3Properties maintenance3Properties;

        // Vulkan 1.2 properties
        vk::PhysicalDeviceDriverProperties driverProperties;
        vk::PhysicalDeviceFloatControlsProperties floatControlsProperties;
        vk::PhysicalDeviceDescriptorIndexingProperties descriptorIndexingProperties;
        vk::PhysicalDeviceDepthStencilResolveProperties depthStencilProperties;
        vk::PhysicalDeviceSamplerFilterMinmaxProperties samplerMinmaxProperties;
        vk::PhysicalDeviceTimelineSemaphoreProperties timelineSemaphoreProperties;

        // Vulkan 1.3 properties
        vk::PhysicalDeviceInlineUniformBlockProperties inlineUniformBlockProperties;
        vk::PhysicalDeviceMaintenance4Properties maintenance4Properties;

        // Chain them together
        properties2.pNext = &subgroupProperties;

        // Chain Vulkan 1.1 properties
        subgroupProperties.pNext = &pointClippingProperties;
        pointClippingProperties.pNext = &multiviewProperties;
        multiviewProperties.pNext = &protectedMemoryProperties;
        protectedMemoryProperties.pNext = &maintenance3Properties;

        // Chain Vulkan 1.2 properties
        maintenance3Properties.pNext = &driverProperties;
        driverProperties.pNext = &floatControlsProperties;
        floatControlsProperties.pNext = &descriptorIndexingProperties;
        descriptorIndexingProperties.pNext = &depthStencilProperties;
        depthStencilProperties.pNext = &samplerMinmaxProperties;
        samplerMinmaxProperties.pNext = &timelineSemaphoreProperties;

        // Chain Vulkan 1.3 properties
        timelineSemaphoreProperties.pNext = &inlineUniformBlockProperties;
        inlineUniformBlockProperties.pNext = &maintenance4Properties;
        maintenance4Properties.pNext = nullptr;  // End of chain

        // Query all properties
        m_physicalDevice.getProperties2(&properties2, instance.getDispatchLoader());

        // Store properties from base properties
        const auto& baseProps = properties2.properties;
        const auto& limits = baseProps.limits;

        //basic
        storeProperty(DeviceProperty::ApiVersion, baseProps.apiVersion);
        storeProperty(DeviceProperty::DriverVersion, baseProps.driverVersion);
        storeProperty(DeviceProperty::VendorId, baseProps.vendorID);
        storeProperty(DeviceProperty::PhysicalDeviceId, baseProps.deviceID);
        storeProperty(DeviceProperty::PhysicalDeviceType,
            static_cast<PhysicalDeviceType>(baseProps.deviceType));
        storeProperty(DeviceProperty::PhysicalDeviceName,
            std::string(static_cast<const char*>(baseProps.deviceName)));

        // Core Limits
        storeProperty(DeviceProperty::MaxImageDimension2d,
            limits.maxImageDimension2D);
        storeProperty(DeviceProperty::MaxImageDimension3d,
            limits.maxImageDimension3D);
        storeProperty(DeviceProperty::MaxImageDimensionCube,
            limits.maxImageDimensionCube);
        storeProperty(DeviceProperty::MaxImageArrayLayers,
            limits.maxImageArrayLayers);

        // Descriptor Limits
        storeProperty(DeviceProperty::MaxBoundDescriptorSets,
            limits.maxBoundDescriptorSets);
        storeProperty(DeviceProperty::MaxPerStageDescriptorSamplers,
            limits.maxPerStageDescriptorSamplers);
        storeProperty(DeviceProperty::MaxPerStageDescriptorUniformBuffers,
            limits.maxPerStageDescriptorUniformBuffers);
        storeProperty(DeviceProperty::MaxPerStageDescriptorStorageBuffers,
            limits.maxPerStageDescriptorStorageBuffers);

        // Buffer Limits
        storeProperty(DeviceProperty::MaxStorageBufferRange,
            limits.maxStorageBufferRange);
        storeProperty(DeviceProperty::MaxUniformBufferRange,
            limits.maxUniformBufferRange);
        storeProperty(DeviceProperty::MinUniformBufferOffsetAlignment,
            static_cast<uint64_t>(limits.minUniformBufferOffsetAlignment));
        storeProperty(DeviceProperty::MinStorageBufferOffsetAlignment,
            static_cast<uint64_t>(limits.minStorageBufferOffsetAlignment));

        // Viewport/Scissor
        storeProperty(DeviceProperty::MaxViewportDimensions,
            std::array<uint32_t, 2>{limits.maxViewportDimensions[0],
            limits.maxViewportDimensions[1]});
        storeProperty(DeviceProperty::MaxViewports,
            limits.maxViewports);
        storeProperty(DeviceProperty::ViewportBoundsRange,
            std::array<float, 2>{limits.viewportBoundsRange[0],
            limits.viewportBoundsRange[1]});
        storeProperty(DeviceProperty::ViewportSubPixelBits,
            limits.viewportSubPixelBits);

        // Sample Limits
        storeProperty(DeviceProperty::MaxFramebufferLayers,
            limits.maxFramebufferLayers);
        storeProperty(DeviceProperty::MaxSampleMaskWords,
            limits.maxSampleMaskWords);
        storeProperty(DeviceProperty::MaxColorAttachments,
            limits.maxColorAttachments);

        // Memory Limits
        storeProperty(DeviceProperty::MaxMemoryAllocationCount,
            limits.maxMemoryAllocationCount);
        storeProperty(DeviceProperty::MaxSamplerAllocationCount,
            limits.maxSamplerAllocationCount);
        storeProperty(DeviceProperty::BufferImageGranularity,
            static_cast<uint64_t>(limits.bufferImageGranularity));
        storeProperty(DeviceProperty::SparseAddressSpaceSize,
            static_cast<uint64_t>(limits.sparseAddressSpaceSize));

        // Quality Settings
        storeProperty(DeviceProperty::MaxSamplerAnisotropy,
            limits.maxSamplerAnisotropy);
        storeProperty(DeviceProperty::MaxSamplerLodBias,
            limits.maxSamplerLodBias);
        storeProperty(DeviceProperty::TimestampPeriod,
            limits.timestampPeriod);
    }

    void PhysicalDevice::enumerateExtensions(const Context& instance)
    {
        // Get extension count
        uint32_t extensionCount;
        m_physicalDevice.enumerateDeviceExtensionProperties(nullptr,
            &extensionCount, nullptr, instance.getDispatchLoader());

        // Get extensions
        std::vector<vk::ExtensionProperties> extensions(extensionCount);
        m_physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount,
            extensions.data(), instance.getDispatchLoader());

        m_availableExtensions.clear();
        for (const auto& ext : extensions) {
            m_availableExtensions.insert(ext.extensionName);
        }
    }

    void PhysicalDevice::enumerateQueueFamilies(const Context& instance)
    {
        // Get queue family count
        uint32_t queueFamilyCount;
        m_physicalDevice.getQueueFamilyProperties2(&queueFamilyCount, nullptr,
            instance.getDispatchLoader());

        // Get queue families
        std::vector<vk::QueueFamilyProperties2> properties(queueFamilyCount);

        std::vector<vk::QueueFamilyGlobalPriorityPropertiesKHR> globalPriorityProps(queueFamilyCount);
        std::vector<vk::QueueFamilyVideoPropertiesKHR> videoProps(queueFamilyCount);
        std::vector<vk::QueueFamilyQueryResultStatusPropertiesKHR> queryResultProps(queueFamilyCount);
        std::vector<vk::QueueFamilyCheckpointPropertiesNV> checkpointProps(queueFamilyCount);
        std::vector<vk::QueueFamilyCheckpointProperties2NV> checkpoint2Props(queueFamilyCount);

        // Chain the structures for each queue family
        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            // Start the chain with the base properties
            properties[i].pNext = &globalPriorityProps[i];

            // Chain optional properties (if extensions are available)
            globalPriorityProps[i].pNext = &videoProps[i];
            videoProps[i].pNext = &queryResultProps[i];
            queryResultProps[i].pNext = &checkpointProps[i];
            checkpointProps[i].pNext = &checkpoint2Props[i];
            checkpoint2Props[i].pNext = nullptr;  // End of chain
        }

        m_physicalDevice.getQueueFamilyProperties2(&queueFamilyCount,
            properties.data(), instance.getDispatchLoader());

        m_queueFamilies.clear();
        for (uint32_t i = 0; i < queueFamilyCount; ++i) {
            m_queueFamilies.push_back(QueueFamily{ properties[i], i });
        }
    }

}