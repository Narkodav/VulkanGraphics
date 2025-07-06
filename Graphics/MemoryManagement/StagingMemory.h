#pragma once
#include "Common.h"
#include "EngineContext.h"
#include "EngineDevice.h"
#include "StagingBuffer.h"
#include "Memory.h"

class StagingMemory : public Memory
{
	size_t m_capacity;

public:
	StagingMemory(const EngineContext& context, const EngineDevice& device, const StagingBuffer& buffer, size_t capacity) :
		Memory(context, device, static_cast<const Buffer&>(buffer), MemoryPropertyFlagBits::HOST_VISIBLE_COHERENT),
		m_capacity(capacity)
	{



	}

	virtual bool bindBuffer(const EngineContext& instance, const EngineDevice& device, const StagingBuffer& buffer)
	{

	}

};

