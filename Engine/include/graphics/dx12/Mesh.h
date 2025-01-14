#pragma once

#include "graphics/dx12/DX12PCH.h"
#include <string>
#include <vector>
#include <cstdint>

#include "graphics/dx12/Transform.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/VertexBuffer.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			struct VertexPosColorUV
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Color;
				DirectX::XMFLOAT2 UV;
			};

			struct MeshData
			{
				enum class IndexSize
				{
					INDEXSIZE_16,
					INDEXSIZE_23
				};

				std::vector<VertexPosColorUV> m_Vertices;
				std::vector<uint16_t> m_Indices;
				IndexSize m_IndexSize;

				VertexBuffer m_VertexBufferN;

				// Vertex buffer for the cube.
				Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
				D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
				// Index buffer for the cube.
				Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
				D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
			};

			class Mesh
			{
			public:
				Mesh();
				bool LoadMesh(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
				bool SetShader(Shader& a_Shader);

				Transform& GetTransform();

				void Update(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
				void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
			private:
				void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, ID3D12Resource** a_pDestinationResource, ID3D12Resource** a_pIntermediateResource, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags = D3D12_RESOURCE_FLAG_NONE);

				D3D12_SHADER_RESOURCE_VIEW_DESC m_ShaderResourceView;
				std::vector<MeshData*> m_MeshData;
				Shader m_Shader;
				Transform m_Transform;
			};
		}
	}
}