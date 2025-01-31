#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <vector>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			constexpr uint32_t MAX_RESOURCES = 65;
			constexpr uint32_t MISSING = 0;

			class Texture;
			class Mesh;
			class Shader;
			class Material;
			struct MaterialData;

			class CommandList;

			class ResourceAtlas
			{
			public:
				template<class T, typename Arg>
				T& GetResource(std::vector<T*>& a_Vector, const std::string& a_Name, Arg a_Arg);

				Mesh& LoadMesh(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList);
				Texture& LoadTexture(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList);
				Shader& LoadShader(const std::string& a_Name);
				Material& LoadMaterial(const std::string& a_Name, const MaterialData& a_MaterialData);

				Texture& GetDefaultTexture();
				Material& GetDefaultMaterial();

				void TransitionResources(std::shared_ptr<CommandList> a_CommandList);
			private:
				std::vector<Texture*> m_Textures = std::vector<Texture*>(MAX_RESOURCES);
				std::vector<Mesh*> m_Meshes = std::vector<Mesh*>(MAX_RESOURCES);
				std::vector<Shader*> m_Shaders = std::vector<Shader*>(MAX_RESOURCES);
				std::vector<Material*> m_Materials = std::vector<Material*>(MAX_RESOURCES);
			};
		}
	}
}