//#pragma once
//
//#include "graphics/dx12/DX12PCH.h"
//
//namespace coopscoop
//{
//	namespace graphics
//	{
//		namespace dx12
//		{
//			class DX12Resource;
//			class VertexBuffer;
//			class IndexBuffer;
//			class Buffer;
//
//			class CommandList
//			{
//			public:
//				CommandList(D3D12_COMMAND_LIST_TYPE a_Type);
//
//				D3D12_COMMAND_LIST_TYPE GetCommandListType() const
//				{
//					return m_d3d12CommandListType;
//				}
//
//				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetGraphicsCommandList() const
//				{
//					return m_d3d12CommandList;
//				}
//
//				void TransitionBarrier(DX12Resource& a_Resource, D3D12_RESOURCE_STATES a_StateAfter, UINT a_Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool a_FlushBarriers = false);
//
//                void CopyVertexBuffer(VertexBuffer& a_VertexBuffer, size_t a_NumVertices, size_t a_VertexStride, const void* a_VertexBufferData);
//
//                void CopyIndexBuffer(IndexBuffer& a_IndexBuffer, size_t a_NumIndicies, DXGI_FORMAT a_IndexFormat, const void* a_IndexBufferData);
//
//                void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY a_PrimitiveTopology);
//
//				void ReleaseTrackedObjects();
//
//				void CopyBuffer(Buffer& a_Buffer, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags = D3D12_RESOURCE_FLAG_NONE);
//
//				bool Close(CommandList& a_PendingCommandList);
//				void Close();
//			private:
//				D3D12_COMMAND_LIST_TYPE m_d3d12CommandListType;
//				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_d3d12CommandList;
//				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_d3d12CommandAllocator;
//
//				using TrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;
//				TrackedObjects m_TrackedObjects;
//			};
//		}
//	}
//}