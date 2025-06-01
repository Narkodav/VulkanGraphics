#pragma once
#include "Common.h"
#include "EngineContext.h"
#include "EngineDevice.h"
#include "Buffer.h"
#include "Memory.h"

class StagingBuffer : public Buffer
{
private:
    size_t capacity = 0;

public:
    StagingBuffer() {};
    StagingBuffer(const EngineContext& instance, const EngineDevice& device,
        size_t byteSize, bool allowConcurrentAccess = false);

    void map(const EngineContext& instance, const EngineDevice& device, size_t byteSize);
    void unmap(const EngineContext& instance, const EngineDevice& device);

    void* getMappedMemory() const { return m_mapped ? device.getDevice().mapMemory(m_buffer, 0, VK_WHOLE_SIZE, {}) : nullptr; };

    void flush(const EngineContext& instance, const EngineDevice& device, size_t byteSize);
};

