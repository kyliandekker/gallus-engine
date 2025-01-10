#pragma once

#include "core/System.h"

#include <Windows.h>
#include <dx12/directx/d3dx12.h>  // Or any related d3dx12 header
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <chrono>
#include <glm/vec2.hpp>
#include <cstdint>  // For uint64_t
#include <queue>    // For std::queue

#include "graphics/dx12/CommandQueue.h"

#define NOMINMAX

struct HWND__;
typedef HWND__* HWND;

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
				CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);

				/// <summary>
				/// Retrieves an available command list from the command queue.
				/// </summary>
				/// <returns>A ComPtr to an ID3D12GraphicsCommandList2.</returns>
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();

				/// <summary>
				/// Executes a command list and signals the fence for synchronization.
				/// </summary>
				/// <param name="commandList">The command list to execute.</param>
				/// <returns>The fence value associated with the executed command list.</returns>
				uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

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
				/// Creates a new command list using the specified command allocator.
				/// </summary>
				/// <param name="allocator">The command allocator to use.</param>
				/// <returns>A ComPtr to an ID3D12GraphicsCommandList2.</returns>
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

				/// <summary>
				/// Tracks command allocators currently in use.
				/// </summary>
				struct CommandAllocatorEntry
				{
					uint64_t fenceValue;
					Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				};

				using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>; /// Queue of command allocators.
				using CommandListQueue = std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>>; /// Queue of command lists.

				D3D12_COMMAND_LIST_TYPE                     m_CommandListType; /// Type of command list (e.g., DIRECT, BUNDLE).
				Microsoft::WRL::ComPtr<ID3D12Device2>       m_d3d12Device = nullptr; /// Associated ID3D12Device2.
				Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_d3d12CommandQueue = nullptr; /// The ID3D12CommandQueue.
				Microsoft::WRL::ComPtr<ID3D12Fence>         m_d3d12Fence = nullptr; /// Fence for synchronization.
				HANDLE                                      m_FenceEvent = nullptr; /// Event handle for fence synchronization.
				uint64_t                                    m_FenceValue; /// Current fence value.

				CommandAllocatorQueue                       m_CommandAllocatorQueue; /// Queue of in-flight command allocators.
				CommandListQueue                            m_CommandListQueue; /// Queue of available command lists.
			};

			inline const uint8_t g_NumSwapChainBuffers = 3; /// Number of swap chain buffers.
			inline const uint8_t g_NumSrvBuffers = 2; /// Number of SRV buffers.
			inline bool g_UseWarp = false; /// Whether to use WARP (Windows Advanced Rasterization Platform).
			inline bool g_VSync = false; /// Whether V-Sync is enabled.

			/// <summary>
			/// Tracks and calculates frames per second (FPS).
			/// </summary>
			class FPSCounter
			{
			public:

				/// <summary>
				/// Retrieves the current frames per second (FPS).
				/// </summary>
				/// <returns>The FPS as a double.</returns>
				double GetFPS() const;
			private:
				friend class DX12System;

				/// <summary>
				/// Updates the FPS counter.
				/// </summary>
				void Update();

				/// <summary>
				/// Initializes the FPS counter.
				/// </summary>
				void Initialize();

				double m_FPS = 0.0; /// The FPS as a double.
				uint64_t m_FrameCounter = 0; /// The number of frames.
				double m_ElapsedSeconds = 0.0; /// The number of seconds since the last frame.
				std::chrono::high_resolution_clock m_Clock; /// The clock.
				std::chrono::steady_clock::time_point m_T0 = m_Clock.now(); /// The clock from first frame.
			};

#define CATEGORY_DX12 "DX12"

			/// <summary>
			/// Represents a DirectX 12 rendering window, managing device resources, rendering, and synchronization.
			/// </summary>
			class DX12System : public core::ThreadedSystem
			{
			public:
				/// <summary>
				/// Initializes the system, setting up necessary resources.
				/// </summary>
				/// <param name="a_hWnd">Handle to the window.</param>
				/// <param name="a_Size">Size of the window.</param>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool Initialize(HWND a_hWnd, const glm::ivec2 a_Size);

				/// <summary>
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Cleans up resources and destroys the dx12 window.
				/// </summary>
				/// <returns>True if destruction succeeds, otherwise false.</returns>
				bool Destroy() override;

				/// <summary>
				/// Resizes the rendering window and updates resources.
				/// </summary>
				/// <param name="a_Size">New size of the window.</param>
				void Resize(const glm::ivec2& a_Size);

				/// <summary>
				/// Resets the rendering pipeline resources.
				/// </summary>
				void Reset();

				/// <summary>
				/// Retrieves the DirectX 12 device used by this window.
				/// </summary>
				/// <returns>A ComPtr to the ID3D12Device2 instance.</returns>
				Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const;

				/// <summary>
				/// Retrieves the SRV descriptor heap.
				/// </summary>
				/// <returns>A ComPtr to the SRV descriptor heap.</returns>
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRVDescriptorHeap() const;

				/// <summary>
				/// Retrieves the current FPS counter.
				/// </summary>
				/// <returns>An instance of FPSCounter containing FPS information.</returns>
				FPSCounter GetFPS() const;

				/// <summary>
				/// Waits for all GPU commands to complete execution.
				/// </summary>
				void Wait();

				/// <summary>
				/// Signals the GPU to stop waiting.
				/// </summary>
				void StopWaiting();

				Microsoft::WRL::ComPtr<ID3D12Resource> m_RenderTargetTexture; /// The render target texture used for rendering.
				CD3DX12_CPU_DESCRIPTOR_HANDLE m_RenderTargetSrvHandleCPU; /// The CPU handle for the render target SRV.
				CD3DX12_GPU_DESCRIPTOR_HANDLE m_RenderTargetSrvHandleGPU; /// The GPU handle for the render target SRV.
			protected:
				/// <summary>
				/// Initializes the thread.
				/// </summary>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool InitializeThread() override;

				/// <summary>
				/// Destroys the system, releasing resources and performing necessary cleanup.
				/// </summary>
				/// <returns>True if the destruction was successful, otherwise false.</returns>
				void Finalize() override;

				/// <summary>
				/// Enables the debug layer for DirectX 12.
				/// </summary>
				void EnableDebugLayer();

				/// <summary>
				/// Checks if tearing is supported for V-Sync-less rendering.
				/// </summary>
				/// <returns>True if tearing is supported, otherwise false.</returns>
				bool CheckTearingSupport();

				/// <summary>
				/// Retrieves the DXGI adapter.
				/// </summary>
				/// <param name="a_UseWarp">Whether to use the WARP adapter.</param>
				/// <returns>True if the adapter was successfully retrieved, otherwise false.</returns>
				bool GetAdapter(bool a_UseWarp);

				/// <summary>
				/// Creates the DirectX 12 device.
				/// </summary>
				/// <param name="a_Adapter">The DXGI adapter to use for the device.</param>
				/// <returns>True if the device was successfully created, otherwise false.</returns>
				bool CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> a_Adapter);

				/// <summary>
				/// Creates the swap chain.
				/// </summary>
				/// <param name="hWnd">Handle to the window.</param>
				/// <param name="a_CommandQueue">Command queue for the swap chain.</param>
				/// <param name="a_Width">Width of the swap chain buffers.</param>
				/// <param name="a_Height">Height of the swap chain buffers.</param>
				/// <param name="a_BufferCount">Number of swap chain buffers.</param>
				/// <returns>True if the swap chain was successfully created, otherwise false.</returns>
				bool CreateSwapChain(HWND hWnd, Microsoft::WRL::ComPtr<ID3D12CommandQueue> a_CommandQueue, uint32_t a_Width, uint32_t a_Height, uint32_t a_BufferCount);

				/// <summary>
				/// Creates a descriptor heap.
				/// </summary>
				/// <param name="a_Device">The DirectX 12 device used to create the descriptor heap.</param>
				/// <param name="a_Type">The type of descriptor heap (e.g., RTV, DSV, CBV_SRV_UAV).</param>
				/// <param name="a_Flags">Flags for the descriptor heap (e.g., SHADER_VISIBLE).</param>
				/// <param name="a_NumDescriptors">Number of descriptors in the heap.</param>
				/// <param name="a_DescriptorHeap">The resulting descriptor heap.</param>
				/// <returns>True if the descriptor heap was successfully created, otherwise false.</returns>
				bool CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, D3D12_DESCRIPTOR_HEAP_TYPE a_Type, D3D12_DESCRIPTOR_HEAP_FLAGS a_Flags, uint32_t a_NumDescriptors, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& a_DescriptorHeap);

				/// <summary>
				/// Updates the render target views (RTVs) for the back buffers.
				/// </summary>
				/// <param name="a_Device">The DirectX 12 device.</param>
				/// <param name="a_SwapChain">The swap chain.</param>
				/// <param name="a_DescriptorHeap">Descriptor heap for the RTVs.</param>
				/// <param name="a_Type">Descriptor heap type.</param>
				/// <param name="a_NumDescriptors">Number of descriptors in the heap.</param>
				void UpdateRenderTargetViews(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, Microsoft::WRL::ComPtr<IDXGISwapChain4> a_SwapChain, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> a_DescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptors);

				/// <summary>
				/// Creates a command allocator.
				/// </summary>
				/// <param name="a_Device">The DirectX 12 device.</param>
				/// <param name="a_Type">The type of command list (e.g., DIRECT, COMPUTE, COPY).</param>
				/// <returns>The created command allocator as a ComPtr.</returns>
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, D3D12_COMMAND_LIST_TYPE a_Type);

				/// <summary>
				/// Creates a graphics command list.
				/// </summary>
				/// <param name="a_Device">The DirectX 12 device.</param>
				/// <param name="a_CommandAllocator">The command allocator for the command list.</param>
				/// <param name="a_Type">The type of command list (e.g., DIRECT, COMPUTE, COPY).</param>
				/// <returns>The created graphics command list as a ComPtr.</returns>
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> a_CommandAllocator, D3D12_COMMAND_LIST_TYPE a_Type);

				/// <summary>
				/// Creates a GPU fence for synchronization.
				/// </summary>
				/// <param name="a_Device">The DirectX 12 device.</param>
				/// <returns>True if the fence was successfully created, otherwise false.</returns>
				bool CreateFence(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device);

				/// <summary>
				/// Creates an event handle for CPU-GPU synchronization.
				/// </summary>
				/// <returns>True if the event handle was successfully created, otherwise false.</returns>
				bool CreateEventHandle();

				/// <summary>
				/// Clears a render target view (RTV) with a specified clear color.
				/// </summary>
				/// <param name="a_CommandList">The command list to record the clear operation.</param>
				/// <param name="a_Rtv">The descriptor handle of the RTV to clear.</param>
				/// <param name="a_ClearColor">The clear color as an array of four floats (RGBA).</param>
				void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_Rtv, FLOAT* a_ClearColor);

				/// <summary>
				/// Clears a depth stencil view (DSV) with a specified depth value.
				/// </summary>
				/// <param name="a_CommandList">The command list to record the clear operation.</param>
				/// <param name="a_Dsv">The descriptor handle of the DSV to clear.</param>
				/// <param name="a_Depth">The depth value to clear with.</param>
				void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_Dsv, FLOAT a_Depth);

				/// <summary>
				/// Transitions a resource from one state to another.
				/// </summary>
				/// <param name="a_CommandList">The command list to record the transition operation.</param>
				/// <param name="a_Resource">The resource to transition.</param>
				/// <param name="a_BeforeState">The current state of the resource.</param>
				/// <param name="a_AfterState">The desired state of the resource.</param>
				void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource, D3D12_RESOURCE_STATES a_BeforeState, D3D12_RESOURCE_STATES a_AfterState);

				/// <summary>
				/// Cleans up the render targets associated with the swap chain.
				/// </summary>
				/// <param name="a_SwapChain">The swap chain containing the render targets to clean up.</param>
				void CleanUpRenderTargets(Microsoft::WRL::ComPtr<IDXGISwapChain4> a_SwapChain);

				/// <summary>
				/// Ensures all previously submitted GPU commands are completed before continuing execution.
				/// </summary>
				/// <remarks>
				/// This method blocks the CPU until the GPU has finished processing all commands up to this point. 
				/// It is useful for synchronization, resource cleanup, or preparing for device destruction.
				/// </remarks>
				void Flush();

				/// <summary>
				/// Creates an offscreen render target for custom rendering operations.
				/// </summary>
				/// <param name="a_Width">The width of the render target.</param>
				/// <param name="a_Height">The height of the render target.</param>
				/// <returns>True if the render target was successfully created, otherwise false.</returns>
				bool CreateOffscreenRenderTarget(uint32_t a_Width, uint32_t a_Height);

				HWND m_hWnd = nullptr;

				FPSCounter m_FpsCounter; /// The FPSCounter instance for tracking and reporting frames per second.
				glm::ivec2 m_Size; /// The current size of the window or rendering area.

				CommandQueue m_DirectCommandQueue; /// Command queue for submitting DirectX 12 commands.

				Microsoft::WRL::ComPtr<IDXGIAdapter4> g_DxgiAdapter4; /// The DirectX Graphics Infrastructure (DXGI) adapter used for the device.
				Microsoft::WRL::ComPtr<ID3D12Device2> g_Device; /// The DirectX 12 device interface for creating resources and managing GPU state.
				Microsoft::WRL::ComPtr<IDXGISwapChain4> g_SwapChain; /// The swap chain responsible for presenting rendered frames to the screen.
				Microsoft::WRL::ComPtr<ID3D12Resource> g_BackBuffers[g_NumSwapChainBuffers]; /// Back buffers used for rendering in the swap chain.
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap; /// Descriptor heap for render target views (RTVs).
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> g_SRVDescriptorHeap; /// Descriptor heap for shader resource views (SRVs).

				Microsoft::WRL::ComPtr<ID3D12Fence> g_Fence; /// The fence object used for GPU synchronization.
				bool g_TearingSupported = false; /// Indicates whether tearing (vsync off) is supported by the system.
				uint64_t g_FenceValue = 0; /// Current fence value for synchronization.
				uint64_t g_FrameFenceValues[g_NumSwapChainBuffers] = {}; /// Fence values for each back buffer frame to ensure synchronization.
				HANDLE g_FenceEvent; /// Event handle used to wait for fence completion.
				UINT g_CurrentBackBufferIndex = 0; /// The index of the current back buffer being rendered to.
				UINT g_RTVDescriptorSize = 0; /// The size of a single descriptor in the RTV descriptor heap.
				UINT g_SRVDescriptorSize = 0; /// The size of a single descriptor in the SRV descriptor heap.
				UINT m_ExtraRenderTargets = 1; /// The number of additional render targets beyond the default ones.
			};
		}
	}
}