#include "graphics/dx12/ResourceAtlas.h"

#include <algorithm>

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Material.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			template<class T, typename Arg>
			T& ResourceAtlas::GetResource(std::vector<T*>& a_Vector, const std::string& a_Name, Arg a_Arg)
			{
				std::wstring name = std::wstring(a_Name.begin(), a_Name.end());
				T* res = nullptr;
				for (size_t i = 0; i < a_Vector.size(); i++)
				{
					if (a_Vector[i] && a_Vector[i]->GetName() == name)
					{
						res = a_Vector[i];
						break;
					}
					if (!a_Vector[i])
					{
						break;
					}
				}
				if (!res)
				{
					for (size_t i = 0; i < a_Vector.size(); i++)
					{
						T* texture = a_Vector[i];
						if (!a_Vector[i])
						{
							a_Vector[i] = new T();
							a_Vector[i]->Load(a_Name, a_Arg);

							res = a_Vector[i];
							break;
						}
					}
				}
				if (!res)
				{
					res = a_Vector[MISSING];
				}
				return *res;
			}

			Mesh& ResourceAtlas::LoadMesh(const std::string& a_Name, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				return GetResource(m_Meshes, a_Name, a_CommandList);
			}

			Texture& ResourceAtlas::LoadTexture(const std::string& a_Name, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				return GetResource(m_Textures, a_Name, a_CommandList);
			}

			Shader& ResourceAtlas::LoadShader(const std::string& a_Name)
			{
				return GetResource(m_Shaders, a_Name, nullptr);
			}

			Material& ResourceAtlas::LoadMaterial(const std::string& a_Name, const MaterialData& a_MaterialData)
			{
				return GetResource(m_Materials, a_Name, a_MaterialData);
			}

			Texture& ResourceAtlas::GetDefaultTexture()
			{
				return *m_Textures[MISSING];
			}

			Material& ResourceAtlas::GetDefaultMaterial()
			{
				return *m_Materials[MISSING];
			}

			void ResourceAtlas::TransitionResources(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				for (auto texture : m_Textures)
				{
					if (texture && texture->IsValid())
					{
						texture->Transition(a_CommandList);
					}
				}
			}
		}
	}
}