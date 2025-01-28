#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <dx12/DirectXTex/DirectXTex/DirectXTex.h>

#include "graphics/dx12/DX12Resource.h"

#include <wrl.h>
#include <string>
#include "core/logger/Logger.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class Texture : public DX12Resource {
            public:
                Texture() = default;
                ~Texture();

                bool CheckSRVSupport() const;

                bool CheckRTVSupport() const;

                bool CheckUAVSupport() const;

                bool CheckDSVSupport() const;

                bool LoadTexture(const std::wstring& a_FilePath, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
                bool Transition(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
                void CopyTextureSubresource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, uint32_t a_FirstSubresource, uint32_t a_NumSubresources, D3D12_SUBRESOURCE_DATA* a_SubresourceData);
                void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
            private:
                Microsoft::WRL::ComPtr<ID3D12Resource> m_ResourceUploadHeap;

                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
                CD3DX12_CPU_DESCRIPTOR_HANDLE depthHandle;
                CD3DX12_CPU_DESCRIPTOR_HANDLE unorderedAccessHandle;
                CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle;
                std::vector<D3D12_SUBRESOURCE_DATA> subresources;
                DirectX::TexMetadata  metadata;
                DirectX::ScratchImage scratchImage;
                const DirectX::Image* pImages;
                Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
            };
        }
    }
}
