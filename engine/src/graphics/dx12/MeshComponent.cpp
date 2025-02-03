#include "graphics/dx12/MeshComponent.h"

#include "core/Engine.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/CommandList.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			void MeshComponent::Initialize()
			{
				// TODO: Default model and shader.
				m_Material = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultMaterial();
				m_Texture = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultTexture();
				//m_Shader = &core::ENGINE.GetDX12().GetResourceAtlas().GetDefaultShader();
			}

			void MeshComponent::SetMesh(Mesh& a_Mesh)
			{
				m_Mesh = &a_Mesh;
			}

			void MeshComponent::SetShader(Shader& a_Shader)
			{
				m_Shader = &a_Shader;
			}

			void MeshComponent::SetTexture(Texture& a_Texture)
			{
				m_Texture = &a_Texture;
			}

			void MeshComponent::SetMaterial(Material& a_Material)
			{
				m_Material = &a_Material;
			}

			void MeshComponent::Render(std::shared_ptr<CommandList> a_CommandList, const Transform& a_Transform, const DirectX::XMMATRIX& a_CameraView, const DirectX::XMMATRIX& a_CameraProjection)
			{
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
					m_Mesh->Render(a_CommandList, a_Transform, a_CameraView, a_CameraProjection);
				}

				if (m_Texture && m_Texture->IsValid())
				{
					m_Texture->Unbind(a_CommandList);
				}
			}
		}
	}
}