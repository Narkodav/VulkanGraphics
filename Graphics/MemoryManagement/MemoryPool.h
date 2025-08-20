#pragma once
#include "../Common.h"
#include "../Rendering/Flags.h"
#include "../Rendering/Device.h"
#include "../Rendering/Context.h"
#include "Buffer.h"
#include "Memory.h"

#include <vector>
#include <list>

namespace Graphics
{
	template<typename MemoryType>
	class MemoryPool
	{
	public:
		struct MemoryRegion {
			uint32_t offset;
			uint32_t size;
		};

		struct Allocation {
			MemoryRegion region;
			uint32_t bufferIndex;

			struct Hash
			{
				constexpr size_t operator()(const Allocation& allocation) const {
					return std::hash<uint32_t>()(allocation.bufferIndex) ^ std::hash<uint32_t>()(allocation.region.offset);
				}
			};

			struct EqualTo
			{
				constexpr bool operator()(const Allocation& left, const Allocation& right) const {
					return left.region.offset == right.region.offset
					&& left.region.size == right.region.size
					&& left.bufferIndex == right.bufferIndex;
				}
			};

			static Allocation getEmptyAllocation() {
				return Allocation{ MemoryRegion{ 0, 0 }, 0 };
			};
		};

		using AllocationSet = std::unordered_set<Allocation, typename Allocation::Hash, typename Allocation::EqualTo>;

	private:
		std::vector<Buffer> m_buffers;
		std::vector<MemoryType> m_memories;

		std::vector<std::list<MemoryRegion>> m_freeRegions;
		std::vector<size_t> m_allocatedSizes;
		std::vector<AllocationSet> m_allocations;

		uint32_t m_chunkCapacity = 0;
		BufferUsage::Flags m_usageFlags;
		MemoryProperty::Flags m_memoryProperties;
		bool m_allowConcurrentAccess = false;
		bool m_initialized = false;

	public:

		MemoryPool() = default;

		MemoryPool(const Context& instance, const Device& device,
			size_t memoryChunkCapacity, BufferUsage::Flags usageFlags,
			MemoryProperty::Flags memoryProperties,
			bool allowConcurrentAccess = false)
		{
			m_chunkCapacity = memoryChunkCapacity;
			m_usageFlags = usageFlags;
			m_memoryProperties = memoryProperties;
			m_allowConcurrentAccess = allowConcurrentAccess;
			m_allocatedSizes = std::vector<size_t>();
			m_allocations = std::vector<AllocationSet>();
			addBuffer(instance, device);
			m_initialized = true;
		}

		MemoryPool(MemoryPool&& other) noexcept
		{
			m_buffers = std::exchange(other.m_buffers, std::vector<Buffer>());
			m_memories = std::exchange(other.m_memories, std::vector<MemoryType>());

			m_freeRegions = std::exchange(other.m_freeRegions, std::vector<std::list<MemoryRegion>>());
			m_allocations = std::exchange(other.m_allocations, std::vector<AllocationSet>());
			m_allocatedSizes = std::exchange(other.m_allocatedSizes, std::vector<size_t>());

			m_chunkCapacity = std::exchange(other.m_chunkCapacity, 0);
			m_usageFlags = std::exchange(other.m_usageFlags, 0);
			m_memoryProperties = std::exchange(other.m_memoryProperties, 0);
			m_allowConcurrentAccess = std::exchange(other.m_allowConcurrentAccess, 0);
			m_initialized = std::exchange(other.m_initialized, 0);
		}

		MemoryPool& operator=(MemoryPool&& other)
		{
			if (this == &other)
				return *this;

			assert(!m_initialized && "MemoryPool::operator=() - MemoryPool already initialized");

			m_buffers = std::exchange(other.m_buffers, std::vector<Buffer>());
			m_memories = std::exchange(other.m_memories, std::vector<MemoryType>());

			m_freeRegions = std::exchange(other.m_freeRegions, std::vector<std::list<MemoryRegion>>());
			m_allocations = std::exchange(other.m_allocations, std::vector<AllocationSet>());
			m_allocatedSizes = std::exchange(other.m_allocatedSizes, std::vector<size_t>());

			m_chunkCapacity = std::exchange(other.m_chunkCapacity, 0);
			m_usageFlags = std::exchange(other.m_usageFlags, 0);
			m_memoryProperties = std::exchange(other.m_memoryProperties, 0);
			m_allowConcurrentAccess = std::exchange(other.m_allowConcurrentAccess, 0);
			m_initialized = std::exchange(other.m_initialized, 0);

			return *this;
		}

		MemoryPool(const MemoryPool&) noexcept = delete;
		MemoryPool& operator=(const MemoryPool&) noexcept = delete;

		~MemoryPool() { assert(!m_initialized && "MemoryPool was not destroyed!"); };

		Allocation allocate(
			const Context& instance, const Device& device, size_t size)
		{
			size_t rst = size % m_buffers[0].getMemoryRequirements().alignment;
			if (rst != 0)
				size += m_buffers[0].getMemoryRequirements().alignment - rst;

			if (size > m_chunkCapacity)
				throw std::runtime_error("Allocation size exceeds chunk capacity");

			for (uint32_t i = 0; i < m_buffers.size(); ++i)
			{
				MemoryRegion region = getRegionBestFit(i, size);
				if (region.size != 0)
				{
					m_allocatedSizes[i] += region.size;
					Allocation all = Allocation{ region, i };
					m_allocations[i].insert(all);
					return all;
				}
			}
			addBuffer(instance, device);
			MemoryRegion region = getRegionBestFit(m_buffers.size() - 1, size);
			Allocation all = Allocation{ region, static_cast<uint32_t>(m_buffers.size() - 1) };
			m_allocations.back().insert(all);
			m_allocatedSizes.back() += region.size;
			return all;
		}

		void free(const Allocation& allocation)
		{
			if (allocation.region.size == 0)
				return;
			auto it = m_allocations[allocation.bufferIndex].find(allocation);
			if (it == m_allocations[allocation.bufferIndex].end() || it->region.size != allocation.region.size)
				throw std::runtime_error("Can't free an invalid allocation");			
			freeRegion(allocation.region, allocation.bufferIndex);
			m_allocatedSizes[allocation.bufferIndex] -= allocation.region.size;
			m_allocations[allocation.bufferIndex].erase(it);
		}

		void shrink(Allocation& allocation, size_t newSize)
		{
			if (newSize == 0)
			{
				free(allocation);
				allocation = Allocation::getEmptyAllocation();
				return;
			}

			auto it = m_allocations[allocation.bufferIndex].find(allocation);
			if (it == m_allocations[allocation.bufferIndex].end() || it->region.size != allocation.region.size)
				throw std::runtime_error("Can't free an invalid allocation");

			if (allocation.region.size < newSize)
				throw std::runtime_error("Can't shrink an allocation to a larger size");

			size_t rst = newSize % m_buffers[0].getMemoryRequirements().alignment;
			if (rst != 0)
				newSize += m_buffers[0].getMemoryRequirements().alignment - rst;
			if (newSize == allocation.region.size)
				return;

			m_allocations[allocation.bufferIndex].erase(it);
			m_allocatedSizes[allocation.bufferIndex] -= allocation.region.size;
			shrinkRegion(allocation.region, newSize, allocation.bufferIndex);
			m_allocatedSizes[allocation.bufferIndex] += newSize;
			m_allocations[allocation.bufferIndex].insert(allocation);
		}

		std::vector<Buffer>& getBuffers() { return m_buffers; };
		std::vector<MemoryType>& getMemories() { return m_memories; };

		const std::vector<Buffer>& getBuffers() const { return m_buffers; };
		const std::vector<MemoryType>& getMemories() const { return m_memories; };

		const auto& getFreeRegions(size_t chunkIndex) const { return m_freeRegions[chunkIndex];  };
		const auto& getAllocations(size_t chunkIndex) const { return m_allocations[chunkIndex]; };
		const auto& getAllFreeRegions() const { return m_freeRegions; };
		const auto& getAllAllocations() const { return m_allocations; };		

		size_t getChunkSize() const { return m_chunkCapacity; };
		size_t getChunkAllocatedSize(size_t chunkIndex) const { return m_allocatedSizes[chunkIndex]; };
		size_t getChunkFreeSize(size_t chunkIndex) const { return m_chunkCapacity - m_allocatedSizes[chunkIndex]; };

		void destroy(const Context& instance, const Device& device) {
			if (!m_initialized)
				return;

			for (auto& buffer : m_buffers)
				buffer.destroy(instance, device);
			for (auto& memory : m_memories)
				memory.destroy(instance, device);
#ifdef _DEBUG
			std::cout << "Freed memory pool" << std::endl;
#endif
			m_initialized = false;
		}

	private:
		void addBuffer(const Context& instance, const Device& device)
		{
			Buffer buffer = Buffer(instance, device,
				m_chunkCapacity, m_usageFlags, m_allowConcurrentAccess);
			MemoryType memory = MemoryType(instance, device,
				buffer.getMemoryRequirements(), m_memoryProperties,
				m_chunkCapacity);
			memory.bindBuffer(instance, device, buffer, 0);
			m_buffers.push_back(std::move(buffer));
			m_memories.push_back(std::move(memory));
			m_freeRegions.push_back(std::list<MemoryRegion>{ MemoryRegion{ 0, m_chunkCapacity } });
			m_allocations.push_back(AllocationSet());
			m_allocatedSizes.push_back(0);
		}

		MemoryRegion getRegionBestFit(size_t bufferIndex, size_t size)
		{
			MemoryRegion region;
			size_t bestFitSize = std::numeric_limits<size_t>::max();
			auto bestFitRegion = m_freeRegions[bufferIndex].end();

			for (auto it = m_freeRegions[bufferIndex].begin();
				it != m_freeRegions[bufferIndex].end(); ++it)
			{
				region = *it;
				if (region.size >= size && region.size < bestFitSize)
				{
					bestFitSize = region.size;
					bestFitRegion = it;
				}
			}

			if (bestFitRegion == m_freeRegions[bufferIndex].end())
				return MemoryRegion{ 0, 0 };

			region.offset = bestFitRegion->offset;
			region.size = size;

			bestFitRegion->offset += size;
			bestFitRegion->size -= size;

			return region;
		}
		void freeRegion(const MemoryRegion& region, size_t bufferIndex)
		{
			//nesting because performance
			for (auto it = m_freeRegions[bufferIndex].begin();
				it != m_freeRegions[bufferIndex].end(); ++it)
			{
				if (region.offset < it->offset)
				{
					if (region.offset + region.size == it->offset)
					{
						it->size += region.size;
						if (it != m_freeRegions[bufferIndex].begin())
						{
							auto next = it--;
							if (it->offset + it->size == next->offset)
							{
								it->size += next->size;
								m_freeRegions[bufferIndex].erase(next);
							}
							else next->offset = region.offset;
						}
						else it->offset = region.offset;
					}
					else
					{
						if (it != m_freeRegions[bufferIndex].begin())
						{
							auto next = it--;
							if (it->offset + it->size == region.offset)
								it->size += region.size;
							else m_freeRegions[bufferIndex].insert(next, region);
						}
						else m_freeRegions[bufferIndex].insert(it, region);
					}
					return;
				}
			}

			if (!m_freeRegions[bufferIndex].empty() &&
				m_freeRegions[bufferIndex].back().offset + m_freeRegions[bufferIndex].back().size == region.offset)
				m_freeRegions[bufferIndex].back().size += region.size;
			else m_freeRegions[bufferIndex].push_back(region);
		}

		void shrinkRegion(MemoryRegion& region, size_t newSize, size_t bufferIndex)
		{
			size_t dif = region.size - newSize;
			for (auto it = m_freeRegions[bufferIndex].begin();
				it != m_freeRegions[bufferIndex].end(); ++it)
			{
				if (region.offset < it->offset)
				{
					if (region.offset + region.size == it->offset)
					{
						it->offset -= dif;
						it->size += dif;
					}
					else
					{
						MemoryRegion regionFree;
						regionFree.offset = region.offset + newSize;
						regionFree.size = dif;
						m_freeRegions[bufferIndex].insert(it, regionFree);
					}
					region.size = newSize;
					return;
				}
			}

			MemoryRegion regionFree;
			regionFree.offset = region.offset + newSize;
			regionFree.size = dif;
			m_freeRegions[bufferIndex].push_back(regionFree);
			region.size = newSize;
		}
	};
}
