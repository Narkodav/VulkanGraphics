#pragma once
#include "../Common.h"
#include "../Rendering/Context.h"
#include "../PlatformManagement/Window.h"

#include "FeatureEnum.h"
#include "PropertyEnum.h"
#include "QueuePropertyEnum.h"

#include <array>
#include <any>
#include <set>
#include <type_traits>
#include <typeindex>

namespace Graphics {

	class PhysicalDevice;

	class QueueFamily
	{
		friend class PhysicalDevice;
	private:
		std::array<std::any, static_cast<size_t>(QueueProperty::PropertiesNum)> m_properties;
		uint32_t m_familyIndex;

	public:
		QueueFamily() = default;

		QueueFamily(const QueueFamily&) = delete;
		QueueFamily(QueueFamily&&) = default;
		QueueFamily& operator=(const QueueFamily&) = delete;
		QueueFamily& operator=(QueueFamily&&) = default;

		QueueFamily(const vk::QueueFamilyProperties2& properties, uint32_t familyIndex);

		const std::any& getProperty(QueueProperty property) const {
			return m_properties[static_cast<size_t>(property)];
		}

		template<QueueProperty P>
		const typename QueuePropertyTypeTrait<P>::Type& getProperty() const {
			return std::any_cast<const typename QueuePropertyTypeTrait<P>::Type&>(
				m_properties[static_cast<size_t>(P)]);
		}

		uint32_t getFamilyIndex() const { return m_familyIndex; };

	private:

		bool getSurfaceSupport(const Context& instance, const Surface& surface,
			vk::PhysicalDevice& physicalDevice) const;

		template<typename T>
		void storeProperty(QueueProperty property, const T& value) {
			m_properties[static_cast<size_t>(property)] = value;
		}
	};

}