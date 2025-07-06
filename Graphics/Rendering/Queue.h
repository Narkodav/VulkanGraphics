#pragma once
#include "../Common.h"
#include "GraphicsContext.h"
#include "Device.h"
#include "SwapChainFormat.h"
#include "Semaphore.h"
#include "Fence.h"
#include "CommandBuffer.h"
#include "CommandPool.h"

class Queue
{
private:
    vk::Queue m_queue = nullptr;
    uint32_t m_family = 0;
    uint32_t m_index = 0;

    bool m_initialized = false;
public:

    Queue() {};
    Queue(const GraphicsContext& instance, const Device& device,
        uint32_t familyIndex, uint32_t queueIndex);

    Queue(Queue&&) noexcept = default;
    Queue& operator=(Queue&&) noexcept = default;

    Queue(const Queue&) noexcept = default;
    Queue& operator=(const Queue&) noexcept = default;

    uint32_t getFamily() const { return m_family; }
    uint32_t getIndex() const { return m_index; }

    const vk::Queue& getQueue() const { return m_queue; };

    void waitIdle(const GraphicsContext& instance) const { m_queue.waitIdle(instance.getDispatchLoader()); };

    void submit(const GraphicsContext& instance,
        const std::vector<vk::PipelineStageFlags>& waitStages,
        const std::vector<std::reference_wrapper<const Semaphore>>& waitSenaphores,
        const std::vector<std::reference_wrapper<const CommandBufferHandle>>& commandBuffers,
        const std::vector<std::reference_wrapper<const Semaphore>>& signalSemaphores,
        const Fence& fence) const;
    
    void submit(const GraphicsContext& instance,
        const std::vector<vk::PipelineStageFlags>& waitStages,
        const std::vector<std::reference_wrapper<const Semaphore>>& waitSenaphores,
        const std::vector<std::reference_wrapper<const CommandBufferHandle>>& commandBuffers,
        const std::vector<std::reference_wrapper<const Semaphore>>& signalSemaphores) const;

    template<size_t WaitSemaphoresSize, size_t CommandBuffersSize, size_t SignalSemaphoresSize>
    void submit(
        const GraphicsContext& instance,
        std::array<vk::PipelineStageFlags, WaitSemaphoresSize> const& waitStages,
        std::array<std::reference_wrapper<Semaphore>, WaitSemaphoresSize> const& waitSemaphores,
        std::array<std::reference_wrapper<CommandBufferHandle>, CommandBuffersSize> const& commandBuffers,
        std::array<std::reference_wrapper<Semaphore>, SignalSemaphoresSize> const& signalSemaphores,
        const Fence& fence) const
    {

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;

        auto waitSemaphoresRaw = convert<vk::Semaphore>
            (waitSemaphores, [](std::reference_wrapper<Semaphore> semaphore)
                { return semaphore.get().getSemaphore(); });

        auto commandBuffersRaw = convert<vk::CommandBuffer>
            (commandBuffers, [](std::reference_wrapper<CommandBufferHandle> commandBuffer)
                { return commandBuffer.get()->getCommandBuffer(); });

        auto signalSemaphoresRaw = convert<vk::Semaphore>
            (signalSemaphores, [](std::reference_wrapper<Semaphore> semaphore)
                { return semaphore.get().getSemaphore(); });

        submitInfo.waitSemaphoreCount = waitSemaphoresRaw.size();
        submitInfo.pWaitSemaphores = waitSemaphoresRaw.data();
        submitInfo.pWaitDstStageMask = waitStages.data();

        submitInfo.commandBufferCount = commandBuffersRaw.size();
        submitInfo.pCommandBuffers = commandBuffersRaw.data();

        submitInfo.signalSemaphoreCount = signalSemaphoresRaw.size();
        submitInfo.pSignalSemaphores = signalSemaphoresRaw.data();

        try {
            m_queue.submit(submitInfo, fence.getFence(), instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
        }
    }

    template<size_t CommandBuffersSize>
    void submit(
        const GraphicsContext& instance,
        std::array<std::reference_wrapper<CommandBufferHandle>, CommandBuffersSize> const& commandBuffers) const
    {

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;

        auto commandBuffersRaw = convert<vk::CommandBuffer>
            (commandBuffers, [](std::reference_wrapper<CommandBufferHandle> commandBuffer)
                { return commandBuffer.get()->getCommandBuffer(); });

        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;

        submitInfo.commandBufferCount = commandBuffersRaw.size();
        submitInfo.pCommandBuffers = commandBuffersRaw.data();

        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        try {
            m_queue.submit(submitInfo, nullptr, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
        }
    }

    template<size_t SwapChainSize, size_t SemaphoresSize>
    bool present(
        const GraphicsContext& instance,
        std::array<std::reference_wrapper<Semaphore>, SemaphoresSize> const& waitSemaphores,
        std::array<std::reference_wrapper<SwapChain>, SwapChainSize> const& swapChains,
        std::array<uint32_t, SwapChainSize>const& imageIndices) const
    {
        auto semapforesRaw = convert<vk::Semaphore>
            (waitSemaphores, [](std::reference_wrapper<Semaphore> semaphore)
                { return semaphore.get().getSemaphore(); });

        auto swapchainsRaw = convert<vk::SwapchainKHR>
            (swapChains, [](std::reference_wrapper<SwapChain> chain)
                { return chain.get().getSwapChain(); });

        vk::PresentInfoKHR presentInfo{};
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;

        presentInfo.waitSemaphoreCount = semapforesRaw.size();
        presentInfo.pWaitSemaphores = semapforesRaw.data();

        presentInfo.swapchainCount = swapchainsRaw.size();
        presentInfo.pSwapchains = swapchainsRaw.data();
        presentInfo.pImageIndices = imageIndices.data();
        presentInfo.pResults = nullptr;

        try {
            vk::Result result = m_queue.presentKHR(presentInfo, instance.getDispatchLoader());
            if (result != vk::Result::eSuccess)
                throw std::runtime_error("failed to present: " + vk::to_string(result));
        }
        catch (const vk::SystemError& e) {
            if (e.code() == vk::Result::eErrorOutOfDateKHR) {
                return true;
            }
            else if (e.code() == vk::Result::eSuboptimalKHR)
                return true;
            throw std::runtime_error("failed to present: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            //if (e.code == vk::Result::eSuboptimalKHR || e.code == vk::Result::eErrorOutOfDateKHR)
            //    return false;
            throw std::runtime_error("Unexpected error when presenting: " + std::string(e.what()));
        }
        return true;
    }


    template<size_t SwapChainSize>
    void present(
        const GraphicsContext& instance,
        std::array<std::reference_wrapper<SwapChain>, SwapChainSize> const& swapChains,
        std::array<uint32_t, SwapChainSize>const& imageIndices) const
    {
        auto swapchainsRaw = convert<vk::SwapchainKHR>
            (swapChains, [](std::reference_wrapper<SwapChain> chain)
                { return chain.get().getSwapChain(); });

        vk::PresentInfoKHR presentInfo{};
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;

        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = nullptr;

        presentInfo.swapchainCount = swapchainsRaw.size();
        presentInfo.pSwapchains = swapchainsRaw.data();
        presentInfo.pImageIndices = imageIndices.data();
        presentInfo.pResults = nullptr;

        try {
            vk::Result result = m_queue.presentKHR(presentInfo, instance.getDispatchLoader());
            if (result != vk::Result::eSuccess)
                throw std::runtime_error("failed to present: " + vk::to_string(result));
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to present: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error when presenting: " + std::string(e.what()));
        }
    }
};

