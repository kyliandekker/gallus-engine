#include "graphics/dx12/DescriptorAllocation.h"

#include "graphics/dx12/DescriptorAllocatorPage.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            DescriptorAllocation::DescriptorAllocation()
                : m_Descriptor{ 0 }
                , m_NumHandles(0)
                , m_DescriptorSize(0)
                , m_Page(nullptr)
            {}
            DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE a_Descriptor, uint32_t a_NumHandles, uint32_t a_DescriptorSize, std::shared_ptr<DescriptorAllocatorPage> a_Page) 
                : m_Descriptor(a_Descriptor)
                , m_NumHandles(a_NumHandles)
                , m_DescriptorSize(a_DescriptorSize)
                , m_Page(a_Page)
            { }

            DescriptorAllocation::~DescriptorAllocation()
            {
                Free();
            }

            DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& a_Allocation) noexcept
                : m_Descriptor(a_Allocation.m_Descriptor)
                , m_NumHandles(a_Allocation.m_NumHandles)
                , m_DescriptorSize(a_Allocation.m_DescriptorSize)
                , m_Page(std::move(a_Allocation.m_Page))
            {
                a_Allocation.m_Descriptor.ptr = 0;
                a_Allocation.m_NumHandles = 0;
                a_Allocation.m_DescriptorSize = 0;
            }
            DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& a_Other) noexcept
            {
                // Free this descriptor if it points to anything.
                Free();

                m_Descriptor = a_Other.m_Descriptor;
                m_NumHandles = a_Other.m_NumHandles;
                m_DescriptorSize = a_Other.m_DescriptorSize;
                m_Page = std::move(a_Other.m_Page);

                a_Other.m_Descriptor.ptr = 0;
                a_Other.m_NumHandles = 0;
                a_Other.m_DescriptorSize = 0;

                return *this;
            }

            bool DescriptorAllocation::IsNull() const
            {
                return m_Descriptor.ptr == 0;
            }

            D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t a_Offset) const
            {
                assert(a_Offset < m_NumHandles);
                return { m_Descriptor.ptr + (m_DescriptorSize * a_Offset) };
            }

            uint32_t DescriptorAllocation::GetNumHandles() const
            {
                return m_NumHandles;
            }

            std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const
            {
                return m_Page;
            }

            void DescriptorAllocation::Free()
            {
                if (!IsNull() && m_Page)
                {
                    m_Page->Free(std::move(*this));

                    m_Descriptor.ptr = 0;
                    m_NumHandles = 0;
                    m_DescriptorSize = 0;
                    m_Page.reset();
                }
            }
        }
    }
}