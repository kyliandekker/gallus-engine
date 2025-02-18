#pragma once

#include "graphics/dx12/DX12PCH.h"

#include "gameplay/systems/components/Component.h"

#include <memory>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

			class Transform;
			class Mesh;
			class Shader;
			class Texture;
			class Material;
		}
	}
	namespace gameplay
	{
		struct EntityID;

		class MeshComponent : public Component
		{
		public:
			MeshComponent();

			void SetMesh(graphics::dx12::Mesh& a_Mesh);
			void SetShader(graphics::dx12::Shader& a_Shader);
			void SetTexture(graphics::dx12::Texture& a_Texture);
			void SetMaterial(graphics::dx12::Material& a_Material);

			graphics::dx12::Mesh* GetMesh() const;
			graphics::dx12::Shader* GetShader();
			graphics::dx12::Texture* GetTexture();
			graphics::dx12::Material* GetMaterial();

			void Render(std::shared_ptr<graphics::dx12::CommandList> a_CommandList, const EntityID& a_EntityID, const DirectX::XMMATRIX& a_CameraView, const DirectX::XMMATRIX& a_CameraProjection);

			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
			void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) override;
		private:
			graphics::dx12::Mesh* m_Mesh = nullptr;
			graphics::dx12::Shader* m_Shader = nullptr;
			graphics::dx12::Texture* m_Texture = nullptr;
			graphics::dx12::Material* m_Material = nullptr;
		};
	}
}