#include "graphics/dx12/VertexBuffer.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			VertexBuffer::VertexBuffer(const std::wstring& a_Name) : Buffer(a_Name)
			{ }

			VertexBuffer::~VertexBuffer()
			{ }

			void VertexBuffer::CreateViews(size_t a_NumElements, size_t a_ElementSize)
			{
				m_NumVertices = a_NumElements;
				m_VertexStride = a_ElementSize;

				m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
				m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_NumVertices * m_VertexStride);
				m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_VertexStride);
			}

			D3D12_VERTEX_BUFFER_VIEW& VertexBuffer::GetVertexBufferView()
			{
				return m_VertexBufferView;
			}

			size_t VertexBuffer::GetNumVertices() const
			{
				return m_NumVertices;
			}

			size_t VertexBuffer::GetVertexStride() const
			{
				return m_VertexStride;
			}
		}
	}
}