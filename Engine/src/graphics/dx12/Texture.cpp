#include "graphics/dx12/Texture.h"

#include <stb_image.h>

#include "core/Engine.h"

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

            bool Texture::LoadTexture(const std::wstring& a_FilePath, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
            {
                if (FAILED(LoadFromTGAFile(a_FilePath.c_str(), &metadata, scratchImage)))
                {
                    LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Failed loading tga.");
                    return false;
                }

                D3D12_RESOURCE_DESC textureDesc = {};
                textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, static_cast<UINT64>(metadata.width),
                    static_cast<UINT>(metadata.height),
                    static_cast<UINT16>(metadata.arraySize));

                CreateResource(textureDesc, a_FilePath);

                subresources = std::vector<D3D12_SUBRESOURCE_DATA>(scratchImage.GetImageCount());
                pImages = scratchImage.GetImages();
                for (int i = 0; i < scratchImage.GetImageCount(); ++i)
                {
                    auto& subresource = subresources[i];
                    subresource.RowPitch = pImages[i].rowPitch;
                    subresource.SlicePitch = pImages[i].slicePitch;
                    subresource.pData = pImages[i].pixels;
                }

                CopyTextureSubresource(a_CommandList, 0, static_cast<uint32_t>(subresources.size()), subresources.data());

                LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "SRV created successfully.");
                return true;
            }

            bool Texture::Transition(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
            {
                CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                    m_Resource.Get(),
                    D3D12_RESOURCE_STATE_COPY_DEST,
                    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                a_CommandList->ResourceBarrier(1, &barrier);

                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MostDetailedMip = 0;
                srvDesc.Texture2D.MipLevels = -1;
                srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

                srvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(core::ENGINE.GetDX12().m_SRVHeap->GetCPUDescriptorHandleForHeapStart());

                // Offset by the index for the SRV
                srvHandle.Offset(0, core::ENGINE.GetDX12().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

                core::ENGINE.GetDX12().GetDevice()->CreateShaderResourceView(m_Resource.Get(), &srvDesc, srvHandle);

                return false;
            }

            void Texture::CopyTextureSubresource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, uint32_t a_FirstSubresource,
                uint32_t a_NumSubresources, D3D12_SUBRESOURCE_DATA* a_SubresourceData)
            {
                auto d3d12Device = core::ENGINE.GetDX12().GetDevice();

                if (m_Resource)
                {
                    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

                    UINT64 requiredSize =
                        GetRequiredIntermediateSize(m_Resource.Get(), a_FirstSubresource, a_NumSubresources);

                    // Create a temporary (intermediate) resource for uploading the subresources
                    CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                    CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
                    if (FAILED(d3d12Device->CreateCommittedResource(
                        &uploadHeap, D3D12_HEAP_FLAG_NONE,
                        &buffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                        IID_PPV_ARGS(&intermediateResource))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed uploading texture.");
                        return;
                    }

                    if (FAILED(UpdateSubresources(a_CommandList.Get(), m_Resource.Get(), intermediateResource.Get(), 0,
                        a_FirstSubresource, a_NumSubresources, a_SubresourceData)))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed updating sub resources for texture.");
                        return;
                    }
                }
            }

            void Texture::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
            {
                a_CommandList->SetGraphicsRootSignature(core::ENGINE.GetDX12().GetRootSignature().Get());
                a_CommandList->SetDescriptorHeaps(1, core::ENGINE.GetDX12().m_SRVHeap.GetAddressOf());

                CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(core::ENGINE.GetDX12().m_SRVHeap->GetGPUDescriptorHandleForHeapStart());
                gpuHandle.Offset(0, core::ENGINE.GetDX12().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

                a_CommandList->SetGraphicsRootDescriptorTable(1, gpuHandle);
            }
        }
    }
}