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

				Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource();

				D3D12_RESOURCE_DESC GetResourceDesc() const;

				bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 a_FormatSupport) const;
				bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 a_FormatSupport) const;

				void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource);
				void CheckFeatureSupport();
			protected:
				D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
				Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
				std::wstring m_Name;
			};
		}
	}
}