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
			template<class T>
			T* GetResource(std::vector<T*>& a_Vector, const std::wstring& a_Name, const fs::path& a_Path, size_t a_StartIndex = MISSING)
			{
				size_t index = a_StartIndex;
				for (size_t i = a_StartIndex; i < a_Vector.size(); i++)
				{
					if (a_Vector[i])
					{
						if (!a_Vector[i]->IsValid())
						{
							index = i;
						}
						else if (a_Vector[i]->GetName() == a_Name || (!a_Path.empty() && a_Path == a_Vector[i]->GetPath()))
						{
							index = i;
							break;
						}
					}
					else
					{
						index = i;
						break;
					}
				}
				if (!a_Vector[index])
				{
					a_Vector[index] = new T();
				}
				return a_Vector[index];
			}

			Mesh& ResourceAtlas::LoadMeshByName(const std::wstring& a_Name, std::shared_ptr<CommandList> a_CommandList)
			{
				Mesh* mesh = GetResource(m_Meshes, a_Name, fs::path());
				if (!mesh->IsValid())
				{
					mesh->LoadByName(a_Name, a_CommandList);
				}
				return *mesh;
			}

			Mesh& ResourceAtlas::LoadMeshByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_CommandList)
			{
				Mesh* mesh = GetResource(m_Meshes, a_Path.stem().generic_wstring(), a_Path);
				if (!mesh->IsValid())
				{
					mesh->LoadByPath(a_Path, a_CommandList);
				}
				return *mesh;
			}

			Texture& ResourceAtlas::LoadTextureByName(const std::wstring& a_Name, std::shared_ptr<CommandList> a_CommandList)
			{
				Texture* texture = GetResource(m_Textures, a_Name, fs::path(), MISSING_TEX);
				if (!texture->IsValid())
				{
					texture->LoadByName(a_Name, a_CommandList);
				}
				return *texture;
			}

			Texture& ResourceAtlas::LoadTextureByDescription(const std::wstring& a_Name, D3D12_RESOURCE_DESC& a_Description)
			{
				Texture* texture = GetResource(m_Textures, a_Name, fs::path(), MISSING_TEX);
				if (!texture->IsValid())
				{
					texture->LoadByName(a_Name, a_Description);
				}
				return *texture;
			}

			Texture& ResourceAtlas::LoadTextureByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_CommandList)
			{
				Texture* texture = GetResource(m_Textures, a_Path.stem().generic_wstring(), a_Path, MISSING_TEX);
				if (!texture->IsValid())
				{
					texture->LoadByPath(a_Path, a_CommandList);
				}
				return *texture;
			}

			Shader& ResourceAtlas::LoadShaderByName(const std::wstring& a_VertexShader, const std::wstring& a_PixelShader)
			{
				Shader* shader = GetResource(m_Shaders, a_VertexShader, fs::path());
				if (!shader->IsValid())
				{
					shader->LoadByName(a_VertexShader, a_PixelShader);
				}
				return *shader;
			}

			Shader& ResourceAtlas::LoadShaderByPath(const fs::path& a_VertexShaderPath, const fs::path& a_PixelShaderPath)
			{
				Shader* shader = GetResource(m_Shaders, a_VertexShaderPath.stem(), a_VertexShaderPath);
				if (!shader->IsValid())
				{
					shader->LoadByPath(a_VertexShaderPath, a_PixelShaderPath);
				}
				return *shader;
			}

			Material& ResourceAtlas::LoadMaterialByName(const std::wstring& a_Name)
			{
				// TODO:
				return *m_Materials[MISSING];
			}

			Material& ResourceAtlas::LoadMaterialByName(const std::wstring& a_Name, const MaterialData& a_MaterialData)
			{
				Material* material = GetResource(m_Materials, a_Name, fs::path());
				if (!material->IsValid())
				{
					material->LoadByName(a_Name, a_MaterialData);
				}
				return *material;
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