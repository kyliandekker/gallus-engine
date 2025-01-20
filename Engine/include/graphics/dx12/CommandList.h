#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <functional>  // For std::function
#include <map>         // for std::map
#include <memory>      // for std::unique_ptr
#include <mutex>       // for std::mutex
#include <vector>      // for std::vector

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class Buffer;
			class ByteAddressBuffer;
			class ConstantBuffer;
			class ConstantBufferView;
			class Device;
			class DynamicDescriptorHeap;
			class GenerateMipsPSO;
			class IndexBuffer;
			class PanoToCubemapPSO;
			class PipelineStateObject;
			class RenderTarget;
			class DX12Resource;
			class ResourceStateTracker;
			class RootSignature;
			class Scene;
			class ShaderResourceView;
			class StructuredBuffer;
			class Texture;
			class UnorderedAccessView;
			class UploadBuffer;
			class VertexBuffer;

			class CommandList : public std::enable_shared_from_this<CommandList>
			{
			public:
				D3D12_COMMAND_LIST_TYPE GetCommandListType() const
				{
					return m_d3d12CommandListType;
				}

				Device& GetDevice() const
				{
					return m_Device;
				}

				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetD3D12CommandList() const
				{
					return m_d3d12CommandList;
				}

				void TransitionBarrier(const std::shared_ptr<DX12Resource>& a_Resource, D3D12_RESOURCE_STATES a_StateAfter,
					UINT a_Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool a_FlushBarriers = false);
				void TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource, D3D12_RESOURCE_STATES a_StateAfter,
					UINT a_Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool a_FlushBarriers = false);

				void UAVBarrier(const std::shared_ptr<DX12Resource>& a_Resource = nullptr, bool a_FlushBarriers = false);
				void UAVBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource, bool a_FlushBarriers = false);

				void AliasingBarrier(const std::shared_ptr<DX12Resource>& a_BeforeResource = nullptr, const std::shared_ptr<DX12Resource>& a_AfterResource = nullptr, bool a_FlushBarriers = false);
				void AliasingBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> a_BeforeResource,
					Microsoft::WRL::ComPtr<ID3D12Resource> a_AfterResource, bool a_FlushBarriers = false);

				void FlushResourceBarriers();

				void CopyResource(const std::shared_ptr<DX12Resource>& a_DestinationResource, const std::shared_ptr<DX12Resource>& a_SrcResource);
				void CopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_DestinationResource, Microsoft::WRL::ComPtr<ID3D12Resource> a_SrcResource);

				void ResolveSubresource(const std::shared_ptr<DX12Resource>& a_DestinationResource, const std::shared_ptr<DX12Resource>& a_SrcResource,
					uint32_t a_DestinationSubresource = 0, uint32_t a_SrcSubresource = 0);

				std::shared_ptr<VertexBuffer> CopyVertexBuffer(size_t a_NumVertices, size_t a_VertexStride, const void* a_VertexBufferData);
				template<typename T>
				std::shared_ptr<VertexBuffer> CopyVertexBuffer(const std::vector<T>& a_VertexBufferData)
				{
					return CopyVertexBuffer(a_VertexBufferData.size(), sizeof(T), a_VertexBufferData.data());
				}

				std::shared_ptr<IndexBuffer> CopyIndexBuffer(size_t a_NumIndicies, DXGI_FORMAT a_IndexFormat, const void* a_IndexBufferData);
				template<typename T>
				std::shared_ptr<IndexBuffer> CopyIndexBuffer(const std::vector<T>& a_IndexBufferData)
				{
					assert(sizeof(T) == 2 || sizeof(T) == 4);

					DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
					return CopyIndexBuffer(a_IndexBufferData.size(), indexFormat, a_IndexBufferData.data());
				}

				std::shared_ptr<ConstantBuffer> CopyConstantBuffer(size_t a_BufferSize, const void* a_BufferData);
				template<typename T>
				std::shared_ptr<ConstantBuffer> CopyConstantBuffer(const T& a_Data)
				{
					return CopyConstantBuffer(sizeof(T), &a_Data);
				}

				std::shared_ptr<ByteAddressBuffer> CopyByteAddressBuffer(size_t a_BufferSize, const void* a_BufferData);
				template<typename T>
				std::shared_ptr<ByteAddressBuffer> CopyByteAddressBuffer(const T& a_Data)
				{
					return CopyByteAddressBuffer(sizeof(T), &a_Data);
				}

				std::shared_ptr<StructuredBuffer> CopyStructuredBuffer(size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData);
				template<typename T>
				std::shared_ptr<StructuredBuffer> CopyStructuredBuffer(const std::vector<T>& a_BufferData)
				{
					return CopyStructuredBuffer(a_BufferData.size(), sizeof(T), a_BufferData.data());
				}

				void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY a_PrimitiveTopology);

				std::shared_ptr<Texture> LoadTextureFromFile(const std::wstring& a_FileName, bool a_sRGB = false);

				void ClearTexture(const std::shared_ptr<Texture>& a_Texture, const float a_ClearColor[4]);

				void ClearDepthStencilTexture(const std::shared_ptr<Texture>& a_Texture, D3D12_CLEAR_FLAGS a_ClearFlags, float a_Depth = 1.0f, uint8_t a_Stencil = 0);

				void GenerateMips(const std::shared_ptr<Texture>& a_Texture);

				void PanoToCubemap(const std::shared_ptr<Texture>& a_CubemapTexture, const std::shared_ptr<Texture>& a_PanoTexture);

				void CopyTextureSubresource(const std::shared_ptr<Texture>& a_Texture, uint32_t a_FirstSubresource, uint32_t a_NumSubresources, D3D12_SUBRESOURCE_DATA* a_SubresourceData);

				void SetGraphicsDynamicConstantBuffer(uint32_t a_RootParameterIndex, size_t a_SizeInBytes, const void* a_BufferData);
				template<typename T>
				void SetGraphicsDynamicConstantBuffer(uint32_t a_RootParameterIndex, const T& a_Data)
				{
					SetGraphicsDynamicConstantBuffer(a_RootParameterIndex, sizeof(T), &a_Data);
				}

				void SetGraphics32BitConstants(uint32_t a_RootParameterIndex, uint32_t a_NumConstants, const void* a_Constants);
				template<typename T>
				void SetGraphics32BitConstants(uint32_t a_RootParameterIndex, const T& a_Constants)
				{
					static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
					SetGraphics32BitConstants(a_RootParameterIndex, sizeof(T) / sizeof(uint32_t), &a_Constants);
				}

				void SetCompute32BitConstants(uint32_t a_RootParameterIndex, uint32_t a_NumConstants, const void* a_Constants);
				template<typename T>
				void SetCompute32BitConstants(uint32_t a_RootParameterIndex, const T& a_Constants)
				{
					static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
					SetCompute32BitConstants(a_RootParameterIndex, sizeof(T) / sizeof(uint32_t), &a_Constants);
				}

				void SetVertexBuffers(uint32_t a_StartSlot, const std::vector<std::shared_ptr<VertexBuffer>>& a_VertexBufferViews);
				void SetVertexBuffer(uint32_t a_Slot, const std::shared_ptr<VertexBuffer>& a_VertexBufferView);

				void SetDynamicVertexBuffer(uint32_t a_Slot, size_t a_NumVertices, size_t a_VertexSize, const void* a_VertexBufferData);
				template<typename T>
				void SetDynamicVertexBuffer(uint32_t a_Slot, const std::vector<T>& a_VertexBufferData)
				{
					SetDynamicVertexBuffer(a_Slot, a_VertexBufferData.size(), sizeof(T), a_VertexBufferData.data());
				}

				void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& a_IndexBuffer);

				void SetDynamicIndexBuffer(size_t a_NumIndicies, DXGI_FORMAT a_IndexFormat, const void* a_IndexBufferData);
				template<typename T>
				void SetDynamicIndexBuffer(const std::vector<T>& a_IndexBufferData)
				{
					static_assert(sizeof(T) == 2 || sizeof(T) == 4);

					DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
					SetDynamicIndexBuffer(a_IndexBufferData.size(), indexFormat, a_IndexBufferData.data());
				}

				void SetGraphicsDynamicStructuredBuffer(uint32_t a_Slot, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData);
				template<typename T>
				void SetGraphicsDynamicStructuredBuffer(uint32_t a_Slot, const std::vector<T>& a_BufferData)
				{
					SetGraphicsDynamicStructuredBuffer(a_Slot, a_BufferData.size(), sizeof(T), a_BufferData.data());
				}

				void SetViewport(const D3D12_VIEWPORT& a_Viewport);
				void SetViewports(const std::vector<D3D12_VIEWPORT>& a_Viewports);

				void SetScissorRect(const D3D12_RECT& a_ScissorRect);
				void SetScissorRects(const std::vector<D3D12_RECT>& a_ScissorRects);

				void SetPipelineState(const std::shared_ptr<PipelineStateObject>& a_PipelineState);

				void SetGraphicsRootSignature(const std::shared_ptr<RootSignature>& a_RootSignature);
				void SetComputeRootSignature(const std::shared_ptr<RootSignature>& a_RootSignature);

				void SetConstantBufferView(uint32_t a_RootParameterIndex, const std::shared_ptr<ConstantBuffer>& a_Buffer, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, size_t a_BufferOffset = 0);

				void SetShaderResourceView(uint32_t a_RootParameterIndex, const std::shared_ptr<Buffer>& a_Buffer, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, size_t a_BufferOffset = 0);

				void SetUnorderedAccessView(uint32_t a_RootParameterIndex, const std::shared_ptr<Buffer>& a_Buffer, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, size_t a_BufferOffset = 0);

				void SetConstantBufferView(uint32_t a_RootParameterIndex, uint32_t a_DescriptorOffset, const std::shared_ptr<ConstantBufferView>& a_Cbv, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				void SetShaderResourceView(uint32_t a_RootParameterIndex, uint32_t a_DescriptorOffset, const std::shared_ptr<ShaderResourceView>& a_Srv, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT a_FirstSubresource = 0, UINT a_NumSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

				void SetShaderResourceView(int32_t a_RootParameterIndex, uint32_t a_DescriptorOffset,
					const std::shared_ptr<Texture>& a_Texture,
					D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
					D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
					UINT a_FirstSubresource = 0,
					UINT a_NumSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

				void SetUnorderedAccessView(uint32_t a_RootParameterIndex, uint32_t a_DescriptorOffset, const std::shared_ptr<UnorderedAccessView>& a_Uav, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, UINT a_FirstSubresource = 0, UINT a_NumSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

				void SetUnorderedAccessView(uint32_t a_RootParameterIndex, uint32_t a_DescriptorOffset, const std::shared_ptr<Texture>& a_Texture, UINT a_Mip, D3D12_RESOURCE_STATES a_StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, UINT a_FirstSubresource = 0, UINT a_NumSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

				void SetRenderTarget(const RenderTarget& a_RenderTarget);

				void Draw(uint32_t a_VertexCount, uint32_t a_InstanceCount = 1, uint32_t a_StartVertex = 0, uint32_t a_StartInstance = 0);
				void DrawIndexed(uint32_t a_IndexCount, uint32_t a_InstanceCount = 1, uint32_t a_StartIndex = 0, int32_t a_BaseVertex = 0, uint32_t a_StartInstance = 0);

				void Dispatch(uint32_t a_NumGroupsX, uint32_t a_NumGroupsY = 1, uint32_t a_NumGroupsZ = 1);
			protected:
				friend class CommandQueue;
				friend class DynamicDescriptorHeap;
				friend class std::default_delete<CommandList>;

				CommandList(Device& a_Device, D3D12_COMMAND_LIST_TYPE a_Type);
				virtual ~CommandList();

				bool Close(const std::shared_ptr<CommandList>& a_PendingCommandList);
				void Close();

				void Reset();

				void ReleaseTrackedObjects();

				void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE a_HeapType, ID3D12DescriptorHeap* a_Heap);

				std::shared_ptr<CommandList> GetGenerateMipsCommandList() const
				{
					return m_ComputeCommandList;
				}
			private:
				void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> a_Object);
				void TrackResource(const std::shared_ptr<DX12Resource>& a_Resource);

				void GenerateMips_UAV(const std::shared_ptr<Texture>& a_Texture, bool a_IsSRGB);

				Microsoft::WRL::ComPtr<ID3D12Resource> CopyBuffer(size_t a_BufferSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags = D3D12_RESOURCE_FLAG_NONE);

				void BindDescriptorHeaps();

				Device& m_Device;
				D3D12_COMMAND_LIST_TYPE m_d3d12CommandListType;
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_d3d12CommandList;
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_d3d12CommandAllocator;

				std::shared_ptr<CommandList> m_ComputeCommandList;

				ID3D12RootSignature* m_RootSignature;

				ID3D12PipelineState* m_PipelineState;

				std::unique_ptr<UploadBuffer> m_UploadBuffer;

				std::unique_ptr<ResourceStateTracker> m_ResourceStateTracker;

				std::unique_ptr<DynamicDescriptorHeap> m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

				ID3D12DescriptorHeap* m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

				std::unique_ptr<GenerateMipsPSO> m_GenerateMipsPSO;

				std::unique_ptr<PanoToCubemapPSO> m_PanoToCubemapPSO;

				using TrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;

				TrackedObjects m_TrackedObjects;

				static std::map<std::wstring, ID3D12Resource*> ms_TextureCache;
				static std::mutex ms_TextureCacheMutex;
			};
		}
	}
}