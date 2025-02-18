#pragma once

#include "graphics/dx12/DX12PCH.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class HeapAllocation
			{
			public:
				HeapAllocation() = default;
				HeapAllocation(const D3D12_DESCRIPTOR_HEAP_DESC& a_Desc, Microsoft::WRL::ComPtr<ID3D12Device2> a_Device);

				size_t Allocate();
				void Deallocate(size_t a_Index);

				D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart();
				D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart();
				CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(size_t a_Index);
				CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(size_t a_Index);

				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap();
			private:
				D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;

				std::vector<bool> m_Allocated;

				Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
			};
		}
	}
}