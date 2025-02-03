#include "graphics/dx12/Buffer.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			Buffer::Buffer(const std::wstring& a_Name) : DX12Resource(a_Name)
			{ }

			Buffer::Buffer(const D3D12_RESOURCE_DESC& a_ResDesc,
				size_t a_NumElements, size_t a_ElementSize,
				const std::wstring& a_Name) : DX12Resource(a_Name)
			{


				CreateViews(a_NumElements, a_ElementSize);
			}

			void Buffer::CreateViews(size_t a_NumElements, size_t a_ElementSize)
			{
				throw std::exception("Unimplemented function.");
			}
		}
	}
}