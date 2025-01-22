#include "graphics/dx12/Texture.h"

#include <dx12/DirectXTex/DirectXTex/DirectXTex.h>

#include "core/Engine.h"
#include "core/logger/Logger.h"

#undef max

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            void Texture::Resize(uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize)
            {
                if (m_Resource)
                {
                    // ResourceStateTracker::RemoveGlobalResourceState( m_d3d12Resource.Get() );

                    CD3DX12_RESOURCE_DESC resDesc(m_Resource->GetDesc());

                    resDesc.Width = std::max(a_Width, 1u);
                    resDesc.Height = std::max(a_Height, 1u);
                    resDesc.DepthOrArraySize = a_DepthOrArraySize;
                    resDesc.MipLevels = resDesc.SampleDesc.Count > 1 ? 1 : 0;

                    CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
                    if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreateCommittedResource(
                        &defaultHeap, D3D12_HEAP_FLAG_NONE, &resDesc,
                        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_Resource))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating resource.");
                        return;
                    }

                    // Retain the name of the resource if one was already specified.
                    m_Resource->SetName(m_Name.c_str());

                    CreateViews();
                }
            }

            D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView() const
            {
                return m_RenderTargetView.GetDescriptorHandle();
            }

            D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilView() const
            {
                return m_DepthStencilView.GetDescriptorHandle();
            }

            D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView() const
            {
                return m_ShaderResourceView.GetDescriptorHandle();
            }

            D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(uint32_t a_Mip) const
            {
                return m_UnorderedAccessView.GetDescriptorHandle(a_Mip);
            }

            bool Texture::HasAlpha() const
            {
                DXGI_FORMAT format = GetResourceDesc().Format;

                bool hasAlpha = false;

                switch (format)
                {
                    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
                    case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    case DXGI_FORMAT_R32G32B32A32_UINT:
                    case DXGI_FORMAT_R32G32B32A32_SINT:
                    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
                    case DXGI_FORMAT_R16G16B16A16_FLOAT:
                    case DXGI_FORMAT_R16G16B16A16_UNORM:
                    case DXGI_FORMAT_R16G16B16A16_UINT:
                    case DXGI_FORMAT_R16G16B16A16_SNORM:
                    case DXGI_FORMAT_R16G16B16A16_SINT:
                    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
                    case DXGI_FORMAT_R10G10B10A2_UNORM:
                    case DXGI_FORMAT_R10G10B10A2_UINT:
                    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
                    case DXGI_FORMAT_R8G8B8A8_UNORM:
                    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    case DXGI_FORMAT_R8G8B8A8_UINT:
                    case DXGI_FORMAT_R8G8B8A8_SNORM:
                    case DXGI_FORMAT_R8G8B8A8_SINT:
                    case DXGI_FORMAT_BC1_TYPELESS:
                    case DXGI_FORMAT_BC1_UNORM:
                    case DXGI_FORMAT_BC1_UNORM_SRGB:
                    case DXGI_FORMAT_BC2_TYPELESS:
                    case DXGI_FORMAT_BC2_UNORM:
                    case DXGI_FORMAT_BC2_UNORM_SRGB:
                    case DXGI_FORMAT_BC3_TYPELESS:
                    case DXGI_FORMAT_BC3_UNORM:
                    case DXGI_FORMAT_BC3_UNORM_SRGB:
                    case DXGI_FORMAT_B5G5R5A1_UNORM:
                    case DXGI_FORMAT_B8G8R8A8_UNORM:
                    case DXGI_FORMAT_B8G8R8X8_UNORM:
                    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
                    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
                    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    case DXGI_FORMAT_BC6H_TYPELESS:
                    case DXGI_FORMAT_BC7_TYPELESS:
                    case DXGI_FORMAT_BC7_UNORM:
                    case DXGI_FORMAT_BC7_UNORM_SRGB:
                    case DXGI_FORMAT_A8P8:
                    case DXGI_FORMAT_B4G4R4A4_UNORM:
                    {
                        hasAlpha = true;
                        break;
                    }
                }

                return hasAlpha;
            }

            size_t Texture::BitsPerPixel() const
            {
                auto format = GetResourceDesc().Format;
                return DirectX::BitsPerPixel(format);
            }

            bool Texture::IsUAVCompatibleFormat(DXGI_FORMAT a_Format)
            {
                switch (a_Format)
                {
                    case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    case DXGI_FORMAT_R32G32B32A32_UINT:
                    case DXGI_FORMAT_R32G32B32A32_SINT:
                    case DXGI_FORMAT_R16G16B16A16_FLOAT:
                    case DXGI_FORMAT_R16G16B16A16_UINT:
                    case DXGI_FORMAT_R16G16B16A16_SINT:
                    case DXGI_FORMAT_R8G8B8A8_UNORM:
                    case DXGI_FORMAT_R8G8B8A8_UINT:
                    case DXGI_FORMAT_R8G8B8A8_SINT:
                    case DXGI_FORMAT_R32_FLOAT:
                    case DXGI_FORMAT_R32_UINT:
                    case DXGI_FORMAT_R32_SINT:
                    case DXGI_FORMAT_R16_FLOAT:
                    case DXGI_FORMAT_R16_UINT:
                    case DXGI_FORMAT_R16_SINT:
                    case DXGI_FORMAT_R8_UNORM:
                    case DXGI_FORMAT_R8_UINT:
                    case DXGI_FORMAT_R8_SINT:
                    {
                        return true;
                    }
                    default:
                    {
                        return false;
                    }
                }
            }

            bool Texture::IsSRGBFormat(DXGI_FORMAT a_Format)
            {
                switch (a_Format)
                {
                    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    case DXGI_FORMAT_BC1_UNORM_SRGB:
                    case DXGI_FORMAT_BC2_UNORM_SRGB:
                    case DXGI_FORMAT_BC3_UNORM_SRGB:
                    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    case DXGI_FORMAT_BC7_UNORM_SRGB:
                    {
                        return true;
                    }
                    default:
                    {
                        return false;
                    }
                }
            }

            bool Texture::IsBGRFormat(DXGI_FORMAT a_Format)
            {
                switch (a_Format)
                {
                    case DXGI_FORMAT_B8G8R8A8_UNORM:
                    case DXGI_FORMAT_B8G8R8X8_UNORM:
                    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
                    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    {
                        return true;
                    }
                    default:
                    {
                        return false;
                    }
                }
            }

            bool Texture::IsDepthFormat(DXGI_FORMAT a_Format)
            {
                switch (a_Format)
                {
                    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    case DXGI_FORMAT_D32_FLOAT:
                    case DXGI_FORMAT_D24_UNORM_S8_UINT:
                    case DXGI_FORMAT_D16_UNORM:
                    {
                        return true;
                    }
                    default:
                    {
                        return false;
                    }
                }
            }

            DXGI_FORMAT Texture::GetTypelessFormat(DXGI_FORMAT a_Format)
            {
                DXGI_FORMAT typelessFormat = a_Format;

                switch (a_Format)
                {
                    case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    case DXGI_FORMAT_R32G32B32A32_UINT:
                    case DXGI_FORMAT_R32G32B32A32_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R32G32B32_FLOAT:
                    case DXGI_FORMAT_R32G32B32_UINT:
                    case DXGI_FORMAT_R32G32B32_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R16G16B16A16_FLOAT:
                    case DXGI_FORMAT_R16G16B16A16_UNORM:
                    case DXGI_FORMAT_R16G16B16A16_UINT:
                    case DXGI_FORMAT_R16G16B16A16_SNORM:
                    case DXGI_FORMAT_R16G16B16A16_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R32G32_FLOAT:
                    case DXGI_FORMAT_R32G32_UINT:
                    case DXGI_FORMAT_R32G32_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R32G32_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    {
                        typelessFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R10G10B10A2_UNORM:
                    case DXGI_FORMAT_R10G10B10A2_UINT:
                    {
                        typelessFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R8G8B8A8_UNORM:
                    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    case DXGI_FORMAT_R8G8B8A8_UINT:
                    case DXGI_FORMAT_R8G8B8A8_SNORM:
                    case DXGI_FORMAT_R8G8B8A8_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R16G16_FLOAT:
                    case DXGI_FORMAT_R16G16_UNORM:
                    case DXGI_FORMAT_R16G16_UINT:
                    case DXGI_FORMAT_R16G16_SNORM:
                    case DXGI_FORMAT_R16G16_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R16G16_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_D32_FLOAT:
                    case DXGI_FORMAT_R32_FLOAT:
                    case DXGI_FORMAT_R32_UINT:
                    case DXGI_FORMAT_R32_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R32_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R8G8_UNORM:
                    case DXGI_FORMAT_R8G8_UINT:
                    case DXGI_FORMAT_R8G8_SNORM:
                    case DXGI_FORMAT_R8G8_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R8G8_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R16_FLOAT:
                    case DXGI_FORMAT_D16_UNORM:
                    case DXGI_FORMAT_R16_UNORM:
                    case DXGI_FORMAT_R16_UINT:
                    case DXGI_FORMAT_R16_SNORM:
                    case DXGI_FORMAT_R16_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R16_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_R8_UNORM:
                    case DXGI_FORMAT_R8_UINT:
                    case DXGI_FORMAT_R8_SNORM:
                    case DXGI_FORMAT_R8_SINT:
                    {
                        typelessFormat = DXGI_FORMAT_R8_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC1_UNORM:
                    case DXGI_FORMAT_BC1_UNORM_SRGB:
                    {
                        typelessFormat = DXGI_FORMAT_BC1_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC2_UNORM:
                    case DXGI_FORMAT_BC2_UNORM_SRGB:
                    {
                        typelessFormat = DXGI_FORMAT_BC2_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC3_UNORM:
                    case DXGI_FORMAT_BC3_UNORM_SRGB:
                    {
                        typelessFormat = DXGI_FORMAT_BC3_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC4_UNORM:
                    case DXGI_FORMAT_BC4_SNORM:
                    {
                        typelessFormat = DXGI_FORMAT_BC4_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC5_UNORM:
                    case DXGI_FORMAT_BC5_SNORM:
                    {
                        typelessFormat = DXGI_FORMAT_BC5_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    {
                        typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    {
                        typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC6H_UF16:
                    case DXGI_FORMAT_BC6H_SF16:
                    {
                        typelessFormat = DXGI_FORMAT_BC6H_TYPELESS;
                        break;
                    }
                    case DXGI_FORMAT_BC7_UNORM:
                    case DXGI_FORMAT_BC7_UNORM_SRGB:
                    {
                        typelessFormat = DXGI_FORMAT_BC7_TYPELESS;
                        break;
                    }
                }

                return typelessFormat;
            }

            DXGI_FORMAT Texture::GetSRGBFormat(DXGI_FORMAT a_Format)
            {
                DXGI_FORMAT srgbFormat = a_Format;

                switch (a_Format)
                {
                    case DXGI_FORMAT_R8G8B8A8_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                        break;
                    }
                    case DXGI_FORMAT_BC1_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_BC1_UNORM_SRGB;
                        break;
                    }
                    case DXGI_FORMAT_BC2_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_BC2_UNORM_SRGB;
                        break;
                    }
                    case DXGI_FORMAT_BC3_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_BC3_UNORM_SRGB;
                        break;
                    }
                    case DXGI_FORMAT_B8G8R8A8_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
                        break;
                    }
                    case DXGI_FORMAT_B8G8R8X8_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
                        break;
                    }
                    case DXGI_FORMAT_BC7_UNORM:
                    {
                        srgbFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
                        break;
                    }
                }

                return srgbFormat;
            }

            DXGI_FORMAT Texture::GetUAVCompatableFormat(DXGI_FORMAT a_Format)
            {
                DXGI_FORMAT uavFormat = a_Format;

                switch (a_Format)
                {
                    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
                    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    case DXGI_FORMAT_B8G8R8A8_UNORM:
                    case DXGI_FORMAT_B8G8R8X8_UNORM:
                    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
                    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    {
                        uavFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                        break;
                    }
                    case DXGI_FORMAT_R32_TYPELESS:
                    case DXGI_FORMAT_D32_FLOAT:
                    {
                        uavFormat = DXGI_FORMAT_R32_FLOAT;
                        break;
                    }
                }

                return uavFormat;
            }

            Texture::Texture(const D3D12_RESOURCE_DESC& a_ResourceDesc) : DX12Resource(a_ResourceDesc, L"")
            {
                CreateViews();
            }

            D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& a_ResDesc, UINT a_MipSlice, UINT a_ArraySlice = 0,
                UINT planeSlice = 0)
            {
                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
                uavDesc.Format = a_ResDesc.Format;

                switch (a_ResDesc.Dimension)
                {
                    case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
                        if (a_ResDesc.DepthOrArraySize > 1)
                        {
                            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
                            uavDesc.Texture1DArray.ArraySize = a_ResDesc.DepthOrArraySize - a_ArraySlice;
                            uavDesc.Texture1DArray.FirstArraySlice = a_ArraySlice;
                            uavDesc.Texture1DArray.MipSlice = a_MipSlice;
                        }
                        else
                        {
                            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
                            uavDesc.Texture1D.MipSlice = a_MipSlice;
                        }
                        break;
                    case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
                        if (a_ResDesc.DepthOrArraySize > 1)
                        {
                            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                            uavDesc.Texture2DArray.ArraySize = a_ResDesc.DepthOrArraySize - a_ArraySlice;
                            uavDesc.Texture2DArray.FirstArraySlice = a_ArraySlice;
                            uavDesc.Texture2DArray.PlaneSlice = planeSlice;
                            uavDesc.Texture2DArray.MipSlice = a_MipSlice;
                        }
                        else
                        {
                            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                            uavDesc.Texture2D.PlaneSlice = planeSlice;
                            uavDesc.Texture2D.MipSlice = a_MipSlice;
                        }
                        break;
                    case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
                        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                        uavDesc.Texture3D.WSize = a_ResDesc.DepthOrArraySize - a_ArraySlice;
                        uavDesc.Texture3D.FirstWSlice = a_ArraySlice;
                        uavDesc.Texture3D.MipSlice = a_MipSlice;
                        break;
                    default:
                        throw std::exception("Invalid resource dimension.");
                }

                return uavDesc;
            }

            void Texture::CreateViews()
            {
                if (m_Resource)
                {
                    CD3DX12_RESOURCE_DESC desc(m_Resource->GetDesc());

                    // Create RTV
                    if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport())
                    {
                        m_RenderTargetView = core::ENGINE.GetDX12().AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                        core::ENGINE.GetDX12().GetDevice()->CreateRenderTargetView(m_Resource.Get(), nullptr,
                            m_RenderTargetView.GetDescriptorHandle());
                    }
                    // Create DSV
                    if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport())
                    {
                        m_DepthStencilView = core::ENGINE.GetDX12().AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                        core::ENGINE.GetDX12().GetDevice()->CreateDepthStencilView(m_Resource.Get(), nullptr,
                            m_DepthStencilView.GetDescriptorHandle());
                    }
                    // Create SRV
                    if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
                    {
                        m_ShaderResourceView = core::ENGINE.GetDX12().AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                        core::ENGINE.GetDX12().GetDevice()->CreateShaderResourceView(m_Resource.Get(), nullptr,
                            m_ShaderResourceView.GetDescriptorHandle());
                    }
                    // Create UAV for each mip (only supported for 1D and 2D textures).
                    if ((desc.Flags & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0 && CheckUAVSupport() && desc.DepthOrArraySize == 1)
                    {
                        m_UnorderedAccessView = core::ENGINE.GetDX12().AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, desc.MipLevels);
                        for (int i = 0; i < desc.MipLevels; ++i)
                        {
                            auto uavDesc = GetUAVDesc(desc, i);
                            core::ENGINE.GetDX12().GetDevice()->CreateUnorderedAccessView(m_Resource.Get(), nullptr, &uavDesc,
                                m_UnorderedAccessView.GetDescriptorHandle(i));
                        }
                    }
                }
            }
        }
    }
}