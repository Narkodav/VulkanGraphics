#include "StagingBuffer.h"

StagingBuffer::StagingBuffer(const EngineContext& instance, const EngineDevice& device,
    size_t byteSize, bool allowConcurrentAccess = false) :
    Buffer(instance, device, byteSize, BufferUsageFlagBits::TRANSFER_SRC, allowConcurrentAccess),
    capacity(byteSize)
{




}