#include "QueueFamily.h"

QueueFamily::QueueFamily(const vk::QueueFamilyProperties2& properties, uint32_t familyIndex) : 
	m_familyIndex(familyIndex)
{	
    vk::QueueFamilyProperties baseProps = properties.queueFamilyProperties;
    vk::QueueFamilyGlobalPriorityPropertiesKHR globalPriorityProps = 
        *static_cast<vk::QueueFamilyGlobalPriorityPropertiesKHR*>(properties.pNext);
    vk::QueueFamilyVideoPropertiesKHR videoProps =
        *static_cast<vk::QueueFamilyVideoPropertiesKHR*>(globalPriorityProps.pNext);
    vk::QueueFamilyQueryResultStatusPropertiesKHR queryResultProps =
        *static_cast<vk::QueueFamilyQueryResultStatusPropertiesKHR*>(videoProps.pNext);
    vk::QueueFamilyCheckpointPropertiesNV checkpointProps =
        *static_cast<vk::QueueFamilyCheckpointPropertiesNV*>(queryResultProps.pNext);
    vk::QueueFamilyCheckpointProperties2NV checkpoint2Props =
        *static_cast<vk::QueueFamilyCheckpointProperties2NV*>(checkpointProps.pNext);

    storeProperty(QueueProperty::QUEUE_FLAGS,
        static_cast<QueueFlags::Flags>(baseProps.queueFlags));
    storeProperty(QueueProperty::QUEUE_COUNT,
        static_cast<uint32_t>(baseProps.queueCount));
    storeProperty(QueueProperty::MIN_IMAGE_TRANSFER_GRANULARITY,
        std::array<uint32_t, 3>({ baseProps.minImageTransferGranularity.width,
        baseProps.minImageTransferGranularity.height,
        baseProps.minImageTransferGranularity.depth }));
    storeProperty(QueueProperty::TIMESTAMP_VALID_BITS,
        static_cast<uint32_t>(baseProps.timestampValidBits));

}

bool QueueFamily::getSurfaceSupport(const EngineContext& instance, const Window& window,
    vk::PhysicalDevice& physicalDevice) const
{
    return physicalDevice.getSurfaceSupportKHR(m_familyIndex,
        window.getSurface(), instance.getDispatchLoader());
}