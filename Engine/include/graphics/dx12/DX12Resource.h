#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <string>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12Resource
			{
			public:
				DX12Resource(const D3D12_RESOURCE_DESC& a_ResourceDesc, const std::wstring& a_Name);
				DX12Resource(const std::wstring& a_Name);
				virtual ~DX12Resource();

				bool IsValid() const;

				Microsoft::WRL::ComPtr<ID3D12Resource> GetResource();

				D3D12_RESOURCE_DESC GetResourceDesc() const;
			protected:
				Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
				std::wstring m_Name;
			};
		}
	}
}