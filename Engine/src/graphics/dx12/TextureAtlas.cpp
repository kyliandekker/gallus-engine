#include "graphics/dx12/TextureAtlas.h"

#include <algorithm>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			Texture& TextureAtlas::LoadTexture(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				std::wstring name = std::wstring(a_Path.begin(), a_Path.end());
				Texture* tex = nullptr;
				for (size_t i = 0; i < m_Textures.size(); i++)
				{
					if (m_Textures[i] && m_Textures[i]->GetName() == name)
					{
						tex = m_Textures[i];
						break;
					}
					if (!m_Textures[i])
					{
						break;
					}
				}
				if (!tex)
				{
					for (size_t i = 0; i < m_Textures.size(); i++)
					{
						Texture* texture = m_Textures[i];
						if (!m_Textures[i])
						{
							m_Textures[i] = new Texture();
							m_Textures[i]->LoadTexture(a_Path, a_CommandList);

							tex = m_Textures[i];
							break;
						}
					}
				}
				return *tex;
			}
		}
	}
}