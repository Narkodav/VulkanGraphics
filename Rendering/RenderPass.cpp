#include "RenderPass.h"

RenderPass::RenderPass(const EngineContext& instance, const Device& device, const SwapChainFormat& format)
{
    std::vector<vk::AttachmentDescription> attachments;
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = format.getSurfaceFormat().format;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;

    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    attachments.push_back(colorAttachment);
    vk::AttachmentReference depthAttachmentRef{};

    if (format.getDepthFormat() != vk::Format::eUndefined)
    {
        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = format.getDepthFormat();
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        dependency.srcStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        attachments.push_back(depthAttachment);
    }

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (device.getDevice().createRenderPass(&renderPassInfo, nullptr, &m_renderPass, instance.getDispatchLoader()) 
    != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create render pass!");
    }

    m_initialized = true;
}