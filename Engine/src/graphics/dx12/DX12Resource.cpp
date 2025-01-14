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
				m_Name = a_Name;
				CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				if (core::ENGINE.GetDX12().GetDevice()->CreateCommittedResource(
					&defaultHeap,
					D3D12_HEAP_FLAG_NONE,
					&a_ResourceDesc,
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&m_Resource)
				))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource: \"%s\".", a_Name.c_str());
					return;
				}
				m_Resource->SetName(a_Name.c_str());
			}

			DX12Resource::DX12Resource(const std::wstring& a_Name)
			{
				m_Name = a_Name;
			}

			DX12Resource::~DX12Resource()
			{

			}

			bool DX12Resource::IsValid() const
			{
				return (m_Resource != nullptr);
			}

			Microsoft::WRL::ComPtr<ID3D12Resource> DX12Resource::GetResource()
			{
				return m_Resource;
			}

			D3D12_RESOURCE_DESC DX12Resource::GetResourceDesc() const
			{
				D3D12_RESOURCE_DESC resDesc = {};
				if (m_Resource)
				{
					resDesc = m_Resource->GetDesc();
				}

				return resDesc;
			}
		}
	}
}