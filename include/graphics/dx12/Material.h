#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

#include <memory>

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

			class CommandList;

			class Material : public DX12Resource
			{
			public:
				Material() = default;

				void Bind(std::shared_ptr<CommandList> a_CommandList);
				void Unbind(std::shared_ptr<CommandList> a_CommandList);
			private:
				bool Load(const std::string& a_Name, const MaterialData& a_MaterialData);

				MaterialData m_MaterialData;

				friend class ResourceAtlas;
			};
		}
	}
}