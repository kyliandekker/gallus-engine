#include "gameplay/systems/components/MeshComponent.h"

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Transform.h"
#include "core/Engine.h"

#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace gameplay
	{
		MeshComponent::MeshComponent()
		{
			m_Mesh = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultMesh();
			m_Shader = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultShader();
			m_Texture = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultTexture();
			m_Material = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultMaterial();
		}

		void MeshComponent::SetMesh(graphics::dx12::Mesh& a_Mesh)
		{
			m_Mesh = &a_Mesh;
		}

		void MeshComponent::SetShader(graphics::dx12::Shader& a_Shader)
		{
			m_Shader = &a_Shader;
		}

		void MeshComponent::SetTexture(graphics::dx12::Texture& a_Texture)
		{
			m_Texture = &a_Texture;
		}

		void MeshComponent::SetMaterial(graphics::dx12::Material& a_Material)
		{
			m_Material = &a_Material;
		}

		graphics::dx12::Mesh* MeshComponent::GetMesh() const
		{
			return m_Mesh;
		}

		graphics::dx12::Shader* MeshComponent::GetShader()
		{
			return m_Shader;
		}

		graphics::dx12::Texture* MeshComponent::GetTexture()
		{
			return m_Texture;
		}

		graphics::dx12::Material* MeshComponent::GetMaterial()
		{
			return m_Material;
		}

		void MeshComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_CommandList, const EntityID& a_EntityID, const DirectX::XMMATRIX& a_CameraView, const DirectX::XMMATRIX& a_CameraProjection)
		{
			graphics::dx12::Transform transform;
			transform.SetPosition({ 0.0f, -0.5f, 1.5f });

			if (core::ENGINE.GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(a_EntityID))
			{
				transform = core::ENGINE.GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(a_EntityID).Transform();
			}

			if (m_Texture && m_Texture->IsValid())
			{
				m_Texture->Bind(a_CommandList);
			}

			if (m_Material && m_Material->IsValid())
			{
				m_Material->Bind(a_CommandList);
			}

			if (m_Shader)
			{
				m_Shader->Bind(a_CommandList);
			}

			if (m_Mesh)
			{
				m_Mesh->Render(a_CommandList, transform, a_CameraView, a_CameraProjection);
			}

			if (m_Texture && m_Texture->IsValid())
			{
				m_Texture->Unbind(a_CommandList);
			}
		}

		// TODO:
		void MeshComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{}

		void MeshComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{}
	}
}