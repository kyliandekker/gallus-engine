#include "graphics/dx12/HeapAllocation.h"

#include "core/Engine.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			HeapAllocation::HeapAllocation(const D3D12_DESCRIPTOR_HEAP_DESC& a_Desc)
			{
				HRESULT hr = core::ENGINE.GetDX12().GetDevice()->CreateDescriptorHeap(&a_Desc, IID_PPV_ARGS(&m_Heap));
				if (FAILED(hr))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create descriptor heap.");
					return;
				}

				m_Allocated = std::vector<bool>(a_Desc.NumDescriptors);
			}

			size_t HeapAllocation::Allocate()
			{
				int index = 0;
				for (bool allocated : m_Allocated)
				{
					if (!allocated) {
						break;
					}
					index++;
				}

				m_Allocated[index] = true;
				return index;
			}

			void HeapAllocation::Deallocate(size_t a_Index)
			{
				m_Allocated[a_Index] = false;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE HeapAllocation::GetCPUDescriptorHandleForHeapStart()
			{
				return m_Heap->GetCPUDescriptorHandleForHeapStart();
			}

			D3D12_GPU_DESCRIPTOR_HANDLE HeapAllocation::GetGPUDescriptorHandleForHeapStart()
			{
				return m_Heap->GetGPUDescriptorHandleForHeapStart();
			}

			CD3DX12_GPU_DESCRIPTOR_HANDLE HeapAllocation::GetGPUHandle(size_t a_Index)
			{
				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_Heap->GetGPUDescriptorHandleForHeapStart());
				gpuHandle.Offset(a_Index, core::ENGINE.GetDX12().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

				return gpuHandle;
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE HeapAllocation::GetCPUHandle(size_t a_Index)
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_Heap->GetCPUDescriptorHandleForHeapStart());
				cpuHandle.Offset(a_Index, core::ENGINE.GetDX12().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

				return cpuHandle;
			}

			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapAllocation::GetHeap()
			{
				return m_Heap;
			}
		}
	}
}