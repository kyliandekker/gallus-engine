#include "graphics/dx12/CommandQueue.h"

#include <d3d12.h>

#include "core/logger/Logger.h"
#include "graphics/dx12/CommandList.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
#pragma region DX12_COMMAND_QUEUE
			CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE a_CommandListType, Microsoft::WRL::ComPtr<ID3D12Device2> a_Device) :
				m_CommandListType(a_CommandListType),
				m_Device(a_Device),
				m_FenceValue(0)
			{
				D3D12_COMMAND_QUEUE_DESC desc = {};
				desc.Type = a_CommandListType;
				desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				desc.NodeMask = 0;

				if (FAILED(m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command queue.");
					return;
				}
				if (FAILED(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))))
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

				if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().m_FenceValue))
				{
					commandAllocator = m_CommandAllocatorQueue.front().m_CommandAllocator;
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
					commandList->CreateCommandList(commandAllocator, m_CommandListType, m_Device);
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

			uint64_t CommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> a_CommandList)
			{
				a_CommandList->GetCommandList()->Close();

				ID3D12CommandAllocator* commandAllocator;
				UINT dataSize = sizeof(commandAllocator);
				if (FAILED(a_CommandList->GetCommandList()->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed setting private data for command list.");
					return -1;
				}

				ID3D12CommandList* const ppCommandLists[] = {
					a_CommandList->GetCommandList().Get()
				};

				m_CommandQueue->ExecuteCommandLists(1, ppCommandLists);
				uint64_t fenceValue = Signal();

				m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
				m_CommandListQueue.push(a_CommandList);

				// The ownership of the command allocator has been transferred to the ComPtr
				// in the command allocator queue. It is safe to release the reference 
				// in this temporary COM pointer here.
				commandAllocator->Release();

				return fenceValue;
			}

			uint64_t CommandQueue::Signal()
			{
				uint64_t fenceValue = ++m_FenceValue;
				m_CommandQueue->Signal(m_Fence.Get(), fenceValue);
				return fenceValue;
			}

			bool CommandQueue::IsFenceComplete(uint64_t a_FenceValue)
			{
				return m_Fence->GetCompletedValue() >= a_FenceValue;
			}

			void CommandQueue::WaitForFenceValue(uint64_t a_FenceValue)
			{
				if (!IsFenceComplete(a_FenceValue))
				{
					m_Fence->SetEventOnCompletion(a_FenceValue, m_FenceEvent);
					::WaitForSingleObject(m_FenceEvent, DWORD_MAX);
				}
			}

			void CommandQueue::Flush()
			{
				WaitForFenceValue(Signal());
			}

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetCommandQueue() const
			{
				return m_CommandQueue;
			}

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
			{
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				if (FAILED(m_Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command allocator.");
					return nullptr;
				}

				return commandAllocator;
			}
		}
	}
}