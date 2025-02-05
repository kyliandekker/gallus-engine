#pragma once

#include "graphics/dx12/Buffer.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class VertexBuffer : public Buffer
			{
			public:
				VertexBuffer(const std::wstring& a_Name = L"");
				virtual ~VertexBuffer();

				void CreateViews(size_t a_NumElements, size_t a_ElementSize) override;
				D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();
				size_t GetNumVertices() const;
				size_t GetVertexStride() const;
			private:
				size_t m_NumVertices = 0;
				size_t m_VertexStride = 0;

				D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
			};
		}
	}
}