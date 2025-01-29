#pragma once

#include "graphics/dx12/DX12PCH.h"
#include <string>
#include <vector>
#include <cstdint>

#include "graphics/dx12/Transform.h"
#include "graphics/dx12/Shader.h"
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

				VertexBuffer m_VertexBuffer;
				IndexBuffer m_IndexBuffer;

				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
			};

			class Texture;

			class Mesh : DX12Resource
			{
			public:
				Mesh();
				bool LoadMesh(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
				bool LoadTexture(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

				bool Transition(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

				bool SetShader(Shader& a_Shader);

				void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, const Transform& a_Transform, DirectX::XMMATRIX a_CameraView, DirectX::XMMATRIX a_CameraProjection);
			private:
				D3D12_SHADER_RESOURCE_VIEW_DESC m_ShaderResourceView;
				std::vector<MeshData*> m_MeshData;

				Shader m_Shader;
				Texture* m_Texture = nullptr;
			};
		}
	}
}