#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

#include <string>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class Buffer : public DX12Resource
			{
			public:
				Buffer(const std::wstring& a_Name = L"");
				Buffer(const D3D12_RESOURCE_DESC& a_ResDesc,
					size_t a_NumElements, size_t a_ElementSize,
					const std::wstring& a_Name = L"");
				virtual void CreateViews(size_t a_NumElements, size_t a_ElementSize);
			};
		}
	}
}