#pragma once

#include "core/System.h"

#include <Windows.h>
#include <dx12/directx/d3dx12.h>  // Or any related d3dx12 header
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <chrono>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <cstdint>  // For uint64_t
#include <queue>    // For std::queue
#include <DirectXMath.h>

#include "graphics/dx12/CommandQueue.h"

#undef min
#undef max

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

			inline const uint8_t BufferCount = 3; /// Number of swap chain buffers.
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

				double GetTotalTime() const;
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
				double m_TotalTime = 0.0; /// The number of seconds since the last frame.
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
				/// <param name="a_Wait">Determines whether the application waits until the system has been fully initialized.</param>
				/// <param name="a_hWnd">Handle to the window.</param>
				/// <param name="a_Size">Size of the window.</param>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool Initialize(bool a_Wait, HWND a_hWnd, const glm::ivec2 a_Size);

				/// <summary>
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Cleans up resources and destroys the dx12 window.
				/// </summary>
				/// <returns>True if destruction succeeds, otherwise false.</returns>
				bool Destroy() override;

				void IncreaseFov();

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

				Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;

				UINT GetCurrentBackBufferIndex() const;

				D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;

				void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

				void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);

				void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);

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
				bool CreateDevice();

				/// <summary>
				/// Checks if tearing is supported for V-Sync-less rendering.
				/// </summary>
				/// <returns>True if tearing is supported, otherwise false.</returns>
				bool CheckTearingSupport();

				Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const;

				std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_Type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
				
				/// <summary>
				/// Ensures all previously submitted GPU commands are completed before continuing execution.
				/// </summary>
				/// <remarks>
				/// This method blocks the CPU until the GPU has finished processing all commands up to this point. 
				/// It is useful for synchronization, resource cleanup, or preparing for device destruction.
				/// </remarks>
				void Flush();

				/// <summary>
				/// Creates the swap chain.
				/// </summary>
				/// <returns>True if the swap chain was successfully created, otherwise false.</returns>
				bool CreateSwapChain();

				/// <summary>
				/// Creates a descriptor heap.
				/// </summary>
				/// <param name="a_NumDescriptors">Number of descriptors in the heap.</param>
				/// <param name="a_Type">The type of descriptor heap (e.g., RTV, DSV, CBV_SRV_UAV).</param>
				/// <param name="a_Flags">Flags for the descriptor heap (e.g., SHADER_VISIBLE).</param>
				/// <returns>The descriptor heap.</returns>
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT a_NumDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE a_Type);

				UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE a_Type) const;

				/// <summary>
				/// Updates the render target views (RTVs) for the back buffers.
				/// </summary>
				/// <param name="a_Device">The DirectX 12 device.</param>
				/// <param name="a_SwapChain">The swap chain.</param>
				/// <param name="a_DescriptorHeap">Descriptor heap for the RTVs.</param>
				/// <param name="a_Type">Descriptor heap type.</param>
				/// <param name="a_NumDescriptors">Number of descriptors in the heap.</param>
				void UpdateRenderTargetViews();

				void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList, ID3D12Resource** a_pDestinationResource, ID3D12Resource** a_pIntermediateResource, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags = D3D12_RESOURCE_FLAG_NONE);

				void ResizeDepthBuffer(const glm::ivec2& a_Size);

				// The application instance handle that this application was created with.
				HINSTANCE m_hInstance;

				Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
				Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;

				std::shared_ptr<CommandQueue> m_DirectCommandQueue;
				std::shared_ptr<CommandQueue> m_ComputeCommandQueue;
				std::shared_ptr<CommandQueue> m_CopyCommandQueue;

				uint64_t m_FenceValues[BufferCount] = {};

				// Vertex buffer for the cube.
				Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
				D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
				// Index buffer for the cube.
				Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
				D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

				// Depth buffer.
				Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthBuffer;
				// Descriptor heap for depth buffer.
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

				// Root signature
				Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

				// Pipeline state object.
				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

				D3D12_VIEWPORT m_Viewport;
				D3D12_RECT m_ScissorRect;

				float m_FoV;

				DirectX::XMMATRIX m_ModelMatrix;
				DirectX::XMMATRIX m_ViewMatrix;
				DirectX::XMMATRIX m_ProjectionMatrix;

				HWND m_hWnd;

				std::wstring m_WindowName;

				bool m_VSync;
				bool m_Fullscreen;

				uint64_t m_FrameCounter;

				Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3d12RTVDescriptorHeap;
				Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12BackBuffers[BufferCount];

				UINT m_RTVDescriptorSize;
				UINT m_CurrentBackBufferIndex;

				RECT m_WindowRect;
				bool m_IsTearingSupported;

				glm::ivec2 m_Size;

				FPSCounter m_FpsCounter;
			};
		}
	}
}