#include "graphics/dx12/Mesh.h"

#include <tiny_gltf/tiny_gltf.h>
#include <stb_image.h>
#include <filesystem>

#include "core/Engine.h"
#include "core/FileUtils.h"
#include "core/DataStream.h"
#include "core/logger/Logger.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{

			Mesh::Mesh()
			{ }

            bool Mesh::LoadMesh(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
            {
				// Upload vertex buffer data.
				core::DataStream data;
				if (!file::FileLoader::LoadFile(a_Path, data))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading mesh file %s.", a_Path.c_str());
					return false;
				}

				tinygltf::Model model;
				tinygltf::TinyGLTF loader;
				std::string err, warn;
				if (!loader.LoadASCIIFromString(&model, &err, &warn, data.dataAs<const char>(), data.size(), std::filesystem::path(a_Path).parent_path().string()))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading mesh file %s.", err.c_str());
					return false;
				}

				m_MeshData.reserve(model.meshes.size());
				for (const auto& mesh : model.meshes)
				{
					MeshData* meshData = new MeshData();

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
						&meshData->m_VertexBuffer, &meshData->intermediateVertexBuffer,
						meshData->m_Vertices.size(), sizeof(VertexPosColorUV), meshData->m_Vertices.data());

					// Create the vertex buffer view.
					meshData->m_VertexBufferView.BufferLocation = meshData->m_VertexBuffer->GetGPUVirtualAddress();
					meshData->m_VertexBufferView.SizeInBytes = sizeof(VertexPosColorUV) * meshData->m_Vertices.size();
					meshData->m_VertexBufferView.StrideInBytes = sizeof(VertexPosColorUV);

					// Upload index buffer data.
					UpdateBufferResource(a_CommandList,
						&meshData->m_IndexBuffer, &meshData->intermediateIndexBuffer,
						meshData->m_Indices.size(), indexSize, meshData->m_Indices.data());

					// Create index buffer view.
					meshData->m_IndexBufferView.BufferLocation = meshData->m_IndexBuffer->GetGPUVirtualAddress();
					meshData->m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
					meshData->m_IndexBufferView.SizeInBytes = indexSize * meshData->m_Indices.size();
				}

                return true;
            }

			bool Mesh::SetShader(Shader& a_Shader)
			{
				m_Shader = a_Shader;
				return true;
			}

            void Mesh::UpdateBufferResource(
                Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList,
                ID3D12Resource** a_pDestinationResource,
                ID3D12Resource** a_pIntermediateResource,
                size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData,
                D3D12_RESOURCE_FLAGS a_Flags)
            {
                auto device = core::ENGINE.GetDX12().GetDevice();

                size_t bufferSize = a_NumElements * a_ElementSize;

                CD3DX12_HEAP_PROPERTIES defaultHeaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
                CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, a_Flags);
                // Create a committed resource for the GPU resource in a default heap.
                if (FAILED(device->CreateCommittedResource(
                    &defaultHeaptype,
                    D3D12_HEAP_FLAG_NONE,
                    &buffer,
                    D3D12_RESOURCE_STATE_COMMON,
                    nullptr,
                    IID_PPV_ARGS(a_pDestinationResource))))
                {
                    LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource.");
                    return;
                }

                // Create an committed resource for the upload.
                if (a_BufferData)
                {
                    CD3DX12_HEAP_PROPERTIES uploadHeaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                    CD3DX12_RESOURCE_DESC buff = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
                    if (FAILED(device->CreateCommittedResource(
                        &uploadHeaptype,
                        D3D12_HEAP_FLAG_NONE,
                        &buff,
                        D3D12_RESOURCE_STATE_GENERIC_READ,
                        nullptr,
                        IID_PPV_ARGS(a_pIntermediateResource))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource.");
                        return;
                    }

                    D3D12_SUBRESOURCE_DATA subresourceData = {};
                    subresourceData.pData = a_BufferData;
                    subresourceData.RowPitch = bufferSize;
                    subresourceData.SlicePitch = subresourceData.RowPitch;

                    UpdateSubresources(a_CommandList.Get(),
                        *a_pDestinationResource, *a_pIntermediateResource,
                        0, 0, 1, &subresourceData);
                }
            }

			Transform& Mesh::GetTransform()
			{
				return m_Transform;
			}

			void Mesh::Update(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
            {
            }

            void Mesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
			{
				m_Shader.Bind(a_CommandList);

				for (auto& meshData : m_MeshData)
				{
					a_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					a_CommandList->IASetVertexBuffers(0, 1, &meshData->m_VertexBufferView);
					a_CommandList->IASetIndexBuffer(&meshData->m_IndexBufferView);

					// Update the MVP matrix
					DirectX::XMMATRIX mvpMatrix = m_Transform.GetWorldMatrix() * view * projection;
					a_CommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

					a_CommandList->DrawIndexedInstanced(meshData->m_Indices.size(), 1, 0, 0, 0);
				}
			}
		}
	}
}