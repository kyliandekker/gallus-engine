#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

#include <string>
#include <vector>
#include <cstdint>

#include "graphics/dx12/Transform.h"
#include "graphics/dx12/VertexBuffer.h"
#include "graphics/dx12/IndexBuffer.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			struct VertexPosColorUV
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
				DirectX::XMFLOAT2 UV;
			};

			struct MeshPartData
			{
				enum class IndexSize
				{
					INDEXSIZE_16,
					INDEXSIZE_23
				};

				std::vector<VertexPosColorUV> m_Vertices;
				std::vector<uint16_t> m_Indices;
				IndexSize m_IndexSize;

				VertexBuffer m_VertexBuffer;
				IndexBuffer m_IndexBuffer;

				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
			};

			class Mesh : public DX12Resource
			{
			public:
				Mesh();
				void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, const Transform& a_Transform, DirectX::XMMATRIX a_CameraView, DirectX::XMMATRIX a_CameraProjection);
			private:
				bool Load(const std::string& a_Name, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

				D3D12_SHADER_RESOURCE_VIEW_DESC m_ShaderResourceView;
				std::vector<MeshPartData*> m_MeshData;

				friend class ResourceAtlas;
			};
		}
	}
}