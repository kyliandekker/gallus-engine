#include "graphics/dx12/CommandQueue.h"

#include <d3d12.h>

#include "core/logger/Logger.h"
#include "core/Engine.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
#pragma region DX12_COMMAND_QUEUE
			CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE a_CommandListType) :
				m_CommandListType(a_CommandListType),
				m_FenceValue(0)
			{
				D3D12_COMMAND_QUEUE_DESC desc = {};
				desc.Type = a_CommandListType;
				desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				desc.NodeMask = 0;

				if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command queue.");
					return;
				}
				if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating fence.");
					return;
				}

				m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
				if (!m_FenceEvent)
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating fence event.");
					return;
				}
			}

			std::shared_ptr<CommandList> CommandQueue::GetCommandList()
			{
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				std::shared_ptr<CommandList> commandList = std::make_shared<CommandList>();

				if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue))
				{
					commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;
					m_CommandAllocatorQueue.pop();

					if (FAILED(commandAllocator->Reset()))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resetting command allocator.");
						return commandList;
					}
				}
				else
				{
					commandAllocator = CreateCommandAllocator();
				}

				if (!m_CommandListQueue.empty())
				{
					commandList = m_CommandListQueue.front();
					m_CommandListQueue.pop();

					if (FAILED((commandList->GetCommandList()->Reset(commandAllocator.Get(), nullptr))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resetting command list.");
						return commandList;
					}
				}
				else
				{
					commandList->CreateCommandList(commandAllocator, m_CommandListType);
				}

				// Associate the command allocator with the command list so that it can be
				// retrieved when the command list is executed.
				if (FAILED(commandList->GetCommandList()->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get())))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed associating command allocator with command list.");
					return commandList;
				}

				return commandList;
			}

			uint64_t CommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> commandList)
			{
				commandList->GetCommandList()->Close();

				ID3D12CommandAllocator* commandAllocator;
				UINT dataSize = sizeof(commandAllocator);
				if (FAILED(commandList->GetCommandList()->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed setting private data for command list.");
					return -1;
				}

				ID3D12CommandList* const ppCommandLists[] = {
					commandList->GetCommandList().Get()
				};

				m_d3d12CommandQueue->ExecuteCommandLists(1, ppCommandLists);
				uint64_t fenceValue = Signal();

				m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
				m_CommandListQueue.push(commandList);

				// The ownership of the command allocator has been transferred to the ComPtr
				// in the command allocator queue. It is safe to release the reference 
				// in this temporary COM pointer here.
				commandAllocator->Release();

				return fenceValue;
			}

			uint64_t CommandQueue::Signal()
			{
				uint64_t fenceValue = ++m_FenceValue;
				m_d3d12CommandQueue->Signal(m_d3d12Fence.Get(), fenceValue);
				return fenceValue;
			}

			bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
			{
				return m_d3d12Fence->GetCompletedValue() >= fenceValue;
			}

			void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
			{
				if (!IsFenceComplete(fenceValue))
				{
					m_d3d12Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
					::WaitForSingleObject(m_FenceEvent, DWORD_MAX);
				}
			}

			void CommandQueue::Flush()
			{
				WaitForFenceValue(Signal());
			}

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const
			{
				return m_d3d12CommandQueue;
			}

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
			{
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command allocator.");
					return nullptr;
				}

				return commandAllocator;
			}
		}
	}
}