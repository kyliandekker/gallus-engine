#include "graphics/dx12/DX12Resource.h"

#include "core/Engine.h" 
#include "core/logger/Logger.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			DX12Resource::DX12Resource(const D3D12_RESOURCE_DESC& a_ResourceDesc, const std::wstring& a_Name)
			{
				CreateResource(a_ResourceDesc, a_Name);
			}

			DX12Resource::DX12Resource(const std::wstring& a_Name)
			{
				m_Name = a_Name;
			}

			DX12Resource::~DX12Resource()
			{ }

			bool DX12Resource::CreateResource(const D3D12_RESOURCE_DESC& a_ResourceDesc, const std::wstring& a_Name, const CD3DX12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState)
			{
				m_Name = a_Name;
				if (core::ENGINE.GetDX12().GetDevice()->CreateCommittedResource(
					&a_Heap,
					D3D12_HEAP_FLAG_NONE,
					&a_ResourceDesc,
					a_ResourceState,
					nullptr,
					IID_PPV_ARGS(&m_Resource)
				))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource: \"%s\".", a_Name.c_str());
					return false;
				}
				m_Resource->SetName(a_Name.c_str());

				CheckFeatureSupport();

				return true;
			}

			bool DX12Resource::IsValid() const
			{
				return (m_Resource != nullptr);
			}

			Microsoft::WRL::ComPtr<ID3D12Resource>& DX12Resource::GetResource()
			{
				return m_Resource;
			}

			const D3D12_RESOURCE_DESC& DX12Resource::GetResourceDesc() const
			{
				D3D12_RESOURCE_DESC resDesc = {};
				if (m_Resource)
				{
					resDesc = m_Resource->GetDesc();
				}

				return resDesc;
			}

			bool DX12Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 a_FormatSupport) const
			{
				return (m_FormatSupport.Support1 & a_FormatSupport) != 0;
			}

			bool DX12Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 a_FormatSupport) const
			{
				return (m_FormatSupport.Support2 & a_FormatSupport) != 0;
			}
			
			void DX12Resource::SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource)
			{
				m_Resource = a_Resource;
			}

			void DX12Resource::CheckFeatureSupport()
			{
				auto d3d12Device = core::ENGINE.GetDX12().GetDevice();

				auto desc = m_Resource->GetDesc();
				m_FormatSupport.Format = desc.Format;
				if (FAILED(d3d12Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
					sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT))))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed checking feature support.");
					return;
				}
			}
		}
	}
}