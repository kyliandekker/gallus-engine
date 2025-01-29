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

                bool IsValid() const;

                bool CheckSRVSupport() const;
                bool CheckRTVSupport() const;
                bool CheckUAVSupport() const;
                bool CheckDSVSupport() const;

                bool Transition(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
                void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
                void Unbind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);
            private:
                bool LoadTexture(const std::string& a_FilePath, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

                Microsoft::WRL::ComPtr<ID3D12Resource> m_ResourceUploadHeap;
                size_t m_SRVIndex;

                friend class TextureAtlas;
            };
        }
    }
}
