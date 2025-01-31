#pragma once

#include "graphics/dx12/DX12PCH.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList
			{
			public:
				CommandList() = default;
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList() { return m_CommandList; };

				bool CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> a_Allocator, D3D12_COMMAND_LIST_TYPE a_CommandListType);
				void UpdateBufferResource(ID3D12Resource** a_pDestinationResource, ID3D12Resource** a_pIntermediateResource, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags = D3D12_RESOURCE_FLAG_NONE);
				void TransitionResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource, D3D12_RESOURCE_STATES a_BeforeState, D3D12_RESOURCE_STATES a_AfterState);
			private:
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_CommandList;
			};
		}
	}
}