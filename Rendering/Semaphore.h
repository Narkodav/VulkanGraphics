#pragma once
#include "Common.h"
#include "EngineContext.h"
#include "EngineDevice.h"

class Semaphore
{
private:
	vk::Semaphore m_semaphore;

	bool m_initialized = false;
public:
	
    Semaphore() {};
    Semaphore(const EngineContext& instance, const EngineDevice& device);

    Semaphore(Semaphore&& other) noexcept {

        m_semaphore = std::exchange(other.m_semaphore, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);

    };

    //moving to an initialized device is undefined behavior, destroy before moving
    Semaphore& operator=(Semaphore&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "Semaphore::operator=() - Semaphore already initialized");

        m_semaphore = std::exchange(other.m_semaphore, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    Semaphore(const Semaphore&) noexcept = delete;
    Semaphore& operator=(const Semaphore&) noexcept = delete;

    ~Semaphore() { assert(!m_initialized && "Semaphore was not destroyed!"); };

    void destroy(const EngineContext& instance, const EngineDevice& device) {
        if (!m_initialized)
            return;

        device.getDevice().destroySemaphore(m_semaphore, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
        std::cout << "Destroyed semaphore" << std::endl;
#endif
        m_initialized = false;
    }

    bool isInitialized() const { return m_initialized; };

    const vk::Semaphore& getSemaphore() const { return m_semaphore; };

};

