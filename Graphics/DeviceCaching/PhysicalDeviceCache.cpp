#include "PhysicalDeviceCache.h"

namespace Graphics {

	PhysicalDeviceCache::PhysicalDeviceCache(const Context& instance)
	{
		std::vector<vk::PhysicalDevice> devices;
		try
		{
			devices = instance.getInstance().enumeratePhysicalDevices(
				instance.getDispatchLoader());
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error("Failed to enumerate physical devices: " + std::string(e.what()));
		}

		if (devices.size() == 0)
			throw std::runtime_error("Failed to find available Devices");

		for (const auto& physicalDevice : devices)
		{
			PhysicalDevice device(instance, physicalDevice);
			m_systemDevices.push_back(std::move(device));
		}
	}

	DeviceSearchResult PhysicalDeviceCache::getFittingDevice(const Context& instance,
		const Surface& surface, const DeviceRequirements& requirements)
	{
		for (const auto& device : m_systemDevices)
		{
			DeviceSearchResult result = checkDeviceSuitability(instance, surface, requirements, device);
			if (result.isSuitable())
				return result;
		}

		return DeviceSearchResult::unsuitable();
	}

	DeviceSearchResult PhysicalDeviceCache::getFittingDevice(const DeviceRequirements& requirements)
	{
		for (const auto& device : m_systemDevices)
		{
			DeviceSearchResult result = checkDeviceSuitability(requirements, device);
			if (result.isSuitable())
				return result;
		}

		return DeviceSearchResult::unsuitable();
	}

	DeviceSearchResult PhysicalDeviceCache::checkDeviceSuitability(const Context& instance,
		const Surface& surface, const DeviceRequirements& requirements,
		const PhysicalDevice& device)
	{
		//device checks
		for (const auto& feature : requirements.features)
			if (!featureChecks[static_cast<size_t>(feature.first)]
			(feature.second, device.getFeature(feature.first)))
				return DeviceSearchResult::unsuitable();

		for (const auto& property : requirements.properties)
			if (!propertyChecks[static_cast<size_t>(property.first)]
			(property.second, device.getProperty(property.first)))
				return DeviceSearchResult::unsuitable();

		for (const auto& extension : requirements.extensions)
			if (device.getAvailableExtensions().find(extension) == device.getAvailableExtensions().end())
				return DeviceSearchResult::unsuitable();

		std::vector<std::vector<uint32_t>> queueFamilyIndices;
		const auto& queueFamilies = device.getQueueFamilies();

		//queue family checks
		for (const auto& requirement : requirements.queueProperties)
		{
			queueFamilyIndices.push_back(std::vector<uint32_t>());
			if (requirement.shouldSupportPresent)
				for (const auto& queueFamily : queueFamilies)
				{
					if (checkQueueFamilySuitability(instance, device, surface,
						requirement.queueProperties, queueFamily))
						queueFamilyIndices.back().push_back(queueFamily.getFamilyIndex());
				}
			else
				for (const auto& queueFamily : queueFamilies)
					if (checkQueueFamilySuitability(requirement.queueProperties, queueFamily))
						queueFamilyIndices.back().push_back(queueFamily.getFamilyIndex());
			if (queueFamilyIndices.back().size() == 0)
				return DeviceSearchResult::unsuitable();
		}
		return DeviceSearchResult::suitable(device, std::move(queueFamilyIndices));
	}

	DeviceSearchResult PhysicalDeviceCache::checkDeviceSuitability(
		const DeviceRequirements& requirements, const PhysicalDevice& device)
	{
		//device checks
		for (const auto& feature : requirements.features)
			if (!featureChecks[static_cast<size_t>(feature.first)]
			(feature.second, device.getFeature(feature.first)))
				return DeviceSearchResult::unsuitable();

		for (const auto& property : requirements.properties)
			if (!propertyChecks[static_cast<size_t>(property.first)]
			(property.second, device.getProperty(property.first)))
				return DeviceSearchResult::unsuitable();

		for (const auto& extension : requirements.extensions)
			if (device.getAvailableExtensions().find(extension) == device.getAvailableExtensions().end())
				return DeviceSearchResult::unsuitable();

		std::vector<std::vector<uint32_t>> queueFamilyIndices;

		//queue family checks
		for (const auto& requirement : requirements.queueProperties)
		{
			queueFamilyIndices.push_back({});
			for (const auto& queueFamily : device.getQueueFamilies())
			{
				if (checkQueueFamilySuitability(requirement.queueProperties, queueFamily))
					queueFamilyIndices.back().push_back(queueFamily.getFamilyIndex());
			}
			if (queueFamilyIndices.back().size() == 0)
				return DeviceSearchResult::unsuitable();
		}
		return DeviceSearchResult::suitable(device, std::move(queueFamilyIndices));
	}

	bool PhysicalDeviceCache::checkQueueFamilySuitability(const Context& instance,
		const PhysicalDevice& device, const Surface& surface,
		const std::map<QueueProperty, std::any>& requirements,
		const QueueFamily& queueFamily)
	{
		if (!checkQueueFamilySuitability(requirements, queueFamily)) return false;
		if (!device.getHandle().getSurfaceSupportKHR(
			queueFamily.getFamilyIndex(), surface.getSurface(), instance.getDispatchLoader()))
			return false;
		return true;
	}

	bool PhysicalDeviceCache::checkQueueFamilySuitability(const std::map<QueueProperty,
		std::any>& requirements, const QueueFamily& queueFamily)
	{
		for (const auto& property : requirements)
			if (!queuePropertyChecks[static_cast<size_t>(property.first)]
			(property.second, queueFamily.getProperty(property.first)))
				return false;
		return true;
	}
}