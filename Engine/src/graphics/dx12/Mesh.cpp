#include "graphics/dx12/Mesh.h"

#include <tiny_gltf/tiny_gltf.h>

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
				if (!loader.LoadASCIIFromString(&model, &err, &warn, data.dataAs<const char>(), data.size(), "C:/resources/"))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading mesh file %s.", a_Path.c_str());
					return false;
				}

				for (const auto& mesh : model.meshes) {
					for (const auto& primitive : mesh.primitives) {
						const auto& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
						const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
						const auto& posBuffer = model.buffers[posBufferView.buffer];

						const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

						const float* colors = nullptr;
						if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
							const auto& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
							const auto& colorBufferView = model.bufferViews[colorAccessor.bufferView];
							const auto& colorBuffer = model.buffers[colorBufferView.buffer];
							colors = reinterpret_cast<const float*>(&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
						}

						for (size_t i = 0; i < posAccessor.count; ++i) {
							DirectX::XMFLOAT3 position(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
							DirectX::XMFLOAT3 color = colors ? DirectX::XMFLOAT3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]) : DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
							m_Vertices.push_back({ position, color });
						}

						// Extract indices
						const auto& indexAccessor = model.accessors[primitive.indices];
						const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
						const auto& indexBuffer = model.buffers[indexBufferView.buffer];

						if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
							const uint16_t* buf = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
							m_Indices.insert(m_Indices.end(), buf, buf + indexAccessor.count);
						}
						else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
							const uint32_t* buf = reinterpret_cast<const uint32_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
							m_Indices.insert(m_Indices.end(), buf, buf + indexAccessor.count);
						}
					}
				}

				// Upload vertex buffer data.
				UpdateBufferResource(a_CommandList,
					&m_VertexBuffer, &intermediateVertexBuffer,
					m_Vertices.size(), sizeof(VertexPosColorUV), m_Vertices.data());

				// Create the vertex buffer view.
				m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
				m_VertexBufferView.SizeInBytes = sizeof(VertexPosColorUV) * m_Vertices.size();
				m_VertexBufferView.StrideInBytes = sizeof(VertexPosColorUV);

				// Upload index buffer data.
				UpdateBufferResource(a_CommandList,
					&m_IndexBuffer, &intermediateIndexBuffer,
					m_Indices.size(), sizeof(WORD), m_Indices.data());

				// Create index buffer view.
				m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
				m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
				m_IndexBufferView.SizeInBytes = sizeof(uint16_t) * m_Indices.size();

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
                a_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                a_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
                a_CommandList->IASetIndexBuffer(&m_IndexBufferView);
            }

            void Mesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
			{
				// Update the MVP matrix
				DirectX::XMMATRIX mvpMatrix = m_Transform.GetWorldMatrix() * view * projection;
				a_CommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

                a_CommandList->DrawIndexedInstanced(m_Indices.size(), 1, 0, 0, 0);
			}
		}
	}
}