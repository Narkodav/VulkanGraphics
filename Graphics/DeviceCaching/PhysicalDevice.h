#pragma once
#include "../Common.h"
#include "../Rendering/Context.h"

#include "FeatureEnum.h"
#include "PropertyEnum.h"
#include "QueuePropertyEnum.h"
#include "QueueFamily.h"

#include <array>
#include <set>

namespace Graphics {

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	class PhysicalDevice
	{
	private:
		vk::PhysicalDevice m_physicalDevice;

		std::array<std::any, static_cast<size_t>(DeviceFeature::FeaturesNum)> m_features;
		std::array<std::any, static_cast<size_t>(DeviceProperty::PropertiesNum)> m_properties;
		std::vector<QueueFamily> m_queueFamilies;
		std::set<std::string> m_availableExtensions;

	public:

		PhysicalDevice() = default;

		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&) = default;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) = default;

		PhysicalDevice(const Context& instance, vk::PhysicalDevice device) :
			m_physicalDevice(device)
		{
			enumerateFeatures(instance);
			enumerateProperties(instance);
			enumerateExtensions(instance);
			enumerateQueueFamilies(instance);
		}

		const std::any& getFeature(DeviceFeature feature) const {
			return m_features[static_cast<size_t>(feature)];
		}

		const std::any& getProperty(DeviceProperty property) const {
			return m_properties[static_cast<size_t>(property)];
		}

		template<DeviceFeature F>
		const typename DeviceFeatureTypeTrait<F>::Type& getFeature() const {
			return std::any_cast<const typename DeviceFeatureTypeTrait<F>::Type&>(
				m_features[static_cast<size_t>(F)]);
		}

		template<DeviceProperty P>
		const typename DevicePropertyTypeTrait<P>::Type& getProperty() const {
			return std::any_cast<const typename DevicePropertyTypeTrait<P>::Type&>(
				m_properties[static_cast<size_t>(P)]);
		}

		const std::vector<QueueFamily>& getQueueFamilies() const { return m_queueFamilies; };
		const std::set<std::string>& getAvailableExtensions() const { return m_availableExtensions; };
		vk::PhysicalDevice getHandle() const { return m_physicalDevice; };

		SwapChainSupportDetails getSwapChainSupportDetails(
			const Context& instance, const Surface& surface) const
		{
			SwapChainSupportDetails supports;
			supports.capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(surface.getSurface());
			supports.formats = m_physicalDevice.getSurfaceFormatsKHR(surface.getSurface());
			supports.presentModes = m_physicalDevice.getSurfacePresentModesKHR(surface.getSurface());
			return supports;
		}

	private:

		void enumerateFeatures(const Context& instance);
		void enumerateProperties(const Context& instance);
		void enumerateExtensions(const Context& instance);
		void enumerateQueueFamilies(const Context& instance);

		template<typename T>
		void storeFeature(DeviceFeature feature, const T& value) {
			m_features[static_cast<size_t>(feature)] = value;
		}

		template<typename T>
		void storeProperty(DeviceProperty property, const T& value) {
			m_properties[static_cast<size_t>(property)] = value;
		}
	};

}