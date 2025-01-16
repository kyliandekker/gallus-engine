//#include "graphics/dx12/CommandList.h"
//
//#include <d3d12.h>
//
//#include "core/logger/Logger.h"
//#include "core/Engine.h"
//#include "graphics/dx12/DX12Resource.h"
//
//namespace coopscoop
//{
//	namespace graphics
//	{
//		namespace dx12
//		{
//            CommandList::CommandList(D3D12_COMMAND_LIST_TYPE a_Type)
//                : m_d3d12CommandListType(a_Type)
//            {
//                auto device = core::ENGINE.GetDX12().GetDevice();
//
//                if (FAILED(device->CreateCommandAllocator(m_d3d12CommandListType, IID_PPV_ARGS(&m_d3d12CommandAllocator))))
//                {
//                    LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create a command allocator.");
//                    return;
//                }
//
//                if (FAILED(device->CreateCommandList(0, m_d3d12CommandListType, m_d3d12CommandAllocator.Get(),
//                    nullptr, IID_PPV_ARGS(&m_d3d12CommandList))))
//                {
//                    LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create a command list.");
//                    return;
//                }
//            }
//
//            void CommandList::TransitionBarrier(DX12Resource& a_Resource, D3D12_RESOURCE_STATES a_StateAfter, UINT a_Subresource, bool a_FlushBarriers)
//            {
//                auto d3d12Resource = a_Resource.GetResource();
//                if (d3d12Resource)
//                {
//                    // The "before" state is not important. It will be resolved by the resource state tracker.
//                    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON, a_StateAfter, a_Subresource);
//                }
//            }
//
//            void CommandList::CopyVertexBuffer(VertexBuffer& a_VertexBuffer, size_t a_NumVertices, size_t a_VertexStride, const void* a_VertexBufferData)
//            {
//                CopyBuffer(a_VertexBuffer, a_NumVertices, a_VertexStride, a_VertexBufferData);
//            }
//
//            void CommandList::CopyIndexBuffer(IndexBuffer& a_IndexBuffer, size_t a_NumIndicies, DXGI_FORMAT a_IndexFormat, const void* a_IndexBufferData)
//            {
//                size_t indexSizeInBytes = a_IndexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
//                CopyBuffer(a_IndexBuffer, a_NumIndicies, indexSizeInBytes, a_IndexBufferData);
//            }
//
//            void CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY a_PrimitiveTopology)
//            {
//                m_d3d12CommandList->IASetPrimitiveTopology(a_PrimitiveTopology);
//            }
//
//            void CommandList::ReleaseTrackedObjects()
//            {
//                m_TrackedObjects.clear();
//            }
//
//            void CommandList::CopyBuffer(Buffer& a_Buffer, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags)
//            {
//                auto device = core::ENGINE.GetDX12().GetDevice();
//
//                size_t bufferSize = a_NumElements * a_ElementSize;
//
//                Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
//                if (bufferSize == 0)
//                {
//                    // This will result in a NULL resource (which may be desired to define a default null resource).
//                }
//                else
//                {
//                    CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
//                    CD3DX12_RESOURCE_DESC defaultBuffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, a_Flags);
//                    if (FAILED(device->CreateCommittedResource(
//                        &defaultHeap,
//                        D3D12_HEAP_FLAG_NONE,
//                        &defaultBuffer,
//                        D3D12_RESOURCE_STATE_COMMON,
//                        nullptr,
//                        IID_PPV_ARGS(&d3d12Resource))))
//                    {
//                        LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create commited resource.");
//                        return;
//                    }
//
//                    if (a_BufferData != nullptr)
//                    {
//                        CD3DX12_HEAP_PROPERTIES upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
//                        CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//                        // Create an upload resource to use as an intermediate buffer to copy the buffer resource 
//                        Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
//                        if (FAILED(device->CreateCommittedResource(
//                            &upload,
//                            D3D12_HEAP_FLAG_NONE,
//                            &buffer,
//                            D3D12_RESOURCE_STATE_GENERIC_READ,
//                            nullptr,
//                            IID_PPV_ARGS(&uploadResource))))
//                        {
//                            LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create commited resource.");
//                            return;
//                        }
//
//                        D3D12_SUBRESOURCE_DATA subresourceData = {};
//                        subresourceData.pData = a_BufferData;
//                        subresourceData.RowPitch = bufferSize;
//                        subresourceData.SlicePitch = subresourceData.RowPitch;
//
//                        UpdateSubresources(m_d3d12CommandList.Get(), d3d12Resource.Get(),
//                            uploadResource.Get(), 0, 0, 1, &subresourceData);
//
//                        // Add references to resources so they stay in scope until the command list is reset.
//                        m_TrackedObjects.push_back(uploadResource);
//                    }
//                    m_TrackedObjects.push_back(d3d12Resource);
//                }
//
//                a_Buffer.SetResource(d3d12Resource);
//                a_Buffer.CreateViews(a_NumElements, a_ElementSize);
//            }
//
//            bool CommandList::Close(CommandList& a_PendingCommandList)
//            {
//                // Flush any remaining barriers.
//                FlushResourceBarriers();
//
//                m_d3d12CommandList->Close();
//
//                // Commit the final resource state to the global state.
//                m_ResourceStateTracker->CommitFinalResourceStates();
//
//                return numPendingBarriers > 0;
//            }
//
//            void CommandList::Close()
//            {
//                FlushResourceBarriers();
//                m_d3d12CommandList->Close();
//            }
//		}
//	}
//}