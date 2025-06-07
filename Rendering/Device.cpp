#include "Device.h"

vk::DeviceMemory Device::allocateMemory(const EngineContext& instance,
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

uint32_t Device::findMemoryType(const EngineContext& instance,
	uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	memProperties = m_physicalDevice->getHandle().getMemoryProperties(instance.getDispatchLoader());

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

vk::Format Device::findSupportedFormat(const EngineContext& instance, 
	const std::vector<vk::Format>& candidates,
	vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
	for (vk::Format format : candidates) {
		vk::FormatProperties props;
		m_physicalDevice->getHandle().getFormatProperties(format, &props, instance.getDispatchLoader());

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}