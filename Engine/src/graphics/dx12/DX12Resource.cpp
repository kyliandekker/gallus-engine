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

			D3D12_RESOURCE_DESC DX12Resource::GetResourceDesc() const
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

			void DX12Resource::UpdateBufferResource(
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList,
				ID3D12Resource** a_pDestinationResource,
				ID3D12Resource** a_pIntermediateResource,
				size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData,
				D3D12_RESOURCE_FLAGS a_Flags)
			{
				auto device = core::ENGINE.GetDX12().GetDevice();

				size_t bufferSize = a_NumElements * a_ElementSize;

				CD3DX12_HEAP_PROPERTIES defaultHeaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, a_Flags);
				// Create a committed resource for the GPU resource in a default heap.
				if (FAILED(device->CreateCommittedResource(
					&defaultHeaptype,
					D3D12_HEAP_FLAG_NONE,
					&buffer,
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(a_pDestinationResource))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource.");
					return;
				}

				// Create an committed resource for the upload.
				if (a_BufferData)
				{
					CD3DX12_HEAP_PROPERTIES uploadHeaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					CD3DX12_RESOURCE_DESC buff = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
					if (FAILED(device->CreateCommittedResource(
						&uploadHeaptype,
						D3D12_HEAP_FLAG_NONE,
						&buff,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(a_pIntermediateResource))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource.");
						return;
					}

					D3D12_SUBRESOURCE_DATA subresourceData = {};
					subresourceData.pData = a_BufferData;
					subresourceData.RowPitch = bufferSize;
					subresourceData.SlicePitch = subresourceData.RowPitch;

					UpdateSubresources(a_CommandList.Get(),
						*a_pDestinationResource, *a_pIntermediateResource,
						0, 0, 1, &subresourceData);
				}
			}
		}
	}
}