#include "Queue.h"
#include "CommandPool.h"

CommandPool::CommandPool(const GraphicsContext& instance, const Device& device, uint32_t queueFamilyIndex)
{
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
	poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = queueFamilyIndex;

    try {
        m_pool = device.getDevice().createCommandPool(poolInfo, nullptr, instance.getDispatchLoader());
    }
    catch (const vk::SystemError& e) {
        throw std::runtime_error("Failed to create a CommandPool: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Unexpected error creating a CommandPool: " + std::string(e.what()));
    }

    m_initialized = true;
}

void CommandPool::makeOneTimeSubmit(const GraphicsContext& instance, const Device& device,
    const Queue& queue, std::function<void(const CommandBufferHandle&)>&& func)
{
    auto temporary = this->allocateBuffer(instance, device);
    try {
        temporary->reset(instance);
        temporary->record(instance, CommandBufferUsage::Bits::ONE_TIME_SUBMIT);

        func(temporary);

        temporary->stopRecord(instance);
        queue.submit(instance, std::array{ std::ref(temporary) });
        queue.waitIdle(instance);
        this->freeBuffer(instance, device, temporary);
    }
    catch (...) {
        this->freeBuffer(instance, device, temporary);
        throw;
    }
}