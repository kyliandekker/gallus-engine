#pragma once

#include "graphics/dx12/DescriptorAllocation.h"

#include <map>
#include <memory>
#include <mutex>
#include <queue>

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage>
            {
            public:
                Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap();
                D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

                /**
                 * Check to see if this descriptor page has a contiguous block of descriptors
                 * large enough to satisfy the request.
                 */
                bool HasSpace(uint32_t a_NumDescriptors) const;

                /**
                 * Get the number of available handles in the heap.
                 */
                uint32_t NumFreeHandles() const;

                /**
                 * Allocate a number of descriptors from this descriptor heap.
                 * If the allocation cannot be satisfied, then a NULL descriptor
                 * is returned.
                 */
                DescriptorAllocation Allocate(uint32_t numDescriptors);

                /**
                 * Return a descriptor back to the heap.
                 * @param frameNumber Stale descriptors are not freed directly, but put
                 * on a stale allocations queue. Stale allocations are returned to the heap
                 * using the DescriptorAllocatorPage::ReleaseStaleAllocations method.
                 */
                void Free(DescriptorAllocation&& a_DescriptorHandle);

                /**
                 * Returned the stale descriptors back to the descriptor heap.
                 */
                void ReleaseStaleDescriptors();

            protected:
                DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS a_Flags);
                virtual ~DescriptorAllocatorPage() = default;

                // Compute the offset of the descriptor handle from the start of the heap.
                uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE a_Handle);

                // Adds a new block to the free list.
                void AddNewBlock(uint32_t a_Offset, uint32_t a_NumDescriptors);

                // Free a block of descriptors.
                // This will also merge free blocks in the free list to form larger blocks
                // that can be reused.
                void FreeBlock(uint32_t a_Offset, uint32_t a_NumDescriptors);

            private:
                // The offset (in descriptors) within the descriptor heap.
                using OffsetType = uint32_t;
                // The number of descriptors that are available.
                using SizeType = uint32_t;

                struct FreeBlockInfo;
                // A map that lists the free blocks by the offset within the descriptor heap.
                using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;

                // A map that lists the free blocks by size.
                // Needs to be a multimap since multiple blocks can have the same size.
                using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

                struct FreeBlockInfo
                {
                    FreeBlockInfo(SizeType size)
                        : Size(size)
                    {}

                    SizeType                 Size;
                    FreeListBySize::iterator FreeListBySizeIt;
                };

                struct StaleDescriptorInfo
                {
                    StaleDescriptorInfo(OffsetType offset, SizeType size)
                        : Offset(offset)
                        , Size(size)
                    {}

                    // The offset within the descriptor heap.
                    OffsetType Offset;
                    // The number of descriptors
                    SizeType Size;
                };

                // Stale descriptors are queued for release until the frame that they were freed
                // has completed.
                using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

                FreeListByOffset     m_FreeListByOffset;
                FreeListBySize       m_FreeListBySize;
                StaleDescriptorQueue m_StaleDescriptors;

                Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3d12DescriptorHeap;
                D3D12_DESCRIPTOR_HEAP_TYPE                   m_HeapType;
                CD3DX12_CPU_DESCRIPTOR_HANDLE                m_BaseDescriptor;
                uint32_t                                     m_DescriptorHandleIncrementSize;
                uint32_t                                     m_NumDescriptorsInHeap;
                uint32_t                                     m_NumFreeHandles;

                std::mutex m_AllocationMutex;
            };
        }
    }
}