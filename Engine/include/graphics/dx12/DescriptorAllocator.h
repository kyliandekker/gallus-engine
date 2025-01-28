#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <set>
#include <mutex>

#include "graphics/dx12/DescriptorAllocation.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class DescriptorAllocator
            {
            public:
                /**
                 * Allocate a number of contiguous descriptors from a CPU visible descriptor heap.
                 *
                 * @param numDescriptors The number of contiguous descriptors to allocate.
                 * Cannot be more than the number of descriptors per descriptor heap.
                 */
                DescriptorAllocation Allocate(uint32_t a_NumDescriptors = 1);

                /**
                 * When the frame has completed, the stale descriptors can be released.
                 */
                void ReleaseStaleDescriptors();

            protected:
                friend class std::default_delete<DescriptorAllocator>;

                // Can only be created by the Device.
                DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptorsPerHeap = 256);
            private:
                using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;

                // Create a new heap with a specific number of descriptors.
                std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();

                // The device that was use to create this DescriptorAllocator.
                D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;
                uint32_t                   m_NumDescriptorsPerHeap;

                DescriptorHeapPool m_HeapPool;
                // Indices of available heaps in the heap pool.
                std::set<size_t> m_AvailableHeaps;

                std::mutex m_AllocationMutex;
            };
        }
    }
}