#include "Semaphore.h"

namespace Graphics {

	Semaphore::Semaphore(const Context& instance, const Device& device)
	{
		vk::SemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

		try {
			m_semaphore = device.getDevice().createSemaphore(semaphoreInfo, nullptr, instance.getDispatchLoader());
		}
		catch (const vk::SystemError& e) {
			throw std::runtime_error("failed to create a semaphore: " + std::string(e.what()));
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Unexpected error when creating a semaphore: " + std::string(e.what()));
		}

		m_initialized = true;
	}

}