#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "RenderRegion.h"
#include "../MemoryManagement/Buffer.h"
#include "../MemoryManagement/Image.h"
#include "../MemoryManagement/DescriptorSet.h"
#include "../MemoryManagement/DescriptorPool.h"

namespace Graphics {

    class CommandPool;

    class CommandBuffer
    {
    private:
        vk::CommandBuffer m_commandBuffer = nullptr;

        bool m_isValid = false;

        CommandBuffer(vk::CommandBuffer& commandBuffer) :
            m_commandBuffer(commandBuffer), m_isValid(true) {
        };

    public:

        CommandBuffer() : m_commandBuffer(nullptr), m_isValid(false) {};

        CommandBuffer(CommandBuffer&& other) noexcept {
            m_commandBuffer = std::exchange(other.m_commandBuffer, nullptr);
            m_isValid = std::exchange(other.m_isValid, false);
        };

        //moving to an initialized swap chain is undefined behavior, destroy before moving
        CommandBuffer& operator=(CommandBuffer&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_isValid && "CommandBuffer::operator=() - CommandBuffer already allocated");

            m_commandBuffer = std::exchange(other.m_commandBuffer, nullptr);
            m_isValid = std::exchange(other.m_isValid, false);

            return *this;
        };

        CommandBuffer(const CommandBuffer&) noexcept = delete;
        CommandBuffer& operator=(const CommandBuffer&) noexcept = delete;

        ~CommandBuffer() { assert(!m_isValid && "CommandBuffer was not deallocated!"); };

        void record(const Context& instance, CommandBufferUsage::Flags flags = 0);
        void beginRenderPass(const Context& instance, const RenderPass& renderPass,
            const SwapChain& swapChain, uint32_t imageIndex, Color clearColor);

        void beginRenderPass(const Context& instance, const RenderPass& renderPass,
            const SwapChain& swapChain, uint32_t imageIndex, Color clearColor, float clearDepth);

        void bindPipeline(const Context& instance, const Pipeline& pipeline);

        template<size_t bufferAmount>
        void bindVertexBuffers(const Context& instance,
            std::array<std::reference_wrapper<const Buffer>, bufferAmount> buffers,
            std::array<vk::DeviceSize, bufferAmount> offsets,
            uint32_t firstBinding = 0)
        {
            auto rawBuffers = convert<vk::Buffer>
                (buffers, [](std::reference_wrapper<const Buffer> buffer)
                    { return buffer.get().getBuffer(); });

            m_commandBuffer.bindVertexBuffers(firstBinding, rawBuffers.size(), rawBuffers.data(),
                offsets.data(), instance.getDispatchLoader());
        }

        void bindIndexBuffer(const Context& instance,
            const Buffer& buffer, vk::DeviceSize offset);

        void bindDescriptorSets(const Context& instance,
            const Pipeline& pipeline, const std::vector<DescriptorSetHandle>& descriptorSets,
            const std::vector<uint32_t>& dynamicOffsets = {});

        void setPipelineBarrier(const Context& instance,
            vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
            Image& image, vk::ImageLayout newLayout,
            vk::AccessFlags srcAccess, vk::AccessFlags dstAccess);

        void transferBufferData(const Context& instance, const Buffer& srcBuffer,
            const Buffer& dstBuffer, const CopyRegion& copyRegion);

        void transferImageData(const Context& instance, const Buffer& srcBuffer,
            Image& dstImage, size_t offset = 0, vk::Offset3D imageOffset = { 0, 0, 0 });

        void setRenderView(const Context& instance, const RenderRegion& canvas);
        void draw(const Context& instance,
            size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance);
        void drawIndexed(const Context& instance,
            size_t indexCount, size_t instanceCount, size_t firstIndex, size_t indexIncrement, size_t firstInstance);

        void endRenderPass(const Context& instance);
        void stopRecord(const Context& instance);
        void reset(const Context& instance);

        bool isValid() const {
            return m_isValid;
        }

        vk::CommandBuffer getCommandBuffer() const { return m_commandBuffer; };

        friend class CommandPool;
    };

}