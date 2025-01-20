#include "graphics/dx12/Adapter.h"

#include "core/logger/Logger.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class MakeAdapter : public Adapter
            {
            public:
                MakeAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter)
                    : Adapter(dxgiAdapter)
                {}

                virtual ~MakeAdapter() {}
            };

            AdapterList Adapter::GetAdapters(DXGI_GPU_PREFERENCE gpuPreference)
            {
                AdapterList adapters;

                Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
                Microsoft::WRL::ComPtr<IDXGIAdapter>  dxgiAdapter;
                Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

                UINT createFactoryFlags = 0;
#if defined( _DEBUG )
                createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

                if (FAILED(::CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6))))
                {
                    LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating DXGIFactory2.");
                    return;
                }

                for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
                    DXGI_ERROR_NOT_FOUND;
                    ++i)
                {
                    if (SUCCEEDED(
                        D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
                    {
                        if (FAILED(dxgiAdapter.As(&dxgiAdapter4)))
                        {
                            LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting DXGIFactory2 to DXGIFactory4.");
                            return;
                        }
                        std::shared_ptr<Adapter> adapter = std::make_shared<MakeAdapter>(dxgiAdapter4);
                        adapters.push_back(adapter);
                    }
                }

                return adapters;
            }

            std::shared_ptr<Adapter> Adapter::Create(DXGI_GPU_PREFERENCE a_GpuPreference, bool a_UseWarp)
            {
                std::shared_ptr<Adapter> adapter = nullptr;

                Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
                Microsoft::WRL::ComPtr<IDXGIAdapter>  dxgiAdapter;
                Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

                UINT createFactoryFlags = 0;
#if defined( _DEBUG )
                createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

                if (FAILED(::CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6))))
                {
                    LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating DXGIFactory2.");
                    return;
                }

                if (a_UseWarp)
                {
                    if (FAILED(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter))))
                    {
                        LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed using warp.");
                        return;
                    }
                    if (FAILED(dxgiAdapter.As(&dxgiAdapter4)))
                    {
                        LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting DXGIFactory2 to DXGIFactory4.");
                        return;
                    }
                }
                else
                {
                    for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, a_GpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
                        DXGI_ERROR_NOT_FOUND;
                        ++i)
                    {
                        if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device),
                            nullptr)))
                        {
                            if (FAILED(dxgiAdapter.As(&dxgiAdapter4)))
                            {
                                LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting DXGIFactory2 to DXGIFactory4.");
                                return;
                            }
                            break;
                        }
                    }
                }

                if (dxgiAdapter4)
                {
                    adapter = std::make_shared<MakeAdapter>(dxgiAdapter4);
                }

                return adapter;
            }

            Microsoft::WRL::ComPtr<IDXGIAdapter4> Adapter::GetDXGIAdapter() const
            {
                return Microsoft::WRL::ComPtr<IDXGIAdapter4>();
            }

            const std::wstring Adapter::GetDescription() const
            {
                return std::wstring();
            }
            
            Adapter::Adapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter)
                : m_dxgiAdapter(dxgiAdapter)
                , m_Desc{ 0 }
            {
                if (m_dxgiAdapter)
                {
                    if (FAILED(m_dxgiAdapter->GetDesc3(&m_Desc)))
                    {
                        LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed getting descriptor.");
                        return;
                    }
                }
            }

        }
    }
}