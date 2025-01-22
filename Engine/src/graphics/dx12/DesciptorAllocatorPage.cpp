#include "graphics/dx12/DescriptorAllocatorPage.h"

#include "core/Engine.h"
#include "core/logger/Logger.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorAllocatorPage::GetHeap()
            {
                return m_d3d12DescriptorHeap;
            }

            D3D12_DESCRIPTOR_HEAP_TYPE DescriptorAllocatorPage::GetHeapType() const
            {
                return m_HeapType;
            }

            bool DescriptorAllocatorPage::HasSpace(uint32_t a_NumDescriptors) const
            {
                return m_FreeListBySize.lower_bound(a_NumDescriptors) != m_FreeListBySize.end();
            }

            uint32_t DescriptorAllocatorPage::NumFreeHandles() const
            {
                return m_NumFreeHandles;
            }

            DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t a_NumDescriptors)
            {
                std::lock_guard<std::mutex> lock(m_AllocationMutex);

                // There are less than the requested number of descriptors left in the heap.
                // Return a NULL descriptor and try another heap.
                if (a_NumDescriptors > m_NumFreeHandles)
                {
                    return DescriptorAllocation();
                }

                // Get the first block that is large enough to satisfy the request.
                auto smallestBlockIt = m_FreeListBySize.lower_bound(a_NumDescriptors);
                if (smallestBlockIt == m_FreeListBySize.end())
                {
                    // There was no free block that could satisfy the request.
                    return DescriptorAllocation();
                }

                // The size of the smallest block that satisfies the request.
                auto blockSize = smallestBlockIt->first;

                // The pointer to the same entry in the FreeListByOffset map.
                auto offsetIt = smallestBlockIt->second;

                // The offset in the descriptor heap.
                auto offset = offsetIt->first;

                // Remove the existing free block from the free list.
                m_FreeListBySize.erase(smallestBlockIt);
                m_FreeListByOffset.erase(offsetIt);

                // Compute the new free block that results from splitting this block.
                auto newOffset = offset + a_NumDescriptors;
                auto newSize = blockSize - a_NumDescriptors;

                if (newSize > 0)
                {
                    // If the allocation didn't exactly match the requested size,
                    // return the left-over to the free list.
                    AddNewBlock(newOffset, newSize);
                }

                // Decrement free handles.
                m_NumFreeHandles -= a_NumDescriptors;

                return DescriptorAllocation(
                    CD3DX12_CPU_DESCRIPTOR_HANDLE(m_BaseDescriptor, offset, m_DescriptorHandleIncrementSize), a_NumDescriptors,
                    m_DescriptorHandleIncrementSize, shared_from_this());
            }

            void DescriptorAllocatorPage::Free(DescriptorAllocation&& a_DescriptorHandle)
            {
                // Compute the offset of the descriptor within the descriptor heap.
                auto offset = ComputeOffset(a_DescriptorHandle.GetDescriptorHandle());

                std::lock_guard<std::mutex> lock(m_AllocationMutex);
                // Don't add the block directly to the free list until the frame has completed.
                m_StaleDescriptors.emplace(offset, a_DescriptorHandle.GetNumHandles());
            }

            void DescriptorAllocatorPage::ReleaseStaleDescriptors()
            {
                std::lock_guard<std::mutex> lock(m_AllocationMutex);

                while (!m_StaleDescriptors.empty())
                {
                    auto& staleDescriptor = m_StaleDescriptors.front();

                    // The offset of the descriptor in the heap.
                    auto offset = staleDescriptor.Offset;
                    // The number of descriptors that were allocated.
                    auto a_NumDescriptors = staleDescriptor.Size;

                    FreeBlock(offset, a_NumDescriptors);

                    m_StaleDescriptors.pop();
                }
            }

            DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS a_Flags)
                : m_HeapType(a_Type)
                , m_NumDescriptorsInHeap(a_NumDescriptors)
            {
                D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
                heapDesc.Type = m_HeapType;
                heapDesc.NumDescriptors = m_NumDescriptorsInHeap;
                heapDesc.Flags = a_Flags;

                if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_d3d12DescriptorHeap))))
                {
                    LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create descriptor heap.");
                    return;
                }

                m_BaseDescriptor = m_d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                m_DescriptorHandleIncrementSize = core::ENGINE.GetDX12().GetDevice()->GetDescriptorHandleIncrementSize(m_HeapType);
                m_NumFreeHandles = m_NumDescriptorsInHeap;

                // Initialize the free lists
                AddNewBlock(0, m_NumFreeHandles);
            }

            uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE a_Handle)
            {
                return static_cast<uint32_t>(a_Handle.ptr - m_BaseDescriptor.ptr) / m_DescriptorHandleIncrementSize;
            }

            void DescriptorAllocatorPage::AddNewBlock(uint32_t a_Offset, uint32_t a_NumDescriptors)
            {
                auto offsetIt = m_FreeListByOffset.emplace(a_Offset, a_NumDescriptors);
                auto sizeIt = m_FreeListBySize.emplace(a_NumDescriptors, offsetIt.first);
                offsetIt.first->second.FreeListBySizeIt = sizeIt;
            }

            void DescriptorAllocatorPage::FreeBlock(uint32_t a_Offset, uint32_t a_NumDescriptors)
            {
                // Find the first element whose offset is greater than the specified offset.
                // This is the block that should appear after the block that is being freed.
                auto nextBlockIt = m_FreeListByOffset.upper_bound(a_Offset);

                // Find the block that appears before the block being freed.
                auto prevBlockIt = nextBlockIt;
                // If it's not the first block in the list.
                if (prevBlockIt != m_FreeListByOffset.begin())
                {
                    // Go to the previous block in the list.
                    --prevBlockIt;
                }
                else
                {
                    // Otherwise, just set it to the end of the list to indicate that no
                    // block comes before the one being freed.
                    prevBlockIt = m_FreeListByOffset.end();
                }

                // Add the number of free handles back to the heap.
                // This needs to be done before merging any blocks since merging
                // blocks modifies the numDescriptors variable.
                m_NumFreeHandles += a_NumDescriptors;

                if (prevBlockIt != m_FreeListByOffset.end() && a_Offset == prevBlockIt->first + prevBlockIt->second.Size)
                {
                    // The previous block is exactly behind the block that is to be freed.
                    //
                    // PrevBlock.Offset           Offset
                    // |                          |
                    // |<-----PrevBlock.Size----->|<------Size-------->|
                    //

                    // Increase the block size by the size of merging with the previous block.
                    a_Offset = prevBlockIt->first;
                    a_NumDescriptors += prevBlockIt->second.Size;

                    // Remove the previous block from the free list.
                    m_FreeListBySize.erase(prevBlockIt->second.FreeListBySizeIt);
                    m_FreeListByOffset.erase(prevBlockIt);
                }

                if (nextBlockIt != m_FreeListByOffset.end() && a_Offset + a_NumDescriptors == nextBlockIt->first)
                {
                    // The next block is exactly in front of the block that is to be freed.
                    //
                    // Offset               NextBlock.Offset
                    // |                    |
                    // |<------Size-------->|<-----NextBlock.Size----->|

                    // Increase the block size by the size of merging with the next block.
                    a_NumDescriptors += nextBlockIt->second.Size;

                    // Remove the next block from the free list.
                    m_FreeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
                    m_FreeListByOffset.erase(nextBlockIt);
                }

                // Add the freed block to the free list.
                AddNewBlock(a_Offset, a_NumDescriptors);
            }
        }
    }
}