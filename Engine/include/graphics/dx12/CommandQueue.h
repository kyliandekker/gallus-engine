#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <cstdint>  // For uint64_t
#include <queue>    // For std::queue
#include <memory>

#include "graphics/dx12/CommandList.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			/// <summary>
			/// A wrapper for an ID3D12CommandQueue to manage DirectX 12 command 
			/// lists, allocators, and fence synchronization.
			/// </summary>
			class CommandQueue
			{
			public:
				/// <summary>
				/// Default constructor.
				/// </summary>
				CommandQueue() = default;

				/// <summary>
				/// Constructs a CommandQueue with the specified device and command list type.
				/// </summary>
				/// <param name="device">The ID3D12Device2 instance.</param>
				/// <param name="type">The type of command list (e.g., DIRECT, BUNDLE).</param>
				CommandQueue(D3D12_COMMAND_LIST_TYPE a_CommandListType);

				/// <summary>
				/// Retrieves an available command list from the command queue.
				/// </summary>
				/// <returns>A shared pointer to the command list.</returns>
				std::shared_ptr<CommandList> GetCommandList();

				/// <summary>
				/// Executes a command list and signals the fence for synchronization.
				/// </summary>
				/// <param name="commandList">The command list to execute.</param>
				/// <returns>The fence value associated with the executed command list.</returns>
				uint64_t ExecuteCommandList(std::shared_ptr<CommandList> commandList);

				/// <summary>
				/// Signals the command queue's fence.
				/// </summary>
				/// <returns>The new fence value.</returns>
				uint64_t Signal();

				/// <summary>
				/// Checks if the specified fence value has been reached.
				/// </summary>
				/// <param name="fenceValue">The fence value to check.</param>
				/// <returns>True if the fence value has been reached, false otherwise.</returns>
				bool IsFenceComplete(uint64_t fenceValue);

				/// <summary>
				/// Waits for the specified fence value to be reached.
				/// </summary>
				/// <param name="fenceValue">The fence value to wait for.</param>
				void WaitForFenceValue(uint64_t fenceValue);

				/// <summary>
				/// Flushes the command queue, ensuring all GPU work is complete.
				/// </summary>
				void Flush();

				/// <summary>
				/// Retrieves the underlying ID3D12CommandQueue.
				/// </summary>
				/// <returns>A ComPtr to the ID3D12CommandQueue.</returns>
				Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;
			private:
				/// <summary>
				/// Creates a new command allocator.
				/// </summary>
				/// <returns>A ComPtr to an ID3D12CommandAllocator.</returns>
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();

				/// <summary>
				/// Tracks command allocators currently in use.
				/// </summary>
				struct CommandAllocatorEntry
				{
					uint64_t fenceValue;
					Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				};

				using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>; /// Queue of command allocators.
				using CommandListQueue = std::queue<std::shared_ptr<CommandList>>; /// Queue of command lists.

				D3D12_COMMAND_LIST_TYPE                     m_CommandListType; /// Type of command list (e.g., DIRECT, BUNDLE).
				Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_d3d12CommandQueue = nullptr; /// The ID3D12CommandQueue.
				Microsoft::WRL::ComPtr<ID3D12Fence>         m_d3d12Fence = nullptr; /// Fence for synchronization.
				HANDLE                                      m_FenceEvent = nullptr; /// Event handle for fence synchronization.
				uint64_t                                    m_FenceValue; /// Current fence value.

				CommandAllocatorQueue                       m_CommandAllocatorQueue; /// Queue of in-flight command allocators.
				CommandListQueue                            m_CommandListQueue; /// Queue of available command lists.
			};
		}
	}
}