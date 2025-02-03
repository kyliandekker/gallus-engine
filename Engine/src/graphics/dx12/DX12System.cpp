#include <dx12/directx/d3dx12.h>

#include "graphics/dx12/DX12System.h"

#include "core/logger/Logger.h"
#include "core/DataStream.h"
#include "core/FileUtils.h"
#include "core/Engine.h"
#include "graphics/dx12/Material.h" // TODO: remove this one when we load assets from scenes.
#include "graphics/dx12/Texture.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
#pragma region DX12_FPS

			double FPSCounter::GetFPS() const
			{
				return m_FPS;
			}

			double FPSCounter::GetTotalTime() const
			{
				return m_TotalTime;
			}

			void FPSCounter::Update()
			{
				m_FrameCounter++;
				auto t1 = m_Clock.now();
				auto deltaTime = t1 - m_T0;
				m_T0 = t1;

				m_ElapsedSeconds += deltaTime.count() * 1e-9;
				m_TotalTime += deltaTime.count() * 1e-9;
				if (m_ElapsedSeconds > 1.0)
				{
					m_FPS = m_FrameCounter / m_ElapsedSeconds;
					m_FrameCounter = 0;
					m_ElapsedSeconds = 0.0;
				}
			}

			void FPSCounter::Initialize()
			{
				m_FPS = 0.0;
				m_FrameCounter = 0;
				m_ElapsedSeconds = 0.0;
				m_TotalTime = 0.0;
				m_T0 = m_Clock.now();
			}

#pragma endregion DX12_FPS

#pragma region DX12_SYSTEM

			bool DX12System::Initialize(bool a_Wait, HWND a_hWnd, const glm::ivec2& a_Size)
			{
				m_Size = a_Size;

				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Initializing dx12 system.");
				return ThreadedSystem::Initialize(a_Wait);
			}

			bool DX12System::Destroy()
			{
				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Destroying dx12 system.");
				return ThreadedSystem::Destroy();
			}

			bool DX12System::InitializeThread()
			{
#if defined(_DEBUG)
				// Always enable the debug layer before doing anything DX12 related
				// so all possible errors generated while creating DX12 objects
				// are caught by the debug layer.
				Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
				if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving debug interface.");
					return false;
				}
				debugInterface->EnableDebugLayer();
				debugInterface->Release();

				Microsoft::WRL::ComPtr<ID3D12Debug1> debugController1 = nullptr;
				if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController1))))
				{
					debugController1->SetEnableGPUBasedValidation(TRUE);
					debugController1->Release();
				}
#endif
				// Get the adapter.
				if (!GetAdapter(false))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving adapter.");
					return false;
				}

				// Create the device.
				if (!CreateDevice())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating m_Device.");
					return false;
				}

				ID3D12InfoQueue* infoQueue = nullptr;
				if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
				{
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);

					// Optionally filter out less important messages
					D3D12_MESSAGE_ID denyIds[] = {
						D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
						D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
					};

					D3D12_INFO_QUEUE_FILTER filter = {};
					filter.DenyList.NumIDs = _countof(denyIds);
					filter.DenyList.pIDList = denyIds;
					infoQueue->PushStorageFilter(&filter);

					infoQueue->Release();
				}

				core::ENGINE.GetWindow().m_OnResize += std::bind(&DX12System::Resize, this, std::placeholders::_1, std::placeholders::_2);

				// Create the command queues.
				m_DirectCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
				m_ComputeCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
				m_CopyCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);

				m_IsTearingSupported = CheckTearingSupport();

				m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));

				if (!DirectX::XMVerifyCPUSupport())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed verifying DirectX Math library support.");
					return false;
				}

				// Create the swap chain.
				if (!CreateSwapChain())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}

				// Get the copy command queue.
				std::shared_ptr<CommandQueue> cCommandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				std::shared_ptr<CommandList> cCommandList = cCommandQueue->GetCommandList();

				CreateRTV();
				CreateDSV();
				CreateSRV();

#ifdef __EDITOR__
				CreateRenderTexture();
				m_ImGuiWindow.Initialize();
#endif // __EDITOR__

				UpdateRenderTargetViews();
#ifdef __EDITOR__
				m_ImGuiWindow.m_PreviewTexture = &m_ResourceAtlas.LoadTexture("tex_missing.png", cCommandList); // Default texture.
				m_ImGuiWindow.OnRenderTargetCreated();
#endif // __EDITOR__

				if (!CreateRootSignature())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating root signature.");
					return false;
				}

				// Default textures, meshes, shaders and materials.
				Shader& shaderColor = m_ResourceAtlas.LoadShader("color"); // Default color shader.
				Shader& shaderAlbedo = m_ResourceAtlas.LoadShader("albedo"); // Default albedo shader.
				m_ResourceAtlas.LoadTexture("tex_missing.png", cCommandList); // Default texture.
				m_ResourceAtlas.LoadMaterial("default", { { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f }); // Default material.

				Material& m_FaucetMaterial = m_ResourceAtlas.LoadMaterial("faucet", { { 0.16f, 0.16f, 0.16f }, 0.87f, 1.0f });

				m_ChickenMesh.Initialize();
				m_ChickenMesh2.Initialize();
				m_FaucetMesh.Initialize();

				m_ChickenMesh.SetMesh(m_ResourceAtlas.LoadMesh("chicken.gltf", cCommandList));
				m_ChickenMesh.SetTexture(m_ResourceAtlas.LoadTexture("tex_chicken_normal.png", cCommandList));
				m_ChickenMesh.SetShader(shaderAlbedo);

				m_ChickenMesh2.SetMesh(m_ResourceAtlas.LoadMesh("chicken.gltf", cCommandList));
				m_ChickenMesh2.SetTexture(m_ResourceAtlas.LoadTexture("tex_chicken_sick.png", cCommandList));
				m_ChickenMesh2.SetShader(shaderAlbedo);

				m_FaucetMesh.SetMesh(m_ResourceAtlas.LoadMesh("mod_faucet.gltf", cCommandList));
				m_FaucetMesh.SetMaterial(m_FaucetMaterial);
				m_FaucetMesh.SetShader(shaderColor);

				auto fenceValue = cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->WaitForFenceValue(fenceValue);

				std::shared_ptr<CommandQueue> dCommandQueue = GetCommandQueue();
				std::shared_ptr<CommandList> dCommandList = dCommandQueue->GetCommandList();

				m_ResourceAtlas.TransitionResources(dCommandList);
				dCommandQueue->ExecuteCommandList(dCommandList);
				dCommandQueue->WaitForFenceValue(fenceValue);

				// Resize/Create the depth buffer.
				ResizeDepthBuffer(m_Size);

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Initialized dx12 system.");










				// Define a default light direction (pointing downward)
				m_DirectionalLight = { DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), 0.0f, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 0.5f };

				size_t bufferSize = sizeof(DirectionalLight);

				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

				if (FAILED(m_Device->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&bufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_DirectionalLightBuffer))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create DirectionalLight buffer.");
					return false;
				}

				// Copy light data to GPU buffer
				void* mappedData;
				CD3DX12_RANGE readRange(0, 0);
				m_DirectionalLightBuffer->Map(0, &readRange, &mappedData);
				memcpy(mappedData, &m_DirectionalLight, bufferSize);
				m_DirectionalLightBuffer->Unmap(0, nullptr);




				m_FpsCounter.Initialize();

				return ThreadedSystem::InitializeThread();
			}

			bool DX12System::GetAdapter(bool a_UseWarp)
			{
				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
				UINT createFactoryFlags = 0;
#if defined(_DEBUG)
				createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

				if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating DXGI Factory.");
					return false;
				}

				Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;

				if (a_UseWarp)
				{
					if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating warp adapter.");
						return false;
					}
					if (FAILED(dxgiAdapter1.As(&m_Adapter)))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting adapter.");
						return false;
					}
				}
				else
				{
					SIZE_T maxDedicatedVideoMemory = 0;
					for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
					{
						DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
						dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

						// Check to see if the adapter can create a D3D12 m_Device without actually 
						// creating it. The adapter with the largest dedicated video memory
						// is favored.
						if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
							SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
							D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
							dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
						{
							maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
							if ((dxgiAdapter1.As(&m_Adapter)))
							{
								LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting adapter.");
								return false;
							}
						}
					}
				}

				return true;
			}

			bool DX12System::CreateDevice()
			{
				if (FAILED(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating m_Device.");
					return false;
				}
				//    NAME_D3D12_OBJECT(d3d12Device2);

					// Enable debug messages in debug mode.
#if defined(_DEBUG)
				Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
				if (SUCCEEDED(m_Device.As(&pInfoQueue)))
				{
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

					// Suppress whole categories of messages
					//D3D12_MESSAGE_CATEGORY Categories[] = {};

					// Suppress messages based on their severity level
					D3D12_MESSAGE_SEVERITY Severities[] =
					{
						D3D12_MESSAGE_SEVERITY_INFO
					};

					// Suppress individual messages by their ID
					D3D12_MESSAGE_ID DenyIds[] = {
						D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
						D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
						D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
					};

					D3D12_INFO_QUEUE_FILTER NewFilter = {};
					//NewFilter.DenyList.NumCategories = _countof(Categories);
					//NewFilter.DenyList.pCategoryList = Categories;
					NewFilter.DenyList.NumSeverities = _countof(Severities);
					NewFilter.DenyList.pSeverityList = Severities;
					NewFilter.DenyList.NumIDs = _countof(DenyIds);
					NewFilter.DenyList.pIDList = DenyIds;

					if (FAILED(pInfoQueue->PushStorageFilter(&NewFilter)))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed pushing storage filter.");
						return false;
					}
				}
#endif

				return true;
			}

			bool DX12System::CheckTearingSupport()
			{
				BOOL allowTearing = FALSE;

				// Rather than create the DXGI 1.5 factory interface directly, we create the
				// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
				// graphics debugging tools which will not support the 1.5 factory interface 
				// until a future update.
				Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
				if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
				{
					Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
					if (SUCCEEDED(factory4.As(&factory5)))
					{
						factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
							&allowTearing, sizeof(allowTearing));
					}
				}

				return allowTearing == TRUE;
			}

			bool DX12System::CreateSwapChain()
			{
				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;
				UINT createFactoryFlags = 0;
#if defined(_DEBUG)
				createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

				if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating DXGI factory.");
					return false;
				}

				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = m_Size.x;
				swapChainDesc.Height = m_Size.y;
				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.Stereo = FALSE;
				swapChainDesc.SampleDesc = { 1, 0 };
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = g_BufferCount;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				// It is recommended to always allow tearing if tearing support is available.
				swapChainDesc.Flags = m_IsTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
				ID3D12CommandQueue* pCommandQueue = GetCommandQueue()->GetCommandQueue().Get();

				Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
				if (FAILED(dxgiFactory4->CreateSwapChainForHwnd(
					pCommandQueue,
					core::ENGINE.GetWindow().GetHWnd(),
					&swapChainDesc,
					nullptr,
					nullptr,
					&swapChain1)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}

				// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
				// will be handled manually.
				if (FAILED(dxgiFactory4->MakeWindowAssociation(core::ENGINE.GetWindow().GetHWnd(), DXGI_MWA_NO_ALT_ENTER)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed associating window.");
					return false;
				}

				if (FAILED(swapChain1.As(&m_SwapChain)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting swapchain.");
					return false;
				}

				m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

				return m_SwapChain;
			}

			void DX12System::CreateRTV()
			{
				size_t numBuffers = g_BufferCount;
#ifdef __EDITOR__
				numBuffers += 1;
#endif //__EDITOR__
				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
				rtvHeapDesc.NumDescriptors = numBuffers;
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				m_RTV = HeapAllocation(rtvHeapDesc);

				m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}

			void DX12System::CreateDSV()
			{
				// Create the descriptor heap for the depth-stencil view.
				D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
				dsvHeapDesc.NumDescriptors = 1;
				dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				m_DSV = HeapAllocation(dsvHeapDesc);
			}

			void DX12System::CreateSRV()
			{
				D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
				srvHeapDesc.NumDescriptors = 100;  // Adjust based on how many textures you need
				srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Important for binding!
				m_SRV = HeapAllocation(srvHeapDesc);
			}

#ifdef __EDITOR__
			void DX12System::CreateRenderTexture()
			{
				D3D12_RESOURCE_DESC texDesc = {};
				texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				texDesc.Width = m_Size.x;
				texDesc.Height = m_Size.y;
				texDesc.DepthOrArraySize = 1;
				texDesc.MipLevels = 1;
				texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				texDesc.SampleDesc.Count = 1;
				texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				m_RenderTexture = &m_ResourceAtlas.LoadTextureByDescription("RenderTexture", texDesc);
			}
#endif // __EDITOR__

			bool DX12System::CreateRootSignature()
			{
				// Define descriptor ranges
				CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[1]{};

				// SRV for the texture at register t0
				descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

				// Define root parameters
				CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters]{};

				// CBV at register b0 (Model-View-Projection Matrix)
				rootParameters[RootParameters::CBV].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

				// Texture SRV at register t0 (binds a texture)
				rootParameters[RootParameters::TEX_SRV].InitAsDescriptorTable(1, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

				rootParameters[RootParameters::MATERIAL].InitAsConstantBufferView(1);
				rootParameters[RootParameters::LIGHT].InitAsConstantBufferView(2);

				// Define static sampler at register s0 (replaces the removed descriptor table sampler)
				CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] = {
					CD3DX12_STATIC_SAMPLER_DESC(
						0,  // Register s0
						D3D12_FILTER_MIN_MAG_MIP_LINEAR,  // Bilinear filtering
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP
					)
				};

				// Define root signature flags
				D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

				// Create root signature descriptor
				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
				rootSignatureDescription.Init_1_1(
					_countof(rootParameters), rootParameters,
					_countof(staticSamplers), staticSamplers, // Use static samplers
					rootSignatureFlags
				);

				// Serialize the root signature
				Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
				Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
				HRESULT hr = D3DX12SerializeVersionedRootSignature(
					&rootSignatureDescription,
					D3D_ROOT_SIGNATURE_VERSION_1_1,
					&rootSignatureBlob,
					&errorBlob
				);

				if (FAILED(hr))
				{
					std::string errorMessage(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed serializing root signature: \"%s\".", errorMessage.c_str());
					return false;
				}

				// Create the root signature
				if (FAILED(m_Device->CreateRootSignature(0,
					rootSignatureBlob->GetBufferPointer(),
					rootSignatureBlob->GetBufferSize(),
					IID_PPV_ARGS(&m_RootSignature))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating root signature.");
					return false;
				}

				return true;
			}

			void DX12System::Finalize()
			{
#ifdef __EDITOR__
				m_ImGuiWindow.Destroy();
#endif // __EDITOR__

				Flush();

				ThreadedSystem::Finalize();

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Destroyed dx12 system.");
			}

			void DX12System::Flush()
			{
				m_DirectCommandQueue->Flush();
				m_ComputeCommandQueue->Flush();
				m_CopyCommandQueue->Flush();
			}

			void DX12System::UpdateRenderTargetViews()
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTV.GetCPUDescriptorHandleForHeapStart());

#ifdef __EDITOR__
				// Create RTV for custom render target texture
				if (m_RenderTexture->IsValid())
				{
					m_Device->CreateRenderTargetView(m_RenderTexture->GetResource().Get(), nullptr, rtvHandle);
				}
				rtvHandle.Offset(m_RTVDescriptorSize);
#endif // __EDITOR__

				for (int i = 0; i < g_BufferCount; ++i)
				{
					Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
					if (FAILED(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving buffer.");
						return;
					}

					m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

					m_BackBuffers[i] = backBuffer;

					rtvHandle.Offset(m_RTVDescriptorSize);
				}
			}

			void DX12System::ResizeDepthBuffer(const glm::ivec2& a_Size)
			{
				// Flush any GPU commands that might be referencing the depth buffer.
				Flush();

				m_Size = a_Size;

				auto m_Device = GetDevice();

				// Resize screen dependent resources.
				// Create a depth buffer.
				D3D12_CLEAR_VALUE optimizedClearValue = {};
				optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
				optimizedClearValue.DepthStencil = { 1.0f, 0 };

				CD3DX12_HEAP_PROPERTIES heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Size.x, m_Size.y,
					1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
				if (FAILED(m_Device->CreateCommittedResource(
					&heapType,
					D3D12_HEAP_FLAG_NONE,
					&tex,
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					&optimizedClearValue,
					IID_PPV_ARGS(&m_DepthBuffer)
					)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource.");
					return;
				}

				// Update the depth-stencil view.
				D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
				dsv.Format = DXGI_FORMAT_D32_FLOAT;
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsv.Texture2D.MipSlice = 0;
				dsv.Flags = D3D12_DSV_FLAG_NONE;

				m_Device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv,
					m_DSV.GetCPUDescriptorHandleForHeapStart());
			}

			void DX12System::Resize(const glm::ivec2& a_Pos, const glm::ivec2& a_Size)
			{
//				std::lock_guard<std::mutex> lock(m_RenderMutex);
//
//				Flush();
//				for (int i = 0; i < g_BufferCount; ++i)
//				{
//					m_BackBuffers[i].Reset();  // This will release the old resource properly
//				}
//#ifdef __EDITOR__
//				m_ImGuiWindow.OnRenderTargetCleaned();
//#endif // __EDITOR__
//
//				DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
//				if (FAILED(m_SwapChain->GetDesc(&swapChainDesc)))
//				{
//					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed getting desc from swap chain.");
//					return;
//				}
//				if (FAILED(m_SwapChain->ResizeBuffers(g_BufferCount, a_Size.x,
//					a_Size.y, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
//				{
//					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resizing buffers.");
//					return;
//				}
//				ResizeDepthBuffer(a_Size);
//
//				m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
//
//				UpdateRenderTargetViews();
//
//#ifdef __EDITOR__
//				m_ImGuiWindow.Resize(a_Pos, a_Size);
//#endif // __EDITOR__
//
//				m_Size = glm::vec2(a_Size.x, a_Size.y);
//				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
			}

			Microsoft::WRL::ComPtr<ID3D12Resource> DX12System::GetCurrentBackBuffer() const
			{
				return m_BackBuffers[m_CurrentBackBufferIndex];
			}

			UINT DX12System::GetCurrentBackBufferIndex() const
			{
				return m_CurrentBackBufferIndex;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE DX12System::GetCurrentRenderTargetView(bool a_UseRenderTexture)
			{
				size_t backBufferStart = 0;
#ifdef __EDITOR__
				if (a_UseRenderTexture && m_RenderTexture->IsValid())
				{
					return m_RTV.GetCPUHandle(0);
				}
				backBufferStart += 1;
#endif // __EDITOR__
				return m_RTV.GetCPUHandle(backBufferStart + m_CurrentBackBufferIndex);
			}

			void DX12System::Loop()
			{
				std::lock_guard<std::mutex> lock(m_RenderMutex);

#ifdef __EDITOR__
				m_ImGuiWindow.Update();
#endif // __EDITOR

				m_FpsCounter.Update();

				//TESTF("FPS: %f", m_FpsCounter.GetFPS());

				m_Camera1.GetTransform().SetPosition({ 0.0f, 0.0f, 0.0f });

				m_CurrentCamera = &m_Camera1;

				const DirectX::XMMATRIX viewMatrix = m_CurrentCamera->GetViewMatrix();
				const DirectX::XMMATRIX& projectionMatrix = m_CurrentCamera->GetProjectionMatrix();

				// Render part.
				std::shared_ptr<CommandQueue> commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
				std::shared_ptr<CommandList> commandList = commandQueue->GetCommandList();

				UINT currentBackBufferIndex = GetCurrentBackBufferIndex();
				auto backBuffer = GetCurrentBackBuffer();
				auto rtv = GetCurrentRenderTargetView(true); // This "true" only does something on __EDITOR__.
				auto dsv = m_DSV.GetCPUDescriptorHandleForHeapStart();

				commandList->TransitionResource(backBuffer,
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

				// On editor, we want to render onto a render texture, so transition the texture to a render target.
#ifdef __EDITOR__
				commandList->TransitionResource(m_RenderTexture->GetResource(),
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_STATE_RENDER_TARGET);
#endif

				commandList->GetCommandList()->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);
				FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

				commandList->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
				commandList->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

				commandList->GetCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());

				commandList->GetCommandList()->RSSetViewports(1, &m_Viewport);
				commandList->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

				commandList->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameters::LIGHT, m_DirectionalLightBuffer->GetGPUVirtualAddress());

				m_FaucetTransform.SetPosition({ -2.0f, 0.0f, 5.0f });
				m_FaucetTransform.GetRotation().y += 0.1f;

				m_FaucetMesh.Render(commandList, m_FaucetTransform, viewMatrix, projectionMatrix);

				m_ChickenTransform1.SetPosition({ 0.0f, 0.0f, 2.0f });
				m_ChickenTransform1.GetRotation().y += 0.1f;

				m_ChickenMesh.Render(commandList, m_ChickenTransform1, viewMatrix, projectionMatrix);

				m_ChickenTransform2.SetPosition({ 1.0f, 1.0f, 5.0f });
				m_ChickenTransform2.GetRotation().y -= 0.1f;

				m_ChickenMesh2.Render(commandList, m_ChickenTransform2, viewMatrix, projectionMatrix);

#ifdef __EDITOR__
				// Transition back to SRV for ImGui usage
				commandList->TransitionResource(m_RenderTexture->GetResource(),
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

				rtv = GetCurrentRenderTargetView(false); // Now get the normal rtv.

				commandList->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
				commandList->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

				m_ImGuiWindow.Render(commandList);
#endif
				// Present
				{
					commandList->TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

					m_FenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

					UINT syncInterval = g_VSync ? 1 : 0;
					UINT presentFlags = m_IsTearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
					if (FAILED(m_SwapChain->Present(syncInterval, presentFlags)))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed presenting.");
						return;
					}
					m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

					commandQueue->WaitForFenceValue(m_FenceValues[m_CurrentBackBufferIndex]);
				}
			}

			std::shared_ptr<CommandQueue> DX12System::GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_Type) const
			{
				std::shared_ptr<CommandQueue> commandQueue = nullptr;
				switch (a_Type)
				{
					case D3D12_COMMAND_LIST_TYPE_DIRECT:
					{
						commandQueue = m_DirectCommandQueue;
						break;
					}
					case D3D12_COMMAND_LIST_TYPE_COMPUTE:
					{
						commandQueue = m_ComputeCommandQueue;
						break;
					}
					case D3D12_COMMAND_LIST_TYPE_COPY:
					{
						commandQueue = m_CopyCommandQueue;
						break;
					}
					default:
					{
						break;
					}
				}

				return commandQueue;
			}

#pragma endregion DX12_SYSTEM
		}
	}
}