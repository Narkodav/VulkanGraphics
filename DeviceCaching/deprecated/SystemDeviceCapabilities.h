#pragma once
#include "QueueCaching.h"
#include "PropertyCaching.h"
#include "FeatureCaching.h"

#include <vulkan/vulkan.hpp>

#include <optional>
#include <functional>
#include <any>
#include <map>

enum class RequirementFlags
{
    NEEDS_PRESENT_SUPPORT, //requires requirement queue index
    SHOULD_BE_GPU //needs a bool

};

struct DeviceCapabilities {
    vk::PhysicalDevice device;
    std::vector<std::map<QueuePropertyType, std::any>> queueProperties;
    std::map<PropertyType, std::any> properties;
    std::map<FeatureType, bool> features;
    std::set<std::string> extensions;
    
    std::vector<size_t> checkPresentationSupport(const Window& window)
    {
        std::vector<size_t> supportingQueues;
        for (int i = 0; i < queueProperties.size(); i++)
            if (device.getSurfaceSupportKHR(i, window.getSurfaceKHR()))
                supportingQueues.push_back(i);
        return supportingQueues;
    }

};

struct DeviceSelection
{
    DeviceCapabilities deviceCapabilities;
    std::vector<std::vector<size_t>> queueRequirementResults; //first is the family index second the is requirements
};

class SystemDeviceCapabilitiesCache
{
	static std::vector<DeviceCapabilities> devices;

    static void cacheProperties(const EngineContext& instance) //called on instance creation
    {
		auto physicalDevices = instance.getInstance().enumeratePhysicalDevices();
		devices.resize(physicalDevices.size());

		for (int i = 0; i < devices.size(); i++) {
			
            devices[i].device = physicalDevices[i];

            //extensions enumeration
            auto extensios = physicalDevices[i].enumerateDeviceExtensionProperties();
            devices[i].extensions = { extensios.begin(), extensios.end() };
			
            //queue family property enumeration
            auto queueFamilies = physicalDevices[i].getQueueFamilyProperties2();
            devices[i].queueProperties.resize(queueFamilies.size());

            for (int j = 0; j < queueFamilies.size(); j++)
                devices[i].queueProperties[j] = QueueCaching::getProperties(queueFamilies[j]);

            //device properties enumeration
            devices[i].properties = PropertyCaching::getProperties(physicalDevices[i].getProperties2());

            //device features enumeration
            devices[i].features = FeatureCaching::getFeatures(physicalDevices[i].getFeatures2());
		}

    }

    static std::vector<std::vector<size_t>> evaluateDevice(const DeviceCapabilities& device, Rating& rating,
        const std::vector<FeatureType>& features,
        const PropertyCaching::PropertyMap& properties,
        const std::vector<QueueCaching::QueuePropertyMap>& queueProperties,
        const std::vector<const char*>& requiredExtensions,
        const Window* windowFlag = nullptr) //called on instance creation
    {
        std::vector<std::vector<size_t>> suitableQueueIndices;
        suitableQueueIndices.resize(queueProperties.size());

        if (!FeatureCaching::compare(rating, features, device.features))
            return std::vector<std::vector<size_t>>();

        if (!PropertyCaching::compare(rating, properties, device.properties))
            return std::vector<std::vector<size_t>>();

        for (int i = 0; i < device.queueProperties.size(); i++)
        {
            for (int j = 0; j < queueProperties.size(); j++)
            {
                if (QueueCaching::compare(rating, queueProperties[j], device.queueProperties[i], windowFlag))
                    suitableQueueIndices[i].push_back(j);
            }
        }

        if (!rating.suitable)
            return std::vector<std::vector<size_t>>();

        for (const auto& requiredExt : requiredExtensions) {
            bool found = false;
            for (const auto& ext : device.extensions) {
                if (strcmp(requiredExt, ext.c_str()) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                rating.suitable = false;
                rating.reasons.push_back("Missing required extension: " + std::string(requiredExt));
            }
        }

        if (!rating.suitable)
            return std::vector<std::vector<size_t>>();


    }

    static DeviceSelection selectDevice(const std::vector<FeatureType>& features,
        const PropertyCaching::PropertyMap& properties,
        const std::vector<QueueCaching::QueuePropertyMap>& queueProperties,
        const std::vector<const char*>& requiredExtensions,
        const Window* windowFlag = nullptr) //called on instance creation
    {
        std::vector<Rating> ratings(devices.size());

        //device index / family index (in queueProperties) / requirement indices, any size, if second size is zero - unsuitable
        std::vector<std::vector<std::vector<size_t>>> suitableQueueFamilies(devices.size());

        size_t maxScore = 0, maxScoreIndex = 0;

        for (int i = 0; i < devices.size(); i++)
        {
            suitableQueueFamilies[i] = evaluateDevice(devices[i], ratings[i], features,
            properties, queueProperties, requiredExtensions, windowFlag);

            if (ratings[i].suitable && maxScore < ratings[i].score)
            {
                maxScore = ratings[i].score;
                maxScoreIndex = i;
            }
        }

        DeviceSelection deviceResult;
        deviceResult.deviceCapabilities = devices[maxScoreIndex];
        deviceResult.queueRequirementResults = suitableQueueFamilies[maxScoreIndex];

        return deviceResult;

    }


};

