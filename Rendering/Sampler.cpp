#include "Sampler.h"

Sampler::Sampler(const EngineContext& instance, const EngineDevice& device, const Descriptor& descriptor) :
	m_descriptor(descriptor)
{
	try {
		m_sampler = device.getDevice().createSampler(descriptor, nullptr, instance.getDispatchLoader());
	}
	catch (const vk::SystemError& e) {
		throw std::runtime_error("failed to create a sampler: " + std::string(e.what()));
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Unexpected error when creating a sampler: " + std::string(e.what()));
	}

	m_initialized = true;
}