#pragma once

#include "core/System.h"

#include "graphics/dx12/DX12PCH.h"
#include <chrono>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/ResourceAtlas.h"

#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/Light.h"
#include "graphics/dx12/MeshComponent.h"
#include "editor/imgui/ImGuiWindow.h"

#undef min
#undef max

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			inline const uint8_t g_BufferCount = 3; /// Number of swap chain buffers.
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

			enum RootParameters
			{
				CBV,                // ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);
				TEX_SRV,            // Texture2D texture0 : register(t0);
				MATERIAL,            // ConstantBuffer<DirectionalLightCB> DirectionalLightCBCB : register(b1);
				LIGHT,            // ConstantBuffer<DirectionalLightCB> DirectionalLightCBCB : register(b1);
				NumRootParameters
			};

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
				bool Initialize(bool a_Wait, HWND a_hWnd, const glm::ivec2& a_Size);

				/// <summary>
				/// Cleans up resources and destroys the dx12 window.
				/// </summary>
				/// <returns>True if destruction succeeds, otherwise false.</returns>
				bool Destroy() override;
			private:
				/// <summary>
				/// Initializes the thread.
				/// </summary>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool InitializeThread() override;

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

				/// <summary>
				/// Creates the swap chain.
				/// </summary>
				/// <returns>True if the swap chain was successfully created, otherwise false.</returns>
				bool CreateSwapChain();

				/// <summary>
				/// Creates the RTV.
				/// </summary>
				void CreateRTV();

				/// <summary>
				/// Creates the DSV.
				/// </summary>
				void CreateDSV();

				/// <summary>
				/// Creates the SRV.
				/// </summary>
				void CreateSRV();

#ifdef __EDITOR__
				/// <summary>
				/// Creates the Render Texture.
				/// </summary>
				void CreateRenderTexture();
#endif // __EDITOR__

				/// <summary>
				/// Creates the DirectX 12 root signature.
				/// </summary>
				/// <returns>True if the root signature was successfully created, otherwise false.</returns>
				bool CreateRootSignature();

				/// <summary>
				/// Destroys the system, releasing resources and performing necessary cleanup.
				/// </summary>
				/// <returns>True if the destruction was successful, otherwise false.</returns>
				void Finalize() override;

				/// <summary>
				/// Ensures all previously submitted GPU commands are completed before continuing execution.
				/// </summary>
				/// <remarks>
				/// This method blocks the CPU until the GPU has finished processing all commands up to this point. 
				/// It is useful for synchronization, resource cleanup, or preparing for device destruction.
				/// </remarks>
				void Flush();

				/// <summary>
				/// Updates the render target views (RTVs) for the back buffers.
				/// </summary>
				void UpdateRenderTargetViews();

				/// <summary>
				/// Resizes the depth buffer.
				/// </summary>
				/// <param name="a_Size">The new size of the depth buffer.</param>
				void ResizeDepthBuffer(const glm::ivec2& a_Size);

				/// <summary>
				/// Retrieves the current back buffer.
				/// </summary>
				/// <returns>ComPtr to the current back buffer.</returns>
				Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;

				/// <summary>
				/// Retrieves the current back buffer index.
				/// </summary>
				/// <returns>Integer representing the back buffer index.</returns>
				UINT GetCurrentBackBufferIndex() const;

				/// <summary>
				/// Retrieves the current render target view.
				/// </summary>
				/// <returns>Handle to the render target view.</returns>
				D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView(bool a_UseRenderTexture = false);
			public:
				/// <summary>
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Retrieves the DSV heap.
				/// </summary>
				/// <returns>Reference to the DSV heap allocation.</returns>
				HeapAllocation& GetDSV() { return m_DSV; };

				/// <summary>
				/// Retrieves the RTV heap.
				/// </summary>
				/// <returns>Reference to the RTV heap allocation.</returns>
				HeapAllocation& GetRTV() { return m_RTV; };

				/// <summary>
				/// Retrieves the SRV heap.
				/// </summary>
				/// <returns>Reference to the SRV heap allocation.</returns>
				HeapAllocation& GetSRV() { return m_SRV; };

				/// <summary>
				/// Retrieves the resource atlas that contains all materials, textures, meshes and shaders.
				/// </summary>
				/// <returns>Reference to the resource atlas.</returns>
				ResourceAtlas& GetResourceAtlas() { return m_ResourceAtlas; };

				/// <summary>
				/// Retrieves the DirectX 12 device.
				/// </summary>
				/// <returns>ComPtr to the ID3D12Device2.</returns>
				Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const { return m_Device; };

				/// <summary>
				/// Retrieves the DirectX 12 root signature.
				/// </summary>
				/// <returns>ComPtr to the ID3D12RootSignature.</returns>
				Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return m_RootSignature; };

				/// <summary>
				/// Retrieves a command queue of a certain type.
				/// </summary>
				/// <param name="a_Type">The type of command queue such as direct, copy, etc.</param>
				/// <returns>Reference to the CommandQueue.</returns>
				std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_Type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
			protected:
				ResourceAtlas m_ResourceAtlas;

				Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
				Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;

				std::shared_ptr<CommandQueue> m_DirectCommandQueue;
				std::shared_ptr<CommandQueue> m_ComputeCommandQueue;
				std::shared_ptr<CommandQueue> m_CopyCommandQueue;

				uint64_t m_FenceValues[g_BufferCount] = {};

				Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthBuffer;

				HeapAllocation
					m_DSV,
					m_SRV,
					m_RTV;

				Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

				D3D12_VIEWPORT m_Viewport;
				D3D12_RECT m_ScissorRect;

				Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
				Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[g_BufferCount];

				UINT m_RTVDescriptorSize;
				UINT m_CurrentBackBufferIndex;

				RECT m_WindowRect;
				bool m_IsTearingSupported;

				glm::ivec2 m_Size;

				FPSCounter m_FpsCounter;

#ifdef __EDITOR__
				editor::imgui::ImGuiWindow m_ImGuiWindow;
				class Texture* m_RenderTexture = nullptr;
#endif // __EDITOR__

				// TODO: Delete these as they are all supposed to be temporary.

				Camera m_Camera1;
				Camera* m_CurrentCamera = nullptr;

				Transform m_ChickenTransform1;
				Transform m_ChickenTransform2;
				Transform m_FaucetTransform;

				MeshComponent m_ChickenMesh;
				MeshComponent m_ChickenMesh2;
				MeshComponent m_FaucetMesh;

				Microsoft::WRL::ComPtr<ID3D12Resource> m_DirectionalLightBuffer;
				DirectionalLight m_DirectionalLight;
			};
		}
	}
}