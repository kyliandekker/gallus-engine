#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <dx12/DirectXTex/DirectXTex/DirectXTex.h>
#include <wrl.h>
#include <string>
#include <memory>

#include "graphics/dx12/DX12Resource.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class CommandList;

            class Texture : public DX12Resource
            {
            public:
                Texture() = default;
                ~Texture();

                bool CheckSRVSupport() const;
                bool CheckRTVSupport() const;
                bool CheckUAVSupport() const;
                bool CheckDSVSupport() const;

                bool Transition(std::shared_ptr<CommandList> a_CommandList);
                
                void Bind(std::shared_ptr<CommandList> a_CommandList);
                void Unbind(std::shared_ptr<CommandList> a_CommandList);
            private:
                bool Load(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList);

                Microsoft::WRL::ComPtr<ID3D12Resource> m_ResourceUploadHeap = nullptr;
                size_t m_SRVIndex = 0;

                D3D12_RESOURCE_STATES m_State;

                friend class ResourceAtlas;
            };
        }
    }
}
