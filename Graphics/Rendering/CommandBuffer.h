#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "ComputePipeline.h"
#include "RenderRegion.h"
#include "Flags.h"
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

        template<typename PipelineType>
        void bindPipeline(const Context& instance, const PipelineType& pipeline)
        {
            try {
                m_commandBuffer.bindPipeline(pipeline.getBindPoint(), pipeline.getPipeline(), instance.getDispatchLoader());
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error("failed to bind graphics pipeline: " + std::string(e.what()));
            }
            catch (const std::exception& e) {
                throw std::runtime_error("Unexpected error when binding graphics pipeline: " + std::string(e.what()));
            }
        }
        template<size_t bufferAmount>
        void bindVertexBuffers(const Context& instance,
            std::array<const Buffer*, bufferAmount> buffers,
            std::array<vk::DeviceSize, bufferAmount> offsets,
            uint32_t firstBinding = 0)
        {
            auto rawBuffers = convert<vk::Buffer>
                (buffers, [](const Buffer* buffer)
                    { return buffer->getBuffer(); });

            m_commandBuffer.bindVertexBuffers(firstBinding, rawBuffers.size(), rawBuffers.data(),
                offsets.data(), instance.getDispatchLoader());
        }

        void bindIndexBuffer(const Context& instance,
            const Buffer& buffer, vk::DeviceSize offset);

        template<typename PipelineType>
        void bindDescriptorSets(const Context& instance,
            const PipelineType& pipeline, const std::vector<DescriptorSetHandle>& descriptorSets,
            const std::vector<uint32_t>& dynamicOffsets = {})
        {
            auto descriptorSetsRaw = convert<vk::DescriptorSet>
                (descriptorSets, [](const DescriptorSetHandle& set)
                    { return set->getSet(); });

            m_commandBuffer.bindDescriptorSets(pipeline.getBindPoint(),
                pipeline.getLayout(), 0, descriptorSetsRaw, dynamicOffsets,
                instance.getDispatchLoader());
        }

        void setPipelineBarrier(const Context& instance,
            Graphics::PipelineStage::Flags srcStage, Graphics::PipelineStage::Flags dstStage,
            Image& image, vk::ImageLayout newLayout,
            vk::AccessFlags srcAccess, vk::AccessFlags dstAccess);

        void setPipelineBarrier(const Context& instance,
            Graphics::PipelineStage::Flags srcStage, Graphics::PipelineStage::Flags dstStage,
            Buffer& buffer, vk::AccessFlags srcAccess,
            vk::AccessFlags dstAccess);

        void transferBufferData(const Context& instance, const Buffer& srcBuffer,
            const Buffer& dstBuffer, const CopyRegion& copyRegion);

        void transferImageData(const Context& instance, const Buffer& srcBuffer,
            Image& dstImage, Extent3D imageExtent, size_t offset = 0,
            Offset3D imageOffset = Offset3D());

        template<typename PipelineType>
        void pushConstants(const Context& instance, const PipelineType& pipeline,
            ShaderStage::Flags stageFlags, size_t offset, size_t size, const void* values)
        {
            try {
                m_commandBuffer.pushConstants(pipeline.getLayout(),
                    static_cast<vk::ShaderStageFlagBits>(stageFlags),
                    offset, size, values, instance.getDispatchLoader());
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error("Failed to write push constants: " + std::string(e.what()));
            }
            catch (const std::exception& e) {
                throw std::runtime_error("Unexpected error when writing push constants: " + std::string(e.what()));
            }
        };

        void setRenderView(const Context& instance, const RenderRegion& canvas);
        void draw(const Context& instance,
            size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance);
        void drawIndexed(const Context& instance,
            size_t indexCount, size_t instanceCount, size_t firstIndex, size_t indexIncrement, size_t firstInstance);

        void endRenderPass(const Context& instance);
        void stopRecord(const Context& instance);
        void reset(const Context& instance);

        void dispatch(const Context& instance, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

        void drawIndirect(const Context& instance, const Buffer& buffer,
            vk::DeviceSize offset, uint32_t drawCount, uint32_t stride);

        bool isValid() const {
            return m_isValid;
        }

        vk::CommandBuffer getCommandBuffer() const { return m_commandBuffer; };

        friend class CommandPool;
    };

}