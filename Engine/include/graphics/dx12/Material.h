#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			struct MaterialData
			{
				DirectX::XMFLOAT3 DiffuseColor;  // Base color of the material
				float Metallic;                   // 0 = non-metallic, 1 = metallic
				float Smoothness;                 // 0 = rough, 1 = smooth
			};

			class Material : public DX12Resource
			{
			public:
				Material() = default;

				void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
				void Unbind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
			private:
				bool Load(const std::string& a_FilePath, const MaterialData& a_MaterialData);

				MaterialData m_MaterialData;

				friend class ResourceAtlas;
			};
		}
	}
}