#include "graphics/dx12/CommandList.h"

#include <d3d12.h>
#include <filesystem>
namespace fs = std::filesystem;

#include "core/logger/Logger.h"
#include "core/Engine.h"
#include "graphics/dx12/DX12Resource.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			void CommandList::TransitionBarrier(const std::shared_ptr<DX12Resource>& a_Resource, D3D12_RESOURCE_STATES a_StateAfter, UINT a_Subresource, bool a_FlushBarriers)
			{
                if (a_Resource)
                {
                    TransitionBarrier(a_Resource->GetResource(), a_StateAfter, a_Subresource, a_FlushBarriers);
                }
			}

            void CommandList::TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource, D3D12_RESOURCE_STATES a_StateAfter, UINT a_Subresource, bool a_FlushBarriers)
            {
                if (a_Resource)
                {
                    // The "before" state is not important. It will be resolved by the resource state tracker.
                    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(a_Resource.Get(), D3D12_RESOURCE_STATE_COMMON, a_StateAfter, a_Subresource);
                    m_ResourceStateTracker->ResourceBarrier(barrier);
                }

                if (a_FlushBarriers)
                {
                    FlushResourceBarriers();
                }
            }

            void CommandList::UAVBarrier(const std::shared_ptr<DX12Resource>& a_Resource, bool a_FlushBarriers)
            {
                auto d3d12Resource = a_Resource ? a_Resource->GetResource() : nullptr;
                UAVBarrier(d3d12Resource, a_FlushBarriers);
            }

            void CommandList::UAVBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource, bool a_FlushBarriers)
            {
                auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(a_Resource.Get());

                m_ResourceStateTracker->ResourceBarrier(barrier);

                if (a_FlushBarriers)
                {
                    FlushResourceBarriers();
                }
            }

            void CommandList::AliasingBarrier(const std::shared_ptr<DX12Resource>& a_BeforeResource, const std::shared_ptr<DX12Resource>& a_AfterResource, bool a_FlushBarriers)
            {
                auto d3d12BeforeResource = a_BeforeResource ? a_BeforeResource->GetResource() : nullptr;
                auto d3d12AfterResource = a_AfterResource ? a_AfterResource->GetResource() : nullptr;

                AliasingBarrier(d3d12BeforeResource, d3d12AfterResource, a_FlushBarriers);
            }

            void CommandList::AliasingBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> a_BeforeResource, Microsoft::WRL::ComPtr<ID3D12Resource> a_AfterResource, bool a_FlushBarriers)
            {
                auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(a_BeforeResource.Get(), a_AfterResource.Get());

                m_ResourceStateTracker->ResourceBarrier(barrier);

                if (a_FlushBarriers)
                {
                    FlushResourceBarriers();
                }
            }

            void CommandList::FlushResourceBarriers()
            {
                m_ResourceStateTracker->FlushResourceBarriers(shared_from_this());
            }

            void CommandList::CopyResource(const std::shared_ptr<DX12Resource>& a_DestinationResource, const std::shared_ptr<DX12Resource>& a_SrcResource)
            {
                assert(a_DestinationResource && a_SrcResource);

                CopyResource(a_DestinationResource->GetResource(), a_SrcResource->GetResource());
            }

            void CommandList::CopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_DestinationResource, Microsoft::WRL::ComPtr<ID3D12Resource> a_SrcResource)
            {
                assert(a_DestinationResource);
                assert(a_SrcResource);

                TransitionBarrier(a_DestinationResource, D3D12_RESOURCE_STATE_COPY_DEST);
                TransitionBarrier(a_SrcResource, D3D12_RESOURCE_STATE_COPY_SOURCE);

                FlushResourceBarriers();

                m_d3d12CommandList->CopyResource(a_DestinationResource.Get(), a_SrcResource.Get());

                TrackResource(a_DestinationResource);
                TrackResource(a_SrcResource);
            }

            void CommandList::ResolveSubresource(const std::shared_ptr<DX12Resource>& a_DestinationResource, const std::shared_ptr<DX12Resource>& a_SrcResource, uint32_t a_DestinationSubresource, uint32_t a_SrcSubresource)
            {
                assert(a_DestinationResource && a_SrcResource);

                TransitionBarrier(a_DestinationResource, D3D12_RESOURCE_STATE_RESOLVE_DEST, a_DestinationSubresource);
                TransitionBarrier(a_SrcResource, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, a_SrcSubresource);

                FlushResourceBarriers();

                m_d3d12CommandList->ResolveSubresource(a_DestinationResource->GetResource().Get(), a_DestinationSubresource,
                    a_SrcResource->GetResource().Get(), a_SrcSubresource,
                    a_DestinationResource->GetResourceDesc().Format);

                TrackResource(a_SrcResource);
                TrackResource(a_DestinationResource);
            }

            std::shared_ptr<VertexBuffer> CommandList::CopyVertexBuffer(size_t a_NumVertices, size_t a_VertexStride, const void* a_VertexBufferData)
            {
                auto d3d12Resource = CopyBuffer(a_NumVertices * a_VertexStride, a_VertexBufferData);
                std::shared_ptr<VertexBuffer> vertexBuffer = core::ENGINE.GetDX12().GetDevice().CreateVertexBuffer(d3d12Resource, a_NumVertices, a_VertexStride);

                return vertexBuffer;
            }

            std::shared_ptr<IndexBuffer> CommandList::CopyIndexBuffer(size_t a_NumIndicies, DXGI_FORMAT a_IndexFormat, const void* a_IndexBufferData)
            {
                size_t elementSize = a_IndexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;

                auto d3d12Resource = CopyBuffer(a_NumIndicies * elementSize, a_IndexBufferData);

                std::shared_ptr<IndexBuffer> indexBuffer = core::ENGINE.GetDX12().GetDevice().CreateIndexBuffer(d3d12Resource, a_NumIndicies, a_IndexFormat);

                return indexBuffer;
            }

            std::shared_ptr<ConstantBuffer> CommandList::CopyConstantBuffer(size_t a_BufferSize, const void* a_BufferData)
            {
                auto d3d12Resource = CopyBuffer(a_BufferSize, a_BufferData);

                std::shared_ptr<ConstantBuffer> constantBuffer = core::ENGINE.GetDX12().GetDevice().CreateConstantBuffer(d3d12Resource);

                return constantBuffer;
            }

            std::shared_ptr<ByteAddressBuffer> CommandList::CopyByteAddressBuffer(size_t a_BufferSize, const void* a_BufferData)
            {
                auto d3d12Resource = CopyBuffer(a_BufferSize, a_BufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

                std::shared_ptr<ByteAddressBuffer> byteAddressBuffer = core::ENGINE.GetDX12().GetDevice().CreateByteAddressBuffer(d3d12Resource);

                return byteAddressBuffer;
            }

            std::shared_ptr<StructuredBuffer> CommandList::CopyStructuredBuffer(size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData)
            {
                auto d3d12Resource = CopyBuffer(a_NumElements * a_ElementSize, a_BufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

                std::shared_ptr<StructuredBuffer> structuredBuffer = core::ENGINE.GetDX12().GetDevice().CreateStructuredBuffer(d3d12Resource, a_NumElements, a_ElementSize);

                return structuredBuffer;
            }

            void CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY a_PrimitiveTopology)
            {
                m_d3d12CommandList->IASetPrimitiveTopology(a_PrimitiveTopology);
            }

            std::shared_ptr<Texture> CommandList::LoadTextureFromFile(const std::wstring& a_FileName, bool a_sRGB)
            {
                std::shared_ptr<Texture> texture;
                fs::path filePath(a_FileName);
                if (!fs::exists(filePath))
                {
                    throw std::exception("File not found.");
                }

                //std::lock_guard<std::mutex> lock(ms_TextureCacheMutex);
                //auto iter = ms_TextureCache.find(a_FileName);
                //if (iter != ms_TextureCache.end())
                //{
                //    texture = core::ENGINE.GetDX12().GetDevice().CreateTexture(iter->second);
                //}
                //else
                //{
                //    TexMetadata metadata;
                //    ScratchImage scratchImage;

                //    if (filePath.extension() == ".dds")
                //    {
                //        ThrowIfFailed(LoadFromDDSFile(fileName.c_str(), DDS_FLAGS_FORCE_RGB, &metadata, scratchImage));
                //    }
                //    else if (filePath.extension() == ".hdr")
                //    {
                //        ThrowIfFailed(LoadFromHDRFile(fileName.c_str(), &metadata, scratchImage));
                //    }
                //    else if (filePath.extension() == ".tga")
                //    {
                //        ThrowIfFailed(LoadFromTGAFile(fileName.c_str(), &metadata, scratchImage));
                //    }
                //    else
                //    {
                //        ThrowIfFailed(LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
                //    }

                //    // Force the texture format to be sRGB to convert to linear when sampling the texture in a shader.
                //    if (sRGB)
                //    {
                //        metadata.format = MakeSRGB(metadata.format);
                //    }

                //    D3D12_RESOURCE_DESC textureDesc = {};
                //    switch (metadata.dimension)
                //    {
                //        case TEX_DIMENSION_TEXTURE1D:
                //            textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(metadata.format, static_cast<UINT64>(metadata.width),
                //                static_cast<UINT16>(metadata.arraySize));
                //            break;
                //        case TEX_DIMENSION_TEXTURE2D:
                //            textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, static_cast<UINT64>(metadata.width),
                //                static_cast<UINT>(metadata.height),
                //                static_cast<UINT16>(metadata.arraySize));
                //            break;
                //        case TEX_DIMENSION_TEXTURE3D:
                //            textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(metadata.format, static_cast<UINT64>(metadata.width),
                //                static_cast<UINT>(metadata.height),
                //                static_cast<UINT16>(metadata.depth));
                //            break;
                //        default:
                //            throw std::exception("Invalid texture dimension.");
                //            break;
                //    }

                //    auto                                   d3d12Device = m_Device.GetD3D12Device();
                //    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;

                //    ThrowIfFailed(d3d12Device->CreateCommittedResource(
                //        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc,
                //        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&textureResource)));

                //    texture = m_Device.CreateTexture(textureResource);
                //    texture->SetName(fileName);

                //    // Update the global state tracker.
                //    ResourceStateTracker::AddGlobalResourceState(textureResource.Get(), D3D12_RESOURCE_STATE_COMMON);

                //    std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
                //    const Image* pImages = scratchImage.GetImages();
                //    for (int i = 0; i < scratchImage.GetImageCount(); ++i)
                //    {
                //        auto& subresource = subresources[i];
                //        subresource.RowPitch = pImages[i].rowPitch;
                //        subresource.SlicePitch = pImages[i].slicePitch;
                //        subresource.pData = pImages[i].pixels;
                //    }

                //    CopyTextureSubresource(texture, 0, static_cast<uint32_t>(subresources.size()), subresources.data());

                //    if (subresources.size() < textureResource->GetDesc().MipLevels)
                //    {
                //        GenerateMips(texture);
                //    }

                //    // Add the texture resource to the texture cache.
                //    ms_TextureCache[fileName] = textureResource.Get();
                //}

                return texture;
            }

            void CommandList::ClearTexture(const std::shared_ptr<Texture>& a_Texture, const float a_ClearColor[4])
            {
                assert(a_Texture);

                TransitionBarrier(a_Texture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
                m_d3d12CommandList->ClearRenderTargetView(a_Texture->GetRenderTargetView(), a_ClearColor, 0, nullptr);

                TrackResource(a_Texture);
            }

            void CommandList::ClearDepthStencilTexture(const std::shared_ptr<Texture>& a_Texture, D3D12_CLEAR_FLAGS a_ClearFlags, float a_Depth, uint8_t a_Stencil)
            {
                assert(a_Texture);

                TransitionBarrier(a_Texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
                m_d3d12CommandList->ClearDepthStencilView(a_Texture->GetDepthStencilView(), a_ClearFlags, a_Depth, a_Stencil, 0, nullptr);

                TrackResource(a_Texture);
            }

            void CommandList::GenerateMips(const std::shared_ptr<Texture>& a_Texture)
            {
                if (!a_Texture)
                {
                    return;
                }

                auto d3d12Device = core::ENGINE.GetDX12().GetDevice().GetD3D12Device();

                if (m_d3d12CommandListType == D3D12_COMMAND_LIST_TYPE_COPY)
                {
                    if (!m_ComputeCommandList)
                    {
                        m_ComputeCommandList = core::ENGINE.GetDX12().GetDevice().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE).GetCommandList();
                    }
                    m_ComputeCommandList->GenerateMips(a_Texture);
                    return;
                }

                auto d3d12Resource = a_Texture->GetD3D12Resource();

                // If the texture doesn't have a valid resource? Do nothing...
                if (!d3d12Resource)
                {
                    return;
                }
                auto resourceDesc = d3d12Resource->GetDesc();

                // If the texture only has a single mip level (level 0)
                // do nothing.
                if (resourceDesc.MipLevels == 1)
                {
                    return;
                }
                // Currently, only non-multi-sampled 2D textures are supported.
                if (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || resourceDesc.DepthOrArraySize != 1 ||
                    resourceDesc.SampleDesc.Count > 1)
                {
                    throw std::exception("GenerateMips is only supported for non-multi-sampled 2D Textures.");
                }

                Microsoft::WRL::ComPtr<ID3D12Resource> uavResource = d3d12Resource;
                // Create an alias of the original resource.
                // This is done to perform a GPU copy of resources with different formats.
                // BGR -> RGB texture copies will fail GPU validation unless performed
                // through an alias of the BRG resource in a placed heap.
                Microsoft::WRL::ComPtr<ID3D12Resource> aliasResource;

                // If the passed-in resource does not allow for UAV access
                // then create a staging resource that is used to generate
                // the mipmap chain.
                if (!a_Texture->CheckUAVSupport() || (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0)
                {
                    // Describe an alias resource that is used to copy the original texture.
                    auto aliasDesc = resourceDesc;
                    // Placed resources can't be render targets or depth-stencil views.
                    aliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                    aliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

                    // Describe a UAV compatible resource that is used to perform
                    // mipmapping of the original texture.
                    auto uavDesc = aliasDesc;  // The flags for the UAV description must match that of the alias description.
                    uavDesc.Format = Texture::GetUAVCompatableFormat(resourceDesc.Format);

                    D3D12_RESOURCE_DESC resourceDescs[] = { aliasDesc, uavDesc };

                    // Create a heap that is large enough to store a copy of the original resource.
                    auto allocationInfo = d3d12Device->GetResourceAllocationInfo(0, _countof(resourceDescs), resourceDescs);

                    D3D12_HEAP_DESC heapDesc = {};
                    heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
                    heapDesc.Alignment = allocationInfo.Alignment;
                    heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
                    heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                    heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                    heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

                    Microsoft::WRL::ComPtr<ID3D12Heap> heap;
                    if (FAILED(d3d12Device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating heap.");
                        return;
                    }

                    // Make sure the heap does not go out of scope until the command list
                    // is finished executing on the command queue.
                    TrackResource(heap);

                    // Create a placed resource that matches the description of the
                    // original resource. This resource is used to copy the original
                    // texture to the UAV compatible resource.
                    if (FAILED(d3d12Device->CreatePlacedResource(heap.Get(), 0, &aliasDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&aliasResource))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating placed resource.");
                        return;
                    }

                    ResourceStateTracker::AddGlobalResourceState(aliasResource.Get(), D3D12_RESOURCE_STATE_COMMON);
                    // Ensure the scope of the alias resource.
                    TrackResource(aliasResource);

                    // Create a UAV compatible resource in the same heap as the alias
                    // resource.
                    if (FAILED(d3d12Device->CreatePlacedResource(heap.Get(), 0, &uavDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&uavResource))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating placed resource.");
                        return;
                    }

                    ResourceStateTracker::AddGlobalResourceState(uavResource.Get(), D3D12_RESOURCE_STATE_COMMON);

                    // Ensure the scope of the UAV compatible resource.
                    TrackResource(uavResource);

                    // Add an aliasing barrier for the alias resource.
                    AliasingBarrier(nullptr, aliasResource);

                    // Copy the original resource to the alias resource.
                    // This ensures GPU validation.
                    CopyResource(aliasResource, d3d12Resource);

                    // Add an aliasing barrier for the UAV compatible resource.
                    AliasingBarrier(aliasResource, uavResource);
                }

                // Generate mips with the UAV compatible resource.
                auto uavTexture = core::ENGINE.GetDX12().GetDevice().CreateTexture(uavResource);
                GenerateMips_UAV(uavTexture, Texture::IsSRGBFormat(resourceDesc.Format));

                if (aliasResource)
                {
                    AliasingBarrier(uavResource, aliasResource);
                    // Copy the alias resource back to the original resource.
                    CopyResource(d3d12Resource, aliasResource);
                }
            }

            Microsoft::WRL::ComPtr<ID3D12Resource> CommandList::CopyBuffer(size_t a_BufferSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags)
            {
                Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
                if (a_BufferSize == 0)
                {
                    // This will result in a NULL resource (which may be desired to define a default null resource).
                }
                else
                {
                    auto d3d12Device = core::ENGINE.GetDX12().GetDevice();

                    if (FAILED(d3d12Device->CreateCommittedResource(
                        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
                        &CD3DX12_RESOURCE_DESC::Buffer(a_BufferSize, a_Flags), D3D12_RESOURCE_STATE_COMMON, nullptr,
                        IID_PPV_ARGS(&d3d12Resource))))
                    {
                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource when copying buffer.");
                        return nullptr;
                    }

                    // Add the resource to the global resource state tracker.
                    ResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

                    if (a_BufferData != nullptr)
                    {
                        // Create an upload resource to use as an intermediate buffer to copy the buffer resource
                        Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
                        if (FAILED(d3d12Device->CreateCommittedResource(
                            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
                            &CD3DX12_RESOURCE_DESC::Buffer(a_BufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                            IID_PPV_ARGS(&uploadResource))))
                        {
                            LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource when copying buffer.");
                            return nullptr;
                        }

                        D3D12_SUBRESOURCE_DATA subresourceData = {};
                        subresourceData.pData = a_BufferData;
                        subresourceData.RowPitch = a_BufferSize;
                        subresourceData.SlicePitch = subresourceData.RowPitch;

                        m_ResourceStateTracker->TransitionResource(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
                        FlushResourceBarriers();

                        UpdateSubresources(m_d3d12CommandList.Get(), d3d12Resource.Get(), uploadResource.Get(), 0, 0, 1,
                            &subresourceData);

                        // Add references to resources so they stay in scope until the command list is reset.
                        TrackResource(uploadResource);
                    }
                    TrackResource(d3d12Resource);
                }

                return d3d12Resource;
            }
}
	}
}