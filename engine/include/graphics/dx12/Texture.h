#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <wrl.h>
#include <string>
#include <memory>
#include <glm/vec2.hpp>

#include "graphics/dx12/DX12Resource.h"

namespace gallus
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

				void Destroy();

				bool CheckSRVSupport() const;
				bool CheckRTVSupport() const;
				bool CheckUAVSupport() const;
				bool CheckDSVSupport() const;

				glm::ivec2 GetSize() const;

				bool Transition(std::shared_ptr<CommandList> a_CommandList);

				void Bind(std::shared_ptr<CommandList> a_CommandList);
				void Unbind(std::shared_ptr<CommandList> a_CommandList);

				CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
				CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();

				bool Load(const std::string& a_Name, std::shared_ptr<CommandList> a_CommandList);
				bool Load(const std::string& a_Name, const D3D12_RESOURCE_DESC& a_Description);
			private:

				Microsoft::WRL::ComPtr<ID3D12Resource> m_ResourceUploadHeap = nullptr;
				size_t m_SRVIndex = 0;

				friend class ResourceAtlas;
			};
		}
	}
}
