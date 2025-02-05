#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <cstdint>  // For uint64_t
#include <queue>    // For std::queue
#include <memory>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

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
				/// <param name="a_CommandListType">The type of command list (e.g., DIRECT, BUNDLE).</param>
				/// <param name="a_Device">The dx12 device.</param>
				CommandQueue(D3D12_COMMAND_LIST_TYPE a_CommandListType, Microsoft::WRL::ComPtr<ID3D12Device2> a_Device);

				/// <summary>
				/// Retrieves an available command list from the command queue.
				/// </summary>
				/// <returns>A shared pointer to the command list.</returns>
				std::shared_ptr<CommandList> GetCommandList();

				/// <summary>
				/// Executes a command list and signals the fence for synchronization.
				/// </summary>
				/// <param name="a_CommandList">The command list to execute.</param>
				/// <returns>The fence value associated with the executed command list.</returns>
				uint64_t ExecuteCommandList(std::shared_ptr<CommandList> a_CommandList);

				/// <summary>
				/// Signals the command queue's fence.
				/// </summary>
				/// <returns>The new fence value.</returns>
				uint64_t Signal();

				/// <summary>
				/// Checks if the specified fence value has been reached.
				/// </summary>
				/// <param name="a_FenceValue">The fence value to check.</param>
				/// <returns>True if the fence value has been reached, false otherwise.</returns>
				bool IsFenceComplete(uint64_t a_FenceValue);

				/// <summary>
				/// Waits for the specified fence value to be reached.
				/// </summary>
				/// <param name="a_FenceValue">The fence value to wait for.</param>
				void WaitForFenceValue(uint64_t a_FenceValue);

				/// <summary>
				/// Flushes the command queue, ensuring all GPU work is complete.
				/// </summary>
				void Flush();

				/// <summary>
				/// Retrieves the underlying ID3D12CommandQueue.
				/// </summary>
				/// <returns>A ComPtr to the ID3D12CommandQueue.</returns>
				Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const;
			private:
				Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;

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
					uint64_t m_FenceValue;
					Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
				};

				using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>; /// Queue of command allocators.
				using CommandListQueue = std::queue<std::shared_ptr<CommandList>>; /// Queue of command lists.

				D3D12_COMMAND_LIST_TYPE                     m_CommandListType; /// Type of command list (e.g., DIRECT, BUNDLE).
				Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_CommandQueue = nullptr; /// The ID3D12CommandQueue.
				Microsoft::WRL::ComPtr<ID3D12Fence>         m_Fence = nullptr; /// Fence for synchronization.
				HANDLE                                      m_FenceEvent = nullptr; /// Event handle for fence synchronization.
				uint64_t                                    m_FenceValue; /// Current fence value.

				CommandAllocatorQueue                       m_CommandAllocatorQueue; /// Queue of in-flight command allocators.
				CommandListQueue                            m_CommandListQueue; /// Queue of available command lists.
			};
		}
	}
}