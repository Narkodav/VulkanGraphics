#pragma once
#pragma once
#include "Common.h"
#include "Rendering/EngineContext.h"
#include "Rendering/EngineDevice.h"
#include "PlatformManagement/Window.h"
#include "DescriptorSet.h"
#include "Rendering/GraphicsPipeline.h"

using DescriptorSetHandle = std::shared_ptr<DescriptorSet>;

class DescriptorPool
{
public:
    class Size
    {
    private:
        vk::DescriptorPoolSize m_poolSize{};

    public:
        Size(size_t count, DescriptorType type)
        {
            m_poolSize.type = static_cast<vk::DescriptorType>(type);
            m_poolSize.descriptorCount = static_cast<uint32_t>(count);
        }

        operator vk::DescriptorPoolSize() const { return m_poolSize; };
        
    };

private:
    vk::DescriptorPool m_pool;
    std::set<DescriptorSetHandle> m_allocatedSets;
    std::vector<vk::DescriptorPoolSize> m_sizes;

    bool m_initialized = false;
public:
    DescriptorPool() : m_pool(nullptr), m_initialized(false) {};

    DescriptorPool(const EngineContext& instance, const EngineDevice& device,
        const std::vector<Size>& sizes, size_t maxSets);

    DescriptorPool(DescriptorPool&& other) noexcept {
        m_pool = std::exchange(other.m_pool, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);
    };

    //moving to an initialized swap chain is undefined behavior, destroy before moving
    DescriptorPool& operator=(DescriptorPool&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "DescriptorPool::operator=() - DescriptorPool already initialized");

        m_pool = std::exchange(other.m_pool, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    DescriptorPool(const DescriptorPool&) noexcept = delete;
    DescriptorPool& operator=(const DescriptorPool&) noexcept = delete;

    ~DescriptorPool() {
        assert(!m_initialized && "DescriptorPool was not destroyed!");
        //assert(m_allocatedBuffers.size() == 0 && "DescriptorPool has allocated buffers!");
    };

    void destroy(const EngineContext& instance, const EngineDevice& device) {
        if (!m_initialized)
            return;
        //assert(m_allocatedBuffers.size() == 0 && "DescriptorPool has allocated buffers!");

        for (auto& set : m_allocatedSets)
        {
            set->m_set = nullptr;
            set->m_initialized = false;
        }
        device.getDevice().destroyDescriptorPool(m_pool, nullptr, instance.getDispatchLoader());

#ifdef _DEBUG
        std::cout << "Destroyed DescriptorPool" << std::endl;
#endif
        m_initialized = false;
    };

    std::vector<DescriptorSetHandle> allocateSets(
        const EngineContext& instance, const EngineDevice& device,
        const GraphicsPipeline& pipeline, size_t setAmount) {

        std::vector<vk::DescriptorSetLayout> layouts(setAmount, pipeline.getDescriptorSetLayout());
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
        allocInfo.descriptorPool = m_pool;
        allocInfo.descriptorSetCount = layouts.size();
        allocInfo.pSetLayouts = layouts.data();

        auto setsRaw = device.getDevice().allocateDescriptorSets(allocInfo, instance.getDispatchLoader());
        auto setsWrapped = convert<DescriptorSetHandle>
            (setsRaw, [this, &pipeline](vk::DescriptorSet set)
                { 
                    auto setWrapped = std::make_shared<DescriptorSet>(
                        DescriptorSet(set, pipeline.getDescriptorSetLayoutCreateInfo()));
                    m_allocatedSets.insert(setWrapped);
                    return setWrapped;
                });
        
        return setsWrapped;
    }

    //void freeBuffer(const EngineContext& instance, const EngineDevice& device, CommandBufferHandle& buffer) {

    //    if (m_allocatedBuffers.find(buffer) == m_allocatedBuffers.end())
    //        throw std::runtime_error("Buffer is deallocated or doesn't belong to this pool");
    //    m_allocatedBuffers.erase(buffer);
    //    device.getDevice().freeCommandBuffers(
    //        m_pool, 1, &buffer.get()->m_commandBuffer, instance.getDispatchLoader());
    //    buffer.get()->m_commandBuffer = nullptr;
    //    buffer.get()->m_isValid = false;
    //}

    //// Reset the entire pool (faster than freeing individual buffers)
    //void reset(const EngineContext& instance, const EngineDevice& device) {
    //    device.getDevice().resetCommandPool(m_pool, vk::CommandPoolResetFlags(), instance.getDispatchLoader());

    //    for (auto& buffer : m_allocatedBuffers)
    //    {
    //        buffer.get()->m_commandBuffer = nullptr;
    //        buffer.get()->m_isValid = false;
    //    }

    //    m_allocatedBuffers.clear();
    //}

    const vk::DescriptorPool& getPool() const { return m_pool; };
};


