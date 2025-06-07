#include "CommandBuffer.h"

void CommandBuffer::record(const EngineContext& instance, CommandBufferUsage::Flags flags)
{
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
	beginInfo.flags = flags; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional
	
	try {
		m_commandBuffer.begin(beginInfo, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to begin recording command buffer: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when beginning recording command buffer: " + std::string(e.what()));
	}
}

void CommandBuffer::beginRenderPass(const EngineContext& instance,
	const RenderPass& renderPass, const SwapChain& swapChain,
	uint32_t imageIndex, Color clearColor)
{
	vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
	renderPassInfo.renderPass = renderPass.getRenderPass();
	renderPassInfo.framebuffer = swapChain.getFrameBuffers()[imageIndex];
	renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
	renderPassInfo.renderArea.extent = swapChain.getActiveSwapChainFormat().getSwapChainExtent();

	vk::ClearValue clear = clearColor;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clear;
	try {
		m_commandBuffer.beginRenderPass(
			renderPassInfo, vk::SubpassContents::eInline, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to begin render pass: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when beginning render pass: " + std::string(e.what()));
	}
}

void CommandBuffer::beginRenderPass(const EngineContext& instance,
	const RenderPass& renderPass, const SwapChain& swapChain,
	uint32_t imageIndex, Color clearColor, float clearDepth)
{
	vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
	renderPassInfo.renderPass = renderPass.getRenderPass();
	renderPassInfo.framebuffer = swapChain.getFrameBuffers()[imageIndex];
	renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
	renderPassInfo.renderArea.extent = swapChain.getActiveSwapChainFormat().getSwapChainExtent();

	vk::ClearValue clears[] = { clearColor, {clearDepth} };

	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clears;
	try {
		m_commandBuffer.beginRenderPass(
			renderPassInfo, vk::SubpassContents::eInline, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to begin render pass: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when beginning render pass: " + std::string(e.what()));
	}
}

void CommandBuffer::bindGraphicsPipeline(const EngineContext& instance, const GraphicsPipeline& pipeline)
{
	try {
		m_commandBuffer.bindPipeline(
			vk::PipelineBindPoint::eGraphics, pipeline.getPipeline(), instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to bind graphics pipeline: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when binding graphics pipeline: " + std::string(e.what()));
	}
}

void CommandBuffer::bindIndexBuffer(const EngineContext& instance,
	const Buffer& buffer, vk::DeviceSize offset)
{
	try {
		m_commandBuffer.bindIndexBuffer(buffer.getBuffer(), offset, vk::IndexType::eUint32, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to bind index buffer: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when binding index buffer: " + std::string(e.what()));
	}
}

void CommandBuffer::bindDescriptorSets(const EngineContext& instance,
	const GraphicsPipeline& pipeline, const std::vector<DescriptorSetHandle>& descriptorSets,
	const std::vector<uint32_t>& dynamicOffsets /*= {}*/)
{
	auto descriptorSetsRaw = convert<vk::DescriptorSet>
		(descriptorSets, [](const DescriptorSetHandle& set)
			{ return set->getSet(); });

	m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
		pipeline.getLayout(), 0, descriptorSetsRaw, dynamicOffsets, instance.getDispatchLoader());
}

void CommandBuffer::setPipelineBarrier(const EngineContext& instance,
	vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
	Image& image, vk::ImageLayout newLayout,
	vk::AccessFlags srcAccess, vk::AccessFlags dstAccess)
{
	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout = image.getLayout();
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image.getImage();
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;

	m_commandBuffer.pipelineBarrier(
		srcStage,
		dstStage,
		{}, 0, nullptr, 0, nullptr, 1, &barrier);

	image.setLayout(newLayout);
}

void CommandBuffer::transferBufferData(const EngineContext& instance, const Buffer& srcBuffer,
	const Buffer& dstBuffer, const CopyRegion& copyRegion)
{
	try {
		m_commandBuffer.copyBuffer(srcBuffer.getBuffer(), dstBuffer.getBuffer(), 
			static_cast<vk::BufferCopy>(copyRegion), instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to transfer buffer data: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when transferring buffer data: " + std::string(e.what()));
	}
}

void CommandBuffer::transferImageData(const EngineContext& instance, const Buffer& srcBuffer,
	Image& dstImage, size_t offset /*= 0*/, vk::Offset3D imageOffset /*= { 0, 0, 0 }*/)
{
	vk::BufferImageCopy region{};
	region.bufferOffset = offset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = imageOffset;
	region.imageExtent = vk::Extent3D{ static_cast<uint32_t>(dstImage.getWidth()),
	static_cast<uint32_t>(dstImage.getHeight()), 1 };

	try {
		m_commandBuffer.copyBufferToImage(srcBuffer.getBuffer(), dstImage.getImage(),
			dstImage.getLayout(), 1, &region, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to transfer buffer data: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when transferring buffer data: " + std::string(e.what()));
	}
}

void CommandBuffer::setRenderView(const EngineContext& instance, const RenderRegion& canvas)
{

	try {
		m_commandBuffer.setViewport(0, canvas.viewport, instance.getDispatchLoader());
		m_commandBuffer.setScissor(0, canvas.scissor, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to set render view: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when setting render view: " + std::string(e.what()));
	}
}

void CommandBuffer::draw(const EngineContext& instance,
	size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
{
	try {
		m_commandBuffer.draw(3, 1, 0, 0, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to record draw command: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error recording draw command: " + std::string(e.what()));
	}
}

void CommandBuffer::drawIndexed(const EngineContext& instance,
	size_t indexCount, size_t instanceCount, size_t firstIndex, size_t indexIncrement, size_t firstInstance)
{
	try {
		m_commandBuffer.drawIndexed(
			indexCount, instanceCount, firstIndex, indexIncrement, firstInstance, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to record draw indexed command: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error recording draw indexed command: " + std::string(e.what()));
	}
}

void CommandBuffer::endRenderPass(const EngineContext& instance)
{
	try {
		m_commandBuffer.endRenderPass(instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to end render pass: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error ending render pass: " + std::string(e.what()));
	}
}

void CommandBuffer::stopRecord(const EngineContext& instance)
{
	try {
		m_commandBuffer.end(instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to end command buffer record: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error ending command buffer record: " + std::string(e.what()));
	}
}

void CommandBuffer::reset(const EngineContext& instance)
{
	try {
		m_commandBuffer.reset(vk::CommandBufferResetFlagBits(), instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to end command buffer record: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error ending command buffer record: " + std::string(e.what()));
	}
}

//CommandBuffer allocateBuffer(const EngineContext& instance, const Device& device,
//	vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) {
//	vk::CommandBufferAllocateInfo allocInfo{};
//	allocInfo.setCommandPool(m_pool)
//		.setLevel(level)
//		.setCommandBufferCount(1);
//
//	vk::CommandBuffer cmdBuffer;
//	vk::Result result = device.getDevice().allocateCommandBuffers(&allocInfo, &cmdBuffer, instance.getDispatchLoader());
//	if (result != vk::Result::eSuccess)
//		throw std::runtime_error("Failed to allocate command buffer");
//	uint32_t id;
//	if (m_recycledIds.empty())
//		id = m_nextId++;
//	else
//	{
//		id = m_recycledIds.top();
//		m_recycledIds.pop();
//	}
//	CommandBuffer buffer(cmdBuffer, this, id);
//	m_allocatedBufferIds.emplace(id);
//	return buffer;
//}
//
//void freeBuffer(const EngineContext& instance, const Device& device, CommandBuffer& buffer) {
//
//	if (buffer.m_pool != this || buffer.m_pool == nullptr)
//		throw std::runtime_error("Buffer is deallocated or doesn't belong to this pool");
//	m_allocatedBufferIds.erase(buffer.m_bufferId);
//	m_recycledIds.push(buffer.m_bufferId);
//	device.getDevice().freeCommandBuffers(m_pool, 1, &buffer.m_CommandBuffer, instance.getDispatchLoader());
//	buffer.m_pool = nullptr;
//	buffer.m_bufferId = 0;
//	buffer.m_CommandBuffer = nullptr;
//}
//
//vk::CommandBuffer m_CommandBuffer;
//CommandPool* m_pool = nullptr; //used to indicated ownership of the command buffer and if its allocated
//uint32_t m_bufferId = 0;
//
//CommandBuffer(vk::CommandBuffer buffer, CommandPool* pool, uint32_t bufferId) :
//	m_CommandBuffer(buffer), m_pool(pool), m_bufferId(bufferId) {
//};