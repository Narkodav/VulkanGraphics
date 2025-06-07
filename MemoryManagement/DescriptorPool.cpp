#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(const EngineContext& instance, const Device& device,
    const std::vector<Size>& sizes, size_t maxSets)
{
    for (const auto& size : sizes)
        m_sizes.push_back(size);

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    poolInfo.poolSizeCount = m_sizes.size();
    poolInfo.pPoolSizes = m_sizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(maxSets);

    try {
        m_pool = device.getDevice().createDescriptorPool(poolInfo, nullptr, instance.getDispatchLoader());
    }
    catch (const vk::SystemError& e) {
        throw std::runtime_error("Failed to create a DescriptorPool: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unexpected error creating a DescriptorPool: " + std::string(e.what()));
    }

    m_initialized = true;
}