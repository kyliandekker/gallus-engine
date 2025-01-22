#include "graphics/dx12/DescriptorAllocator.h"

#undef max

#include "graphics/dx12/DescriptorAllocatorPage.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            struct MakeAllocatorPage : public DescriptorAllocatorPage
            {
            public:
                MakeAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS a_Flags)
                    : DescriptorAllocatorPage(a_Type, a_NumDescriptors, a_Flags)
                {}

                virtual ~MakeAllocatorPage() {}
            };

            DescriptorAllocation DescriptorAllocator::Allocate(uint32_t a_NumDescriptors)
            {
                std::lock_guard<std::mutex> lock(m_AllocationMutex);

                DescriptorAllocation allocation;

                auto iter = m_AvailableHeaps.begin();
                while (iter != m_AvailableHeaps.end())
                {
                    auto allocatorPage = m_HeapPool[*iter];

                    allocation = allocatorPage->Allocate(a_NumDescriptors);

                    if (allocatorPage->NumFreeHandles() == 0)
                    {
                        iter = m_AvailableHeaps.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }

                    // A valid allocation has been found.
                    if (!allocation.IsNull())
                    {
                        break;
                    }
                }

                // No available heap could satisfy the requested number of descriptors.
                if (allocation.IsNull())
                {
                    m_NumDescriptorsPerHeap = std::max(m_NumDescriptorsPerHeap, a_NumDescriptors);
                    auto newPage = CreateAllocatorPage();

                    allocation = newPage->Allocate(a_NumDescriptors);
                }

                return allocation;
            }

            void DescriptorAllocator::ReleaseStaleDescriptors()
            {
                std::lock_guard<std::mutex> lock(m_AllocationMutex);

                for (size_t i = 0; i < m_HeapPool.size(); ++i)
                {
                    auto page = m_HeapPool[i];

                    page->ReleaseStaleDescriptors();

                    if (page->NumFreeHandles() > 0)
                    {
                        m_AvailableHeaps.insert(i);
                    }
                }
            }

            DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptorsPerHeap, D3D12_DESCRIPTOR_HEAP_FLAGS a_Flags)
                : m_HeapType(a_Type)
                , m_NumDescriptorsPerHeap(a_NumDescriptorsPerHeap)
                , m_Flags(a_Flags)
            { }

            std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage()
            {
                std::shared_ptr<DescriptorAllocatorPage> newPage = std::make_shared<MakeAllocatorPage>(m_HeapType, m_NumDescriptorsPerHeap, m_Flags);

                m_HeapPool.emplace_back(newPage);
                m_AvailableHeaps.insert(m_HeapPool.size() - 1);

                return newPage;
            }
        }
    }
}