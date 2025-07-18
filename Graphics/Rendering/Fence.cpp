#include "Fence.h"

namespace Graphics {

	Fence::Fence(const Context& instance, const Device& device, bool createSignaled/* = false*/)
	{
		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
		if (createSignaled)
			fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		try {
			m_fence = device.getDevice().createFence(fenceInfo, nullptr, instance.getDispatchLoader());
		}
		catch (const vk::SystemError& e) {
			throw std::runtime_error("failed to create a fence: " + std::string(e.what()));
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Unexpected error when creating a fence: " + std::string(e.what()));
		}

		m_initialized = true;
	}

	void Fence::wait(const Context& instance, const Device& device)
	{
		vk::Result result = device.getDevice().waitForFences(m_fence, VK_TRUE, UINT64_MAX, instance.getDispatchLoader());
		if (result != vk::Result::eSuccess)
			throw std::runtime_error("Error waiting for a fence");
	}

	void Fence::reset(const Context& instance, const Device& device)
	{
		device.getDevice().resetFences(m_fence, instance.getDispatchLoader());
	}
}