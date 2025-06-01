#pragma once
#include "Common.h"
#include "EngineContext.h"
#include "EngineDevice.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "RenderRegion.h"
#include "MemoryManagement/Buffer.h"
#include "MemoryManagement/Image.h"
#include "MemoryManagement/DescriptorSet.h"
#include "MemoryManagement/DescriptorPool.h"

class CommandPool;

class CommandBuffer
{
private:
    vk::CommandBuffer m_commandBuffer = nullptr;

    bool m_isValid = false;

    CommandBuffer(vk::CommandBuffer& commandBuffer) :
        m_commandBuffer(commandBuffer), m_isValid(true) {};

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

    void record(const EngineContext& instance, CommandBufferUsage::Flags flags = 0);
    void beginRenderPass(const EngineContext& instance, const RenderPass& renderPass, 
    const SwapChain& swapChain, uint32_t imageIndex, Color clearColor);

    void beginRenderPass(const EngineContext& instance, const RenderPass& renderPass,
        const SwapChain& swapChain, uint32_t imageIndex, Color clearColor, float clearDepth);

    void bindGraphicsPipeline(const EngineContext& instance, const GraphicsPipeline& pipeline);
    
    template<size_t bufferAmount>
    void bindVertexBuffers(const EngineContext& instance,
        std::array<std::reference_wrapper<Buffer>, bufferAmount> buffers,
        std::array<vk::DeviceSize, bufferAmount> offsets,
        uint32_t firstBinding = 0)
    {
        auto rawBuffers = convert<vk::Buffer>
            (buffers, [](std::reference_wrapper<Buffer> buffer)
                { return buffer.get().getBuffer(); });

        m_commandBuffer.bindVertexBuffers(firstBinding, rawBuffers.size(), rawBuffers.data(),
            offsets.data(), instance.getDispatchLoader());
    }

    void bindIndexBuffer(const EngineContext& instance,
        const Buffer& buffer, vk::DeviceSize offset);
    
    void bindDescriptorSets(const EngineContext& instance,
        const GraphicsPipeline& pipeline, const std::vector<DescriptorSetHandle>& descriptorSets,
        const std::vector<uint32_t>& dynamicOffsets = {});

    void setPipelineBarrier(const EngineContext& instance,
        vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
        Image& image, vk::ImageLayout newLayout,
        vk::AccessFlags srcAccess, vk::AccessFlags dstAccess);

    void transferBufferData(const EngineContext& instance, const Buffer& srcBuffer,
        const Buffer& dstBuffer, const CopyRegion& copyRegion);

    void transferImageData(const EngineContext& instance, const Buffer& srcBuffer,
        Image& dstImage, size_t offset = 0, vk::Offset3D imageOffset = { 0, 0, 0 });

    void setRenderView(const EngineContext& instance, const RenderRegion& canvas);
    void draw(const EngineContext& instance,
        size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance);
    void drawIndexed(const EngineContext& instance,
        size_t indexCount, size_t instanceCount, size_t firstIndex, size_t indexIncrement, size_t firstInstance);

    void endRenderPass(const EngineContext& instance);
    void stopRecord(const EngineContext& instance);
    void reset(const EngineContext& instance);

    bool isValid() const {
        return m_isValid;
    }

    vk::CommandBuffer getCommandBuffer() const { return m_commandBuffer; };

	friend class CommandPool;
};

