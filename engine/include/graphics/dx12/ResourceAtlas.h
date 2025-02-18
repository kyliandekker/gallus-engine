#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <vector>

#include "core/FileUtils.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			constexpr uint32_t MAX_RESOURCES = 64;
			constexpr uint32_t MISSING = 0;
#ifdef _EDITOR
			constexpr uint32_t MISSING_TEX = 1;
#else
			constexpr uint32_t MISSING_TEX = 0;
#endif // _EDITOR

			class Texture;
			class Mesh;
			class Shader;
			class Material;
			struct MaterialData;

			class CommandList;

			class ResourceAtlas
			{
			public:
				Mesh& LoadMeshByName(const std::wstring& a_Name, std::shared_ptr<CommandList> a_CommandList);
				Mesh& LoadMeshByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_CommandList);

				Texture& LoadTextureByName(const std::wstring& a_Name, std::shared_ptr<CommandList> a_CommandList);
				Texture& LoadTextureByDescription(const std::wstring& a_Name, D3D12_RESOURCE_DESC& a_Description);
				Texture& LoadTextureByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_CommandList);

				Shader& LoadShaderByName(const std::wstring& a_VertexShader, const std::wstring& a_PixelShader);
				Shader& LoadShaderByPath(const fs::path& a_VertexShaderPath, const fs::path& a_PixelShaderPath);

				Material& LoadMaterialByName(const std::wstring& a_Name);
				Material& LoadMaterialByName(const std::wstring& a_Name, const MaterialData& a_MaterialData);

				Shader& GetDefaultShader();
				Mesh& GetDefaultMesh();
				Texture& GetDefaultTexture();
				Material& GetDefaultMaterial();

				void TransitionResources(std::shared_ptr<CommandList> a_CommandList);

				const std::vector<Texture*>& GetTextures() const;
				const std::vector<Mesh*>& GetMeshes() const;
				const std::vector<Shader*>& GetShaders() const;
				const std::vector<Material*>& GetMaterials() const;
			private:
				std::vector<Texture*> m_Textures = std::vector<Texture*>(MAX_RESOURCES);
				std::vector<Mesh*> m_Meshes = std::vector<Mesh*>(MAX_RESOURCES);
				std::vector<Shader*> m_Shaders = std::vector<Shader*>(MAX_RESOURCES);
				std::vector<Material*> m_Materials = std::vector<Material*>(MAX_RESOURCES);
			};
		}
	}
}