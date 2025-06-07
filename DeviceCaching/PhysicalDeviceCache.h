#pragma once
#include "Common.h"
#include "Rendering/EngineContext.h"

#include "FeatureEnum.h"
#include "PropertyEnum.h"
#include "QueuePropertyEnum.h"
#include "PhysicalDevice.h"

#include <array>
#include <set>
#include <map>
#include <any>
#include <functional>


using RequiredFeatures = std::map<DeviceFeature, std::any>;
using RequiredProperties = std::map<DeviceProperty, std::any>;
using RequiredExtensions = std::set<std::string>;

struct RequiredQueueProperties
{	
	std::map<QueueProperty, std::any> queueProperties;
	bool shouldSupportPresent = false;
};

struct DeviceRequirements
{
	RequiredFeatures features;
	RequiredProperties properties;
	RequiredExtensions extensions;

	std::vector<RequiredQueueProperties> queueProperties;
};

//device will be nullptr if not found
//returns a vector containing vectors of suitable queues for each queue requirement
struct DeviceSearchResult
{
	const PhysicalDevice* device = nullptr;
	std::vector<std::vector<uint32_t>> queueFamilyIndices;

	inline bool isSuitable() { return device != nullptr; };

	static DeviceSearchResult unsuitable() { return DeviceSearchResult(); };
	static DeviceSearchResult suitable(
		const PhysicalDevice& device, std::vector<std::vector<uint32_t>>&& queueFamilyIndices) 
	{
		return DeviceSearchResult{ &device , std::move(queueFamilyIndices) };
	};
};

class PhysicalDeviceCache
{
	using TaskTableSignature = std::function<bool(const std::any&, const std::any&)>;
private:
	std::vector<PhysicalDevice> m_systemDevices;

	static const std::array<TaskTableSignature,
		static_cast<size_t>(DeviceFeature::FEATURES_NUM)> featureChecks;
	static const std::array<TaskTableSignature,
		static_cast<size_t>(DeviceProperty::PROPERTIES_NUM)> propertyChecks;
	static const std::array<TaskTableSignature,
		static_cast<size_t>(QueueProperty::PROPERTIES_NUM)> queuePropertyChecks;

public:
	PhysicalDeviceCache() = default;

	PhysicalDeviceCache(const PhysicalDeviceCache&) = delete;
	PhysicalDeviceCache& operator=(const PhysicalDeviceCache&) = delete;

	PhysicalDeviceCache(PhysicalDeviceCache&&) = default;
	PhysicalDeviceCache& operator=(PhysicalDeviceCache&&) = default;

	PhysicalDeviceCache(const EngineContext& instance);

	//this version will check for queue present support if required
	DeviceSearchResult getFittingDevice(const EngineContext& instance,
		const Window& window, const DeviceRequirements& requirements);

	//this version will not check for queue present support
	DeviceSearchResult getFittingDevice(const DeviceRequirements& requirements);

	const std::vector<PhysicalDevice>& getCachedDevices() { return m_systemDevices; };

	//this version will check for queue present support if required
	static DeviceSearchResult checkDeviceSuitability(const EngineContext& instance,
		const Window& window, const DeviceRequirements& requirements,
		const PhysicalDevice& device);

	//this version will not check for queue present support
	static DeviceSearchResult checkDeviceSuitability(
		const DeviceRequirements& requirements, const PhysicalDevice& device);

	//this version will check for queue present support
	static bool checkQueueFamilySuitability(const EngineContext& instance,
		const PhysicalDevice& device, const Window& window,
		const std::map<QueueProperty, std::any>& requirements,
		const QueueFamily& queueFamily);

	//this version will not check for queue present support
	static bool checkQueueFamilySuitability(const std::map<QueueProperty,
		std::any>& requirements, const QueueFamily& queueFamily);
};

