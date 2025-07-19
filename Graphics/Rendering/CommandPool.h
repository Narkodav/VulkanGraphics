#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"
#include "../PlatformManagement/Window.h"
#include "CommandBuffer.h"
#include "../MemoryManagement/MappedMemory.h"

namespace Graphics {

    using CommandBufferHandle = std::shared_ptr<CommandBuffer>;
    class Queue;

    class CommandPool
    {
    private:
        vk::CommandPool m_pool;
        std::set<std::shared_ptr<CommandBuffer>> m_allocatedBuffers;

        bool m_initialized = false;
    public:
        CommandPool() : m_pool(nullptr), m_initialized(false) {};

        CommandPool(const Context& instance, const Device& device, uint32_t queueFamilyIndex);

        CommandPool(CommandPool&& other) noexcept {
            m_pool = std::exchange(other.m_pool, nullptr);
            m_allocatedBuffers = std::exchange(other.m_allocatedBuffers, {});
            m_initialized = std::exchange(other.m_initialized, false);
        };

        //moving to an initialized swap chain is undefined behavior, destroy before moving
        CommandPool& operator=(CommandPool&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "CommandPool::operator=() - CommandPool already initialized");

            m_pool = std::exchange(other.m_pool, nullptr);
            m_allocatedBuffers = std::exchange(other.m_allocatedBuffers, {});
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        CommandPool(const CommandPool&) noexcept = delete;
        CommandPool& operator=(const CommandPool&) noexcept = delete;

        ~CommandPool() {
            assert(!m_initialized && "CommandPool was not destroyed!");
            assert(m_allocatedBuffers.size() == 0 && "CommandPool has allocated buffers!");
        };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;
            assert(m_allocatedBuffers.size() == 0 && "CommandPool has allocated buffers!");

            device.getDevice().destroyCommandPool(m_pool, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Destroyed CommandPool" << std::endl;
#endif
            m_initialized = false;
        };

        CommandBufferHandle allocateBuffer(const Context& instance, const Device& device,
            vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) {
            vk::CommandBufferAllocateInfo allocInfo{};
            allocInfo.setCommandPool(m_pool)
                .setLevel(level)
                .setCommandBufferCount(1);

            vk::CommandBuffer cmdBuffer;
            vk::Result result = device.getDevice().allocateCommandBuffers(&allocInfo, &cmdBuffer, instance.getDispatchLoader());

            if (result != vk::Result::eSuccess)
                throw std::runtime_error("Failed to allocate command buffer");
            CommandBuffer buffer(cmdBuffer);
            CommandBufferHandle handle = std::make_shared<CommandBuffer>(std::move(buffer));
            m_allocatedBuffers.emplace(handle);
            return handle;
        }

        void freeBuffer(const Context& instance, const Device& device, CommandBufferHandle& buffer) {

            if (m_allocatedBuffers.find(buffer) == m_allocatedBuffers.end())
                throw std::runtime_error("Buffer is deallocated or doesn't belong to this pool");
            m_allocatedBuffers.erase(buffer);
            device.getDevice().freeCommandBuffers(
                m_pool, 1, &buffer->m_commandBuffer, instance.getDispatchLoader());
            buffer->m_commandBuffer = nullptr;
            buffer->m_isValid = false;
        }

        // Reset the entire pool (faster than freeing individual buffers)
        void reset(const Context& instance, const Device& device) {
            device.getDevice().resetCommandPool(m_pool, vk::CommandPoolResetFlags(), instance.getDispatchLoader());

            for (auto& buffer : m_allocatedBuffers)
            {
                buffer->m_commandBuffer = nullptr;
                buffer->m_isValid = false;
            }

            m_allocatedBuffers.clear();
        }

        void makeOneTimeSubmit(const Context& instance, const Device& device,
            const Queue& queue, std::function<void(const CommandBufferHandle&)>&& func);

        template <typename Type, typename Data>
        void makeOneTimeDataTransfer(const Context& instance, const Device& device,
            const Queue& queue, MappedMemory& stagingMemory, const Buffer& stagingBuffer,
            const Buffer& dstBuffer, const Data& data, uint32_t dstOffset = 0)
        {
            makeOneTimeSubmit(instance, device, queue,
                [this, &instance, &stagingMemory, &stagingBuffer, &dstBuffer, &dstOffset, &data]
                (CommandBufferHandle temporary) {
                    auto mappedMemory = stagingMemory.getMapping<Type>(data.size());
                    std::copy(data.begin(), data.end(), mappedMemory.begin());
                    temporary->transferBufferData(instance, stagingBuffer,
                        dstBuffer, CopyRegion(0, dstOffset, data.size() * sizeof(Type)));
                });
        }

        const vk::CommandPool& getPool() const { return m_pool; };
    };
}