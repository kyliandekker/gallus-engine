#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

 /**
  *  @file Texture.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief A wrapper for a DX12 Texture object.
  */

#include "graphics/dx12/DX12PCH.h"

#include "graphics/dx12/DX12Resource.h"

#include <mutex>
#include <unordered_map>
#include <string>

#include "DescriptorAllocation.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class Device;

            class Texture : public DX12Resource
            {
            public:
                /**
                 * Resize the texture.
                 */
                void Resize(uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize = 1);

                void Load(const std::wstring a_Path);

                /**
                 * Get the RTV for the texture.
                 */
                D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

                /**
                 * Get the DSV for the texture.
                 */
                D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

                /**
                 * Get the default SRV for the texture.
                 */
                D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

                /**
                 * Get the UAV for the texture at a specific mip level.
                 * Note: Only only supported for 1D and 2D textures.
                 */
                D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t a_Mip) const;

                bool CheckSRVSupport() const
                {
                    return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
                }

                bool CheckRTVSupport() const
                {
                    return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
                }

                bool CheckUAVSupport() const
                {
                    return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
                        CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
                        CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
                }

                bool CheckDSVSupport() const
                {
                    return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
                }

                /**
                 * Check to see if the image format has an alpha channel.
                 */
                bool HasAlpha() const;

                /**
                 * Check the number of bits per pixel.
                 */
                size_t BitsPerPixel() const;

                static bool IsUAVCompatibleFormat(DXGI_FORMAT a_Format);
                static bool IsSRGBFormat(DXGI_FORMAT a_Format);
                static bool IsBGRFormat(DXGI_FORMAT a_Format);
                static bool IsDepthFormat(DXGI_FORMAT a_Format);

                // Return a typeless format from the given format.
                static DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT a_Format);
                // Return an sRGB format in the same format family.
                static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT a_Format);
                static DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT a_Format);

                Texture() : DX12Resource() {}
                Texture(const D3D12_RESOURCE_DESC& a_ResourceDesc);

                void Bind();

            protected:
                /**
                 * Create SRV and UAVs for the resource.
                 */
                void CreateViews();

            private:
                Microsoft::WRL::ComPtr<ID3D12Resource> intermediateTexture;

                DescriptorAllocation m_RenderTargetView;
                DescriptorAllocation m_DepthStencilView;
                DescriptorAllocation m_ShaderResourceView;
                DescriptorAllocation m_UnorderedAccessView;
            };
        } 
    }
}