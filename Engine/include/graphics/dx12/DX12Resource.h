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
				const std::wstring& GetName() const;

				bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
				bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;

				void CheckFeatureSupport();

				void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource);
			protected:
				Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
				std::unique_ptr<D3D12_CLEAR_VALUE>     m_d3d12ClearValue;
				D3D12_FEATURE_DATA_FORMAT_SUPPORT      m_FormatSupport;
				std::wstring m_Name;
			};
		}
	}
}