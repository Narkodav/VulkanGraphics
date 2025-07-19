#pragma once
#include "../Common.h"
#include "../PlatformManagement/Window.h"
#include "Context.h"
#include "../DeviceCaching/PhysicalDeviceCache.h"

namespace Graphics {

	class Device
	{
	public:

		using FeatureSetFunc = std::function<void(vk::PhysicalDeviceFeatures2&, const std::any&)>;

	private:
		const PhysicalDevice* m_physicalDevice = nullptr;
		vk::Device m_device = nullptr;

		bool m_initialized = false;

		static const std::array<FeatureSetFunc,
			static_cast<size_t>(DeviceFeature::FeaturesNum)> featureSetTaskTable;

	public:

		Device() : m_physicalDevice(nullptr), m_device(nullptr) {};

		template <typename Container>
			requires std::ranges::sized_range<Container>&&
		std::convertible_to<std::ranges::range_value_t<Container>, uint32_t>
			Device(const Context& instance,
				const PhysicalDevice& physicalDevice,
				const DeviceRequirements& requirements,
				Container&& queueFamilyIndices,
				Container&& queueCounts,
				const std::vector<std::vector<float>>& queuePriorities) :
			m_physicalDevice(&physicalDevice) {

			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
			queueCreateInfos.reserve(queueFamilyIndices.size());

			float queuePriority = 1.0f;
			for (size_t i = 0; i < queueFamilyIndices.size(); ++i) {
				vk::DeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
				queueCreateInfo.queueFamilyIndex = queueFamilyIndices[i];
				queueCreateInfo.queueCount = queueCounts[i];
				queueCreateInfo.pQueuePriorities = queuePriorities[i].data();
				queueCreateInfos.push_back(queueCreateInfo);
			}

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

			for (const auto& feature : requirements.features)
				featureSetTaskTable[static_cast<size_t>(feature.first)](features2, feature.second);

			std::vector<const char*> extensionsCStr;
			extensionsCStr.reserve(requirements.extensions.size());
			for (const auto& extension : requirements.extensions) {
				extensionsCStr.push_back(extension.c_str());
			}

			vk::DeviceCreateInfo createInfo{};
			createInfo.sType = vk::StructureType::eDeviceCreateInfo;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &features2.features;

			createInfo.enabledExtensionCount = static_cast<uint32_t>(requirements.extensions.size());
			createInfo.ppEnabledExtensionNames = extensionsCStr.data();

			createInfo.pNext = features2.pNext;

#ifdef _DEBUG
			createInfo.enabledLayerCount = static_cast<uint32_t>(Context::validationLayers.size());
			createInfo.ppEnabledLayerNames = Context::validationLayers.data();
#else
			createInfo.enabledLayerCount = 0;
#endif

			try {
				m_device = physicalDevice.getHandle().createDevice(createInfo, nullptr, instance.getDispatchLoader());
				instance.updateDispatchLoader(m_device);
			}
			catch (const vk::SystemError& e) {
				throw std::runtime_error("failed to create logical device! " + std::string(e.what()));
			}

			m_initialized = true;
		};

		Device(Device&& other) noexcept {

			m_physicalDevice = std::exchange(other.m_physicalDevice, nullptr);
			m_device = std::exchange(other.m_device, nullptr);
			m_initialized = std::exchange(other.m_initialized, false);

		};

		//moving to an initialized device is undefined behavior, destroy before moving
		Device& operator=(Device&& other) noexcept
		{
			if (this == &other)
				return *this;

			assert(!m_initialized && "Device::operator=() - Device already initialized");

			m_physicalDevice = std::exchange(other.m_physicalDevice, nullptr);
			m_device = std::exchange(other.m_device, nullptr);
			m_initialized = std::exchange(other.m_initialized, false);

			return *this;
		};

		Device(const Device& other) noexcept = delete;
		Device& operator=(const Device& other) noexcept = delete;

		~Device() { assert(!m_initialized && "Device was not destroyed!"); };

		void destroy(const Context& instance) {
			if (!m_initialized)
				return;
			if (m_device) {
				m_device.destroy(nullptr, instance.getDispatchLoader());
			}
#ifdef _DEBUG
			std::cout << "Destroyed Device" << std::endl;
#endif
			m_initialized = false;
		}

		// Basic Device Access
		const vk::Device& getDevice() const { return m_device; }
		const PhysicalDevice& getPhysicalDevice() const { return *m_physicalDevice; }

		void waitIdle(const Context& instance) const {
			m_device.waitIdle(instance.getDispatchLoader());
		};

		vk::DeviceMemory allocateMemory(const Context& instance,
			size_t requiredCapacity, uint32_t memoryType) const;

		uint32_t findMemoryType(const Context& instance,
			uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

		vk::Format findSupportedFormat(const Context& instance,
			const std::vector<vk::Format>& candidates,
			vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
	};

}