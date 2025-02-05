#pragma once

#include "graphics/dx12/DX12PCH.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class Shader;
			class Material;
			class Mesh;
			class Transform;

			class CommandList;

			class MeshComponent
			{
			public:
				MeshComponent() = default;
				void Initialize();

				void SetMesh(Mesh& a_Mesh);
				void SetShader(Shader& a_Shader);
				void SetTexture(Texture& a_Texture);
				void SetMaterial(Material& a_Material);

				void Render(std::shared_ptr<CommandList> a_CommandList, const Transform& a_Transform, const DirectX::XMMATRIX& a_CameraView, const DirectX::XMMATRIX& a_CameraProjection);
			private:
				Mesh* m_Mesh;
				Shader* m_Shader = nullptr;
				Texture* m_Texture = nullptr;
				Material* m_Material = nullptr;

				friend class ResourceAtlas;
			};
		}
	}
}