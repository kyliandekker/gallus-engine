#include "graphics/dx12/Material.h"

#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/CommandList.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			void Material::Bind(std::shared_ptr<CommandList> a_CommandList)
			{
				a_CommandList->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameters::MATERIAL, m_Resource->GetGPUVirtualAddress());
			}

			void Material::Unbind(std::shared_ptr<CommandList> a_CommandList)
			{}

			bool Material::LoadByName(const std::wstring& a_Name, const MaterialData& a_MaterialData)
			{
				m_MaterialData = a_MaterialData;

				CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

				D3D12_RESOURCE_DESC bufferDesc = {};
				bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				bufferDesc.Width = sizeof(MaterialData);  // Size of the Material struct
				bufferDesc.Height = 1;
				bufferDesc.DepthOrArraySize = 1;
				bufferDesc.MipLevels = 1;
				bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
				bufferDesc.SampleDesc.Count = 1;
				bufferDesc.SampleDesc.Quality = 0;
				bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				if (!CreateResource(bufferDesc, a_Name, heapProps, D3D12_RESOURCE_STATE_GENERIC_READ))
				{
					return false;
				}

				// Map the constant buffer to update it with material data
				D3D12_RANGE readRange = { 0, 0 };  // We don't need to read, so set to zero
				void* mappedData;
				m_Resource->Map(0, &readRange, &mappedData);
				memcpy(mappedData, &m_MaterialData, sizeof(m_MaterialData));  // Copy data into the buffer
				m_Resource->Unmap(0, nullptr);

				return true;
			}

			// TODO: Implement
			bool Material::LoadByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_CommandList)
			{
				return false;
			}
		}
	}
}