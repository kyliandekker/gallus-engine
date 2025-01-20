#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <wrl/client.h>

#include <memory>
#include <string>
#include <vector>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class Adapter;
			using AdapterList = std::vector<std::shared_ptr<Adapter>>;

			class Adapter
			{
			public:
				static AdapterList GetAdapters(DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);
				static std::shared_ptr<Adapter> Create(DXGI_GPU_PREFERENCE a_GpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, bool a_UseWarp = false);
				Microsoft::WRL::ComPtr<IDXGIAdapter4> GetDXGIAdapter() const;
				const std::wstring GetDescription() const;
			protected:
				Adapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter);
				virtual ~Adapter() = default;

			private:
				Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
				DXGI_ADAPTER_DESC3                    m_Desc;
			};
		}
	}
}