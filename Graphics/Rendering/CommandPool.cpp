#include "Queue.h"
#include "CommandPool.h"

namespace Graphics {

    CommandPool::CommandPool(const Context& instance, const Device& device, uint32_t queueFamilyIndex)
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

    void CommandPool::makeOneTimeSubmit(const Context& instance, const Device& device,
        const Queue& queue, std::function<void(const CommandBufferHandle&)>&& func)
    {
        auto temporary = this->allocateBuffer(instance, device);
        try {
            temporary->reset(instance);
            temporary->record(instance, CommandBufferUsage::Bits::OneTimeSubmit);

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

    void CommandPool::makeOneTimeImageDataTransfer(const Context& instance, const Device& device,
        const Queue& queue, MappedMemory& stagingMemory, const Buffer& stagingBuffer,
        Image& image, const PixelData& data, uint32_t dstOffset /*= 0*/,
        Offset3D dstImageOffset /*= Offset3D()*/)
    {
        makeOneTimeSubmit(instance, device, queue,
            [this, &instance, &stagingMemory, &stagingBuffer, &image,
            &data, &dstOffset, &dstImageOffset]
            (CommandBufferHandle temporary) {
                temporary->setPipelineBarrier(instance,
                    vk::PipelineStageFlagBits::eTopOfPipe,
                    vk::PipelineStageFlagBits::eTransfer,
                    image,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::AccessFlagBits::eNone,
                    vk::AccessFlagBits::eTransferWrite
                    );
                auto pixelData = data.getPixelData();
                auto mappedMemory = stagingMemory.getMapping<uint8_t>(pixelData.size());
                std::copy(pixelData.begin(), pixelData.end(), mappedMemory.begin());
                temporary->transferImageData(instance, stagingBuffer, image,
                    data.getExtent3D(), dstOffset, dstImageOffset);

                temporary->setPipelineBarrier(instance,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    image,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eTransferWrite,
                    vk::AccessFlagBits::eShaderRead
                );
            });
    }
}