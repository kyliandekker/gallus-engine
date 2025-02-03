#pragma once

#include "graphics/dx12/Buffer.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class IndexBuffer : public Buffer
			{
			public:
				IndexBuffer(const std::wstring& a_Name = L"");
				virtual ~IndexBuffer();

				void CreateViews(size_t a_NumElements, size_t a_ElementSize) override;
				D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView();
				size_t GetNumIndicies() const;
				DXGI_FORMAT GetIndexFormat() const;
			private:
				size_t m_NumIndicies = 0;
				DXGI_FORMAT m_IndexFormat;

				D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
			};
		}
	}
}