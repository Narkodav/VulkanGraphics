#include "Queue.h"

namespace Graphics {

    Queue::Queue(const Context& instance, const Device& device,
        uint32_t familyIndex, uint32_t queueIndex)
    {
        m_queue = device.getDevice().getQueue(familyIndex, queueIndex);
        m_family = familyIndex;
        m_index = queueIndex;
        m_initialized = true;
    }

    void Queue::submit(const Context& instance,
        const std::vector<vk::PipelineStageFlags>& waitStages,
        const std::vector<std::reference_wrapper<const Semaphore>>& waitSenaphores,
        const std::vector<std::reference_wrapper<const CommandBufferHandle>>& commandBuffers,
        const std::vector<std::reference_wrapper<const Semaphore>>& signalSemaphores,
        const Fence& fence) const
    {
        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;

        std::vector<vk::Semaphore> waitSemaphoresRaw;
        waitSemaphoresRaw.reserve(waitSenaphores.size());

        std::vector<vk::CommandBuffer> commandBuffersRaw;
        commandBuffersRaw.reserve(commandBuffers.size());

        std::vector<vk::Semaphore> signalSemaphoresRaw;
        signalSemaphoresRaw.reserve(signalSemaphores.size());

        for (int i = 0; i < waitSenaphores.size(); ++i)
            waitSemaphoresRaw.push_back(waitSenaphores[i].get().getSemaphore());
        for (int i = 0; i < commandBuffers.size(); ++i)
            commandBuffersRaw.push_back(commandBuffers[i].get()->getCommandBuffer());
        for (int i = 0; i < signalSemaphores.size(); ++i)
            signalSemaphoresRaw.push_back(signalSemaphores[i].get().getSemaphore());

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

    void Queue::submit(const Context& instance,
        const std::vector<vk::PipelineStageFlags>& waitStages,
        const std::vector<std::reference_wrapper<const Semaphore>>& waitSenaphores,
        const std::vector<std::reference_wrapper<const CommandBufferHandle>>& commandBuffers,
        const std::vector<std::reference_wrapper<const Semaphore>>& signalSemaphores) const
    {
        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;

        std::vector<vk::Semaphore> waitSemaphoresRaw;
        waitSemaphoresRaw.reserve(waitSenaphores.size());

        std::vector<vk::CommandBuffer> commandBuffersRaw;
        commandBuffersRaw.reserve(commandBuffers.size());

        std::vector<vk::Semaphore> signalSemaphoresRaw;
        signalSemaphoresRaw.reserve(signalSemaphores.size());

        for (int i = 0; i < waitSenaphores.size(); ++i)
            waitSemaphoresRaw.push_back(waitSenaphores[i].get().getSemaphore());
        for (int i = 0; i < commandBuffers.size(); ++i)
            commandBuffersRaw.push_back(commandBuffers[i].get()->getCommandBuffer());
        for (int i = 0; i < signalSemaphores.size(); ++i)
            signalSemaphoresRaw.push_back(signalSemaphores[i].get().getSemaphore());

        submitInfo.waitSemaphoreCount = waitSemaphoresRaw.size();
        submitInfo.pWaitSemaphores = waitSemaphoresRaw.data();
        submitInfo.pWaitDstStageMask = waitStages.data();

        submitInfo.commandBufferCount = commandBuffersRaw.size();
        submitInfo.pCommandBuffers = commandBuffersRaw.data();

        submitInfo.signalSemaphoreCount = signalSemaphoresRaw.size();
        submitInfo.pSignalSemaphores = signalSemaphoresRaw.data();

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

    //template<typename... SemaphoresWait, typename... CmdBuffers>
    //void Queue::submit(
    //    const Context& instance,
    //    const std::array<vk::PipelineStageFlags, sizeof...(SemaphoresWait)>& waitStages,
    //    const SemaphoresWait&... waitSemaphores,
    //    const CmdBuffers&... commandBuffers,
    //    const Fence& fence) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //    std::array<vk::Semaphore, sizeof...(SemaphoresWait)> waitSemaphoresRaw{
    //        waitSemaphores.getSemaphore()...
    //    };
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = waitSemaphoresRaw.size();
    //    submitInfo.pWaitSemaphores = waitSemaphoresRaw.data();
    //    submitInfo.pWaitDstStageMask = waitStages.data();
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = 0;
    //    submitInfo.pSignalSemaphores = nullptr;
    //
    //    try {
    //        m_queue.submit(submitInfo, fence.getFence(), instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}
    //
    //template<typename... CmdBuffers, typename... SemaphoresSignal>
    //void Queue::submit(
    //    const Context& instance,
    //    const CmdBuffers&... commandBuffers,
    //    const SemaphoresSignal&... signalSemaphores,
    //    const Fence& fence) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //    std::array<vk::Semaphore, sizeof...(SemaphoresSignal)> signalSemaphoresRaw{
    //        signalSemaphores.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = 0;
    //    submitInfo.pWaitSemaphores = nullptr;
    //    submitInfo.pWaitDstStageMask = nullptr;
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = signalSemaphoresRaw.size();
    //    submitInfo.pSignalSemaphores = signalSemaphoresRaw.data();
    //
    //    try {
    //        m_queue.submit(submitInfo, fence.getFence(), instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}
    //
    //template<typename... CmdBuffers>
    //void Queue::submit(
    //    const Context& instance,
    //    const CmdBuffers&... commandBuffers,
    //    const Fence& fence) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = 0;
    //    submitInfo.pWaitSemaphores = nullptr;
    //    submitInfo.pWaitDstStageMask = nullptr;
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = 0;
    //    submitInfo.pSignalSemaphores = nullptr;
    //
    //    try {
    //        m_queue.submit(submitInfo, fence.getFence(), instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}
    //
    //template<typename... SemaphoresWait, typename... CmdBuffers, typename... SemaphoresSignal>
    //void Queue::submit(
    //    const Context& instance,
    //    const std::array<vk::PipelineStageFlags, sizeof...(SemaphoresWait)>& waitStages,
    //    const SemaphoresWait&... waitSemaphores,
    //    const CmdBuffers&... commandBuffers,
    //    const SemaphoresSignal&... signalSemaphores) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //    std::array<vk::Semaphore, sizeof...(SemaphoresWait)> waitSemaphoresRaw{
    //        waitSemaphores.getSemaphore()...
    //    };
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //    std::array<vk::Semaphore, sizeof...(SemaphoresSignal)> signalSemaphoresRaw{
    //    signalSemaphores.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = waitSemaphoresRaw.size();
    //    submitInfo.pWaitSemaphores = waitSemaphoresRaw.data();
    //    submitInfo.pWaitDstStageMask = waitStages.data();
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = signalSemaphoresRaw.size();
    //    submitInfo.pSignalSemaphores = signalSemaphoresRaw.data();
    //
    //    try {
    //        m_queue.submit(submitInfo, nullptr, instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}
    //
    //template<typename... SemaphoresWait, typename... CmdBuffers>
    //void Queue::submit(
    //    const Context& instance,
    //    const std::array<vk::PipelineStageFlags, sizeof...(SemaphoresWait)>& waitStages,
    //    const SemaphoresWait&... waitSemaphores,
    //    const CmdBuffers&... commandBuffers) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //    std::array<vk::Semaphore, sizeof...(SemaphoresWait)> waitSemaphoresRaw{
    //        waitSemaphores.getSemaphore()...
    //    };
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = waitSemaphoresRaw.size();
    //    submitInfo.pWaitSemaphores = waitSemaphoresRaw.data();
    //    submitInfo.pWaitDstStageMask = waitStages.data();
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = 0;
    //    submitInfo.pSignalSemaphores = nullptr;
    //
    //    try {
    //        m_queue.submit(submitInfo, nullptr, instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}
    //
    //template<typename... CmdBuffers, typename... SemaphoresSignal>
    //void Queue::submit(
    //    const Context& instance,
    //    const CmdBuffers&... commandBuffers,
    //    const SemaphoresSignal&... signalSemaphores) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //    std::array<vk::Semaphore, sizeof...(SemaphoresSignal)> signalSemaphoresRaw{
    //    signalSemaphores.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = 0;
    //    submitInfo.pWaitSemaphores = nullptr;
    //    submitInfo.pWaitDstStageMask = nullptr;
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = signalSemaphoresRaw.size();
    //    submitInfo.pSignalSemaphores = signalSemaphoresRaw.data();
    //
    //    try {
    //        m_queue.submit(submitInfo, nullptr, instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}
    //
    //template<typename... CmdBuffers>
    //void Queue::submit(
    //    const Context& instance,
    //    const CmdBuffers&... commandBuffers) const
    //{
    //    vk::SubmitInfo submitInfo{};
    //    submitInfo.sType = vk::StructureType::eSubmitInfo;
    //
    //    std::array<vk::CommandBuffer, sizeof...(CmdBuffers)> commandBuffersRaw{
    //        commandBuffers.getCommandBuffer()...
    //    };
    //
    //    submitInfo.waitSemaphoreCount = 0;
    //    submitInfo.pWaitSemaphores = nullptr;
    //    submitInfo.pWaitDstStageMask = nullptr;
    //
    //    submitInfo.commandBufferCount = commandBuffersRaw.size();
    //    submitInfo.pCommandBuffers = commandBuffersRaw.data();
    //
    //    submitInfo.signalSemaphoreCount = 0;
    //    submitInfo.pSignalSemaphores = nullptr;
    //
    //    try {
    //        m_queue.submit(submitInfo, nullptr, instance.getDispatchLoader());
    //    }
    //    catch (const vk::SystemError& e) {
    //        throw std::runtime_error("failed to submit command buffers: " + std::string(e.what()));
    //    }
    //    catch (const std::exception& e) {
    //        throw std::runtime_error("Unexpected error when submitting command buffers: " + std::string(e.what()));
    //    }
    //}

}