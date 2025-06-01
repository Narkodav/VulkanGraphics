#include "EngineDevice.h"



bool EngineDevice::evaluateDevice(vk::PhysicalDevice device, const EngineContext& instance, const Window& window,
	BasicRenderingQueueFamilyIndices& basicIndices, SwapChainSupportDetails& swapChainSupportDetails) {
	auto deviceFeatures = device.getFeatures();
	auto deviceProperties = device.getProperties();
	auto deviceExtensions = device.enumerateDeviceExtensionProperties();
	auto deviceQueueFamilyProperties = device.getQueueFamilyProperties();
	BasicRenderingQueueFamilyIndices indices;

	int i = 0;
	for (const auto& queueFamily : deviceQueueFamilyProperties) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.families[static_cast<size_t>(QueueSpecialisation::GRAPHICS)] = i;
		}
		if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) {
			indices.families[static_cast<size_t>(QueueSpecialisation::TRANSFER)] = i;
		}
		vk::Bool32 supported = false;
		auto result = device.getSurfaceSupportKHR(i, window.getSurface(), &supported);

		if (result != vk::Result::eSuccess) {
			throw std::runtime_error("failed to get surface support!");
		}
		else if (supported) {
			indices.families[static_cast<size_t>(QueueSpecialisation::PRESENT)] = i;
		}
		if (indices.isComplete()) {
			break;
		}
		i++;
	}

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
		return false;
	}

	//// Application can't function without geometry shaders
	//if (!deviceFeatures.geometryShader) {
	//	return false;
	//}

	std::set<std::string> requiredExtensionSet(requiredExtensions.begin(), requiredExtensions.end());

	for (const auto& extension : deviceExtensions) {
		requiredExtensionSet.erase(extension.extensionName);
	}

	if (!requiredExtensionSet.empty())
		return false;

	auto result = device.getSurfaceCapabilitiesKHR(window.getSurface(), &swapChainSupportDetails.capabilities);
	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to get surface capabilities!");
	}
	swapChainSupportDetails.formats = device.getSurfaceFormatsKHR(window.getSurface());
	swapChainSupportDetails.presentModes = device.getSurfacePresentModesKHR(window.getSurface());

	if (swapChainSupportDetails.formats.empty() || swapChainSupportDetails.presentModes.empty())
		return false;

	basicIndices = indices;
	return true;
}

void EngineDevice::selectPhysicalDevice(const EngineContext& instance,
	const Window& window)
{
	uint32_t deviceCount = 0;
	const auto& devices = instance.getInstance().enumeratePhysicalDevices();
	if (devices.size() == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	for (const auto& device : devices) {
		if (evaluateDevice(device, instance, window, m_basicIndices, m_swapChainSupportDetails)) {
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == nullptr) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void EngineDevice::createLogicalDevice(const EngineContext& instance,
	const Window& window)
{
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::GRAPHICS)].value(),
		m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::PRESENT)].value(),
		m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::TRANSFER)].value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		vk::DeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk::PhysicalDeviceFeatures deviceFeatures;
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	vk::DeviceCreateInfo createInfo{};
	createInfo.sType = vk::StructureType::eDeviceCreateInfo;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

#ifdef _DEBUG
	createInfo.enabledLayerCount = static_cast<uint32_t>(EngineContext::validationLayers.size());
	createInfo.ppEnabledLayerNames = EngineContext::validationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	try {
		m_device = m_physicalDevice.createDevice(createInfo, nullptr, instance.getDispatchLoader());
		instance.updateDispatchLoader(m_device);
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to create logical device! " + std::string(e.what()));
	}

	m_queues[static_cast<size_t>(QueueSpecialisation::GRAPHICS)] =
		m_device.getQueue(m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::GRAPHICS)].value(),
			0, instance.getDispatchLoader());
	m_queues[static_cast<size_t>(QueueSpecialisation::PRESENT)] =
		m_device.getQueue(m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::PRESENT)].value(),
			0, instance.getDispatchLoader());
	m_queues[static_cast<size_t>(QueueSpecialisation::TRANSFER)] =
		m_device.getQueue(m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::TRANSFER)].value(),
			0, instance.getDispatchLoader());
}

vk::DeviceMemory EngineDevice::allocateMemory(const EngineContext& instance,
	size_t requiredCapacity, uint32_t memoryType) const
{
	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
	allocInfo.allocationSize = requiredCapacity;
	allocInfo.memoryTypeIndex = memoryType;

	try {
		return m_device.allocateMemory(allocInfo, nullptr, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to allocate buffer memory: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error allocating buffer memory: " + std::string(e.what()));
	}
}

uint32_t EngineDevice::findMemoryType(const EngineContext& instance,
	uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	memProperties = m_physicalDevice.getMemoryProperties(instance.getDispatchLoader());

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

vk::Format EngineDevice::findSupportedFormat(const EngineContext& instance, 
	const std::vector<vk::Format>& candidates,
	vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
	for (vk::Format format : candidates) {
		vk::FormatProperties props;
		m_physicalDevice.getFormatProperties(format, &props, instance.getDispatchLoader());

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}


//void EngineDevice::createLogicalDevice(const DeviceRequirements& requirements) {
//
//    // Get queue family indices
//    std::set<uint32_t> uniqueQueueFamilies = {
//        m_queues.graphicsFamily.value(),
//        m_queues.presentFamily.value()
//    };
//
//    // Create queue create infos
//    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
//    float queuePriority = 1.0f;
//
//    for (uint32_t queueFamily : uniqueQueueFamilies) {
//        vk::DeviceQueueCreateInfo queueCreateInfo;
//        queueCreateInfo.queueFamilyIndex = queueFamily;
//        queueCreateInfo.queueCount = 1;
//        queueCreateInfo.pQueuePriorities = &queuePriority;
//        queueCreateInfos.push_back(queueCreateInfo);
//    }
//
//    // Specify device features
//    vk::PhysicalDeviceFeatures deviceFeatures{};
//    deviceFeatures.samplerAnisotropy = VK_TRUE;  // Example feature
//
//    // Specify required extensions
//    std::vector<const char*> deviceExtensions = {
//        VK_KHR_SWAPCHAIN_EXTENSION_NAME
//    };
//
//    // Create the logical device
//    vk::DeviceCreateInfo createInfo{
//        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
//        .pQueueCreateInfos = queueCreateInfos.data(),
//        .enabledLayerCount = 0,
//        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
//        .ppEnabledExtensionNames = deviceExtensions.data(),
//        .pEnabledFeatures = &deviceFeatures
//    };
//
//    try {
//        m_device = m_physicalDevice.createDevice(createInfo);
//
//        // Get queue handles
//        m_graphicsQueue = m_device.getQueue(m_queueFamilyIndices.graphicsFamily.value(), 0);
//        m_presentQueue = m_device.getQueue(m_queueFamilyIndices.presentFamily.value(), 0);
//    }
//    catch (const vk::SystemError& e) {
//        throw std::runtime_error("Failed to create logical device: " + std::string(e.what()));
//    }
//
//}
//
//void EngineDevice::selectPhysicalDevice(const EngineContext& instance,
//	const Window& window)
//{
//	vk::PhysicalDevice device = nullptr;
//    DeviceRequirements::RateResult maxRating;
//	std::vector<DeviceRequirements::RateResult> results;
//
//	auto physicalDevices = instance.getInstance().enumeratePhysicalDevices();
//
//	for (auto& physicalDevice : physicalDevices) {
//		auto properties = physicalDevice.getProperties();
//		auto features = physicalDevice.getFeatures();
//
//        results.push_back(requirements.rateDevice(physicalDevice, window.getSurfaceKHR()));
//
//		if (results.back().rating.suitable && maxRating.rating.score < results.back().rating.score)
//		{
//			maxRating = results.back();
//			device = physicalDevice;
//		}
//	}
//
//	if (device == nullptr)
//	{
//		for (size_t i = 0; i < results.size(); i++) {
//            std::cerr << "Device " << physicalDevices[i].getProperties().deviceName
//                << " is not suitable, reasons: " << std::endl;
//            for (int i = 0; i < results[i].rating.reasons.size(); i++)
//                std::cerr << results[i].rating.reasons[i] << std::endl;
//		}
//		throw std::runtime_error("No suitable device found");
//	}
//
//	std::cout << "Selected device: " << device.getProperties().deviceName << std::endl;
//    m_queueFamiliesPropertyTable = std::move(maxRating.queueFamiliesPropertyTable);
//    m_physicalDevice = device;
//}