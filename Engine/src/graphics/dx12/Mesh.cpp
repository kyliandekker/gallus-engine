#include "graphics/dx12/Mesh.h"

#include <tiny_gltf/tiny_gltf.h>
#include <stb_image.h>
#include <filesystem>
#include <format>

#include "core/Engine.h"
#include "core/FileUtils.h"
#include "core/DataStream.h"
#include "core/logger/Logger.h"
#include "graphics/dx12/Texture.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			Mesh::Mesh() : DX12Resource()
			{ }

            bool Mesh::LoadMesh(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
            {
				// Upload vertex buffer data.
				core::DataStream data;
				if (!file::FileLoader::LoadFile(a_Path, data))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading gltf file %s.", a_Path.c_str());
					return false;
				}

				tinygltf::Model model;
				tinygltf::TinyGLTF loader;
				std::string err, warn;
				if (!loader.LoadASCIIFromString(&model, &err, &warn, data.dataAs<const char>(), data.size(), std::filesystem::path(a_Path).parent_path().string()))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading bin file %s.", err.c_str());
					return false;
				}

				m_MeshData.reserve(model.meshes.size());
				for (const auto& mesh : model.meshes)
				{
					MeshData* meshData = new MeshData();

					std::wstring name = std::format(L"{0}_{1}", std::filesystem::path(a_Path).stem().wstring(), std::wstring(mesh.name.begin(), mesh.name.end()));
					meshData->m_VertexBuffer = VertexBuffer(name);

					size_t indexSize = 0;
					for (const auto& primitive : mesh.primitives)
					{
						const auto& indexAccessor = model.accessors[primitive.indices];
						const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
						const auto& indexBuffer = model.buffers[indexBufferView.buffer];

						// Determine index type and load data
						switch (indexAccessor.componentType)
						{
							case TINYGLTF_COMPONENT_TYPE_SHORT:
							{
								indexSize = sizeof(int16_t);
								break;
							}
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
							{
								indexSize = sizeof(uint16_t);
								break;
							}
							case TINYGLTF_COMPONENT_TYPE_INT:
							{
								indexSize = sizeof(int32_t);
								break;
							}
							case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
							{
								indexSize = sizeof(uint32_t);
								break;
							}
							case TINYGLTF_COMPONENT_TYPE_FLOAT:
							{
								indexSize = sizeof(float);
								break;
							}
						}

						const auto& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
						const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
						const auto& posBuffer = model.buffers[posBufferView.buffer];

						const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

						const float* colors = nullptr;
						if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
						{
							const auto& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
							const auto& colorBufferView = model.bufferViews[colorAccessor.bufferView];
							const auto& colorBuffer = model.buffers[colorBufferView.buffer];
							colors = reinterpret_cast<const float*>(&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
						}

						const float* uvs = nullptr;
						if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
						{
							const auto& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
							const auto& uvBufferView = model.bufferViews[uvAccessor.bufferView];
							const auto& uvBuffer = model.buffers[uvBufferView.buffer];
							uvs = reinterpret_cast<const float*>(&uvBuffer.data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
						}

						for (size_t i = 0; i < posAccessor.count; ++i)
						{
							DirectX::XMFLOAT3 position(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
							DirectX::XMFLOAT3 color = colors ? DirectX::XMFLOAT3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]) : DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
							DirectX::XMFLOAT2 uv = uvs ? DirectX::XMFLOAT2(uvs[i * 2], uvs[i * 2 + 1]) : DirectX::XMFLOAT2(0.0f, 0.0f);
							meshData->m_Vertices.push_back({ position, color, uv });
						}

						const uint8_t* indices = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];
						meshData->m_Indices.resize(indexAccessor.count * indexSize);
						std::memcpy(meshData->m_Indices.data(), indices, indexAccessor.count * indexSize);
					}
					m_MeshData.push_back(meshData);

					// Upload vertex buffer data.
					UpdateBufferResource(a_CommandList,
						&meshData->m_VertexBuffer.GetResource(), &meshData->intermediateVertexBuffer,
						meshData->m_Vertices.size(), sizeof(VertexPosColorUV), meshData->m_Vertices.data());

					// Create the vertex buffer view.
					meshData->m_VertexBuffer.CreateViews(meshData->m_Vertices.size(), sizeof(VertexPosColorUV));

					// Upload index buffer data.
					UpdateBufferResource(a_CommandList,
						&meshData->m_IndexBuffer.GetResource(), &meshData->intermediateIndexBuffer,
						meshData->m_Indices.size(), indexSize, meshData->m_Indices.data());

					// Create index buffer view.
					meshData->m_IndexBuffer.CreateViews(meshData->m_Indices.size(), indexSize);
				}

                return true;
            }

			bool Mesh::LoadTexture(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				m_Texture = &core::ENGINE.GetDX12().GetTextureAtlas().LoadTexture(a_Path, a_CommandList);
				return m_Texture;
			}

			bool Mesh::Transition(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				return m_Texture->Transition(a_CommandList);
			}

			bool Mesh::SetShader(Shader& a_Shader)
			{
				m_Shader = a_Shader;
				return true;
			}

			void Mesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, const Transform& a_Transform, DirectX::XMMATRIX a_CameraView, DirectX::XMMATRIX a_CameraProjection)
			{
				if (m_Texture && m_Texture->IsValid())
				{
					m_Texture->Bind(a_CommandList);
				}

				m_Shader.Bind(a_CommandList);
				for (auto& meshData : m_MeshData)
				{
					a_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					a_CommandList->IASetVertexBuffers(0, 1, &meshData->m_VertexBuffer.GetVertexBufferView());
					a_CommandList->IASetIndexBuffer(&meshData->m_IndexBuffer.GetIndexBufferView());

					// Update the MVP matrix
					DirectX::XMMATRIX mvpMatrix = a_Transform.GetWorldMatrix() * a_CameraView * a_CameraProjection;
					a_CommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

					a_CommandList->DrawIndexedInstanced(meshData->m_Indices.size(), 1, 0, 0, 0);
				}

				if (m_Texture && m_Texture->IsValid())
				{
					m_Texture->Unbind(a_CommandList);
				}
			}
		}
	}
}