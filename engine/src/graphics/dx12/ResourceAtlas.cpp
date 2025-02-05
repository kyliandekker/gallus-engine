#include "graphics/dx12/ResourceAtlas.h"

#include <algorithm>

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/CommandList.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			template<class T, typename Arg>
			T* ResourceAtlas::GetResource(std::vector<T*>& a_Vector, const std::string& a_Name, const Arg& a_Arg)
			{
				std::wstring name = std::wstring(a_Name.begin(), a_Name.end());
				T* res = nullptr;
				for (size_t i = 0; i < a_Vector.size(); i++)
				{
					if (a_Vector[i] && a_Vector[i]->GetName() == name)
					{
						if (!a_Vector[i]->IsValid())
						{
							a_Vector[i]->Load(a_Name, a_Arg);
						}
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
				return res;
			}

			Mesh& ResourceAtlas::LoadMesh(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList)
			{
				Mesh* mesh = GetResource(m_Meshes, a_Name, a_CommandList);
				return *(mesh ? mesh : m_Meshes[MISSING]);
			}

			Texture& ResourceAtlas::LoadTexture(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList)
			{
				Texture* texture = GetResource(m_Textures, a_Name, a_CommandList);
				return *(texture ? texture : m_Textures[MISSING_TEX]);
			}

			Texture& ResourceAtlas::LoadTextureByDescription(const std::string& a_Name, D3D12_RESOURCE_DESC& a_Description)
			{
				Texture* texture = GetResource(m_Textures, a_Name, a_Description);
				return *(texture ? texture : m_Textures[MISSING_TEX]);
			}

			Shader& ResourceAtlas::LoadShader(const std::string& a_Name)
			{
				Shader* shader = GetResource(m_Shaders, a_Name, nullptr);
				return *(shader ? shader : m_Shaders[MISSING]);
			}

			Material& ResourceAtlas::LoadMaterial(const std::string& a_Name, const MaterialData& a_MaterialData)
			{
				Material* material = GetResource(m_Materials, a_Name, a_MaterialData);
				return *(material ? material : m_Materials[MISSING]);
			}

			Texture& ResourceAtlas::GetDefaultTexture()
			{
				return *m_Textures[MISSING_TEX];
			}

			Material& ResourceAtlas::GetDefaultMaterial()
			{
				return *m_Materials[MISSING];
			}

			void ResourceAtlas::TransitionResources(std::shared_ptr<CommandList> a_CommandList)
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