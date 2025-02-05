#include "graphics/dx12/IndexBuffer.h"

#include <cassert>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			IndexBuffer::IndexBuffer(const std::wstring& a_Name) : Buffer(a_Name)
			{
			}

			IndexBuffer::~IndexBuffer()
			{ }

			void IndexBuffer::CreateViews(size_t a_NumElements, size_t a_ElementSize)
			{
				assert(a_ElementSize == 2 || a_ElementSize == 4 && "Indices must be 16, or 32-bit integers.");

				m_NumIndicies = a_NumElements;
				m_IndexFormat = (a_ElementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

				m_IndexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
				m_IndexBufferView.SizeInBytes = static_cast<UINT>(a_NumElements * a_ElementSize);
				m_IndexBufferView.Format = m_IndexFormat;
			}

			D3D12_INDEX_BUFFER_VIEW& IndexBuffer::GetIndexBufferView()
			{
				return m_IndexBufferView;
			}

			size_t IndexBuffer::GetNumIndicies() const
			{
				return m_NumIndicies;
			}

			DXGI_FORMAT IndexBuffer::GetIndexFormat() const
			{
				return m_IndexFormat;
			}
		}
	}
}