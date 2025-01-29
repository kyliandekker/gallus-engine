#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <vector>
#include "graphics/dx12/Texture.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class TextureAtlas
			{
			public:
				Texture& LoadTexture(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
			private:
				std::vector<Texture*> m_Textures = std::vector<Texture*>(65);
			};
		}
	}
}