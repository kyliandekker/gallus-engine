#include "graphics/dx12/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/Engine.h"
#include "core/logger/Logger.h"
#include "graphics/dx12/CommandList.h"
#include "core/FileUtils.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			Texture::~Texture()
			{
				m_Resource.Reset();
			}

			void Texture::Destroy()
			{
				core::ENGINE.GetDX12().GetSRV().Deallocate(m_SRVIndex);
				m_Resource.Reset();
			}

			bool Texture::CheckSRVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
			}

			bool Texture::CheckRTVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
			}

			bool Texture::CheckUAVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
					CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
					CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
			}

			bool Texture::CheckDSVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
			}

			// TODO: This all needs to be loaded from a file eventually instead of from files on the disk.
			bool Texture::Load(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList)
			{
				fs::path path = file::FileLoader::GetPath(std::format("./assets/textures/{0}", a_Name.c_str()));

				int width, height, channels;
				stbi_uc* imageData = stbi_load(path.generic_string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
				if (!imageData)
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load texture: \"%s\".", path.generic_string().c_str());
					return false;
				}

				std::wstring name = std::wstring(a_Name.begin(), a_Name.end());

				D3D12_RESOURCE_DESC textureDesc = {};
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				textureDesc.Width = width;
				textureDesc.Height = height;
				textureDesc.DepthOrArraySize = 1;
				textureDesc.MipLevels = 1;
				textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
				textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

				CreateResource(textureDesc, name);

				UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Resource.Get(), 0, 1);
				CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC bufferResource = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
				if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreateCommittedResource(
					&uploadHeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&bufferResource,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_ResourceUploadHeap))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create upload heap.");
					return false;
				}

				D3D12_SUBRESOURCE_DATA textureData = {};
				textureData.pData = imageData;
				textureData.RowPitch = width * channels;
				textureData.SlicePitch = textureData.RowPitch * height;
				UpdateSubresources(a_CommandList->GetCommandList().Get(), m_Resource.Get(), m_ResourceUploadHeap.Get(), 0, 0, 1, &textureData);

				stbi_image_free(imageData);

				LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded texture: \"%s\".", path.generic_string().c_str());
				return true;
			}

			bool Texture::Load(const std::string& a_Name, const D3D12_RESOURCE_DESC& a_Description)
			{
				return CreateResource(a_Description, std::wstring(a_Name.begin(), a_Name.end()));
			}

			bool Texture::Transition(std::shared_ptr<CommandList> a_CommandList)
			{
				CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					m_Resource.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				a_CommandList->GetCommandList()->ResourceBarrier(1, &barrier);

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = -1;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				m_SRVIndex = core::ENGINE.GetDX12().GetSRV().Allocate();
				core::ENGINE.GetDX12().GetDevice()->CreateShaderResourceView(m_Resource.Get(), &srvDesc, core::ENGINE.GetDX12().GetSRV().GetCPUHandle(m_SRVIndex));

				return true;
			}

			void Texture::Bind(std::shared_ptr<CommandList> a_CommandList)
			{
				a_CommandList->GetCommandList()->SetDescriptorHeaps(1, core::ENGINE.GetDX12().GetSRV().GetHeap().GetAddressOf());

				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = core::ENGINE.GetDX12().GetSRV().GetGPUHandle(m_SRVIndex);

				a_CommandList->GetCommandList()->SetGraphicsRootDescriptorTable(1, gpuHandle);
			}

			void Texture::Unbind(std::shared_ptr<CommandList> a_CommandList)
			{}

			CD3DX12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUHandle()
			{
				return core::ENGINE.GetDX12().GetSRV().GetGPUHandle(m_SRVIndex);
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle()
			{
				return core::ENGINE.GetDX12().GetSRV().GetCPUHandle(m_SRVIndex);
			}
		}
	}
}