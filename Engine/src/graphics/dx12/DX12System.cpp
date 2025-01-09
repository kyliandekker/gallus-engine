#include "graphics/dx12/DX12System.h"

#include "core/logger/Logger.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) :
				m_FenceValue(0),
				m_CommandListType(type),
				m_d3d12Device(device)
			{
				D3D12_COMMAND_QUEUE_DESC desc = {};
				desc.Type = type;
				desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				desc.NodeMask = 0;

				if (FAILED(m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating command queue.");
					return;
				}
				if (FAILED(m_d3d12Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating fence.");
					return;
				}

				m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
				if (!m_FenceEvent)
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating fence event.");
					return;
				}
			}

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
			{
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;

				if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue))
				{
					commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;
					m_CommandAllocatorQueue.pop();

					if (FAILED(commandAllocator->Reset()))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed resetting command allocator.");
						return nullptr;
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

					if (FAILED((commandList->Reset(commandAllocator.Get(), nullptr))))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed resetting command list.");
						return nullptr;
					}
				}
				else
				{
					commandList = CreateCommandList(commandAllocator);
				}

				// Associate the command allocator with the command list so that it can be
				// retrieved when the command list is executed.
				if (FAILED(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get())))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed associating command allocator with command list.");
					return nullptr;
				}

				return commandList;
			}

			uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
			{
				commandList->Close();

				ID3D12CommandAllocator* commandAllocator;
				UINT dataSize = sizeof(commandAllocator);
				if (FAILED(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed setting private data for command list.");
					return -1;
				}

				ID3D12CommandList* const ppCommandLists[] = {
					commandList.Get()
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
				if (FAILED(m_d3d12Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating command allocator.");
					return nullptr;
				}

				return commandAllocator;
			}

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
			{
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;
				if (FAILED(m_d3d12Device->CreateCommandList(0, m_CommandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating command list.");
					return nullptr;
				}

				return commandList;
			}

			double FPSCounter::GetFPS() const
			{
				return m_FPS;
			}

			void FPSCounter::Update()
			{
				m_FrameCounter++;
				auto t1 = m_Clock.now();
				auto deltaTime = t1 - m_T0;
				m_T0 = t1;

				m_ElapsedSeconds += deltaTime.count() * 1e-9;
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
				m_T0 = m_Clock.now();
			}

			bool DX12System::Initialize(HWND a_hWnd, const glm::ivec2 a_Size)
			{
				m_hWnd = a_hWnd;
				m_Size = a_Size;

				LOG(LOGSEVERITY_INFO, CATEGORY_DX12, "Initializing dx12 system.");
				return ThreadedSystem::Initialize();
			}

			void DX12System::Loop()
			{
				m_FpsCounter.Update();

				auto commandList = m_DirectCommandQueue.GetCommandList();
				auto backBuffer = g_BackBuffers[g_CurrentBackBufferIndex];
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
					g_CurrentBackBufferIndex, g_RTVDescriptorSize);

				// Clear the render targets.
				{
					TransitionResource(commandList, backBuffer,
						D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

					FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

					ClearRTV(commandList, rtv, clearColor);
				}

				{
					CD3DX12_CPU_DESCRIPTOR_HANDLE rtvRenderTex = CD3DX12_CPU_DESCRIPTOR_HANDLE(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
						3, g_RTVDescriptorSize);
					FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black color
					ClearRTV(commandList, rtvRenderTex, clearColor);
				}

				// Set the render target.
				commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

				// Set descriptor heaps.
				ID3D12DescriptorHeap* descriptorHeaps[] = { g_SRVDescriptorHeap.Get() };
				commandList->SetDescriptorHeaps(1, descriptorHeaps);

				// Present
				{
					TransitionResource(commandList, backBuffer,
						D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

					g_FrameFenceValues[g_CurrentBackBufferIndex] = m_DirectCommandQueue.ExecuteCommandList(commandList);

					UINT syncInterval = g_VSync ? 1 : 0;
					UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
					if (FAILED(g_SwapChain->Present(syncInterval, presentFlags)))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed presenting.");
					}
					g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

					m_DirectCommandQueue.WaitForFenceValue(g_FrameFenceValues[g_CurrentBackBufferIndex]);
				}
			}

			bool DX12System::Destroy()
			{
				LOG(LOGSEVERITY_INFO, CATEGORY_DX12, "Destroying dx12 system.");
				return ThreadedSystem::Destroy();
			}

			bool DX12System::InitializeThread()
			{
				EnableDebugLayer();
				g_TearingSupported = CheckTearingSupport();

				if (!GetAdapter(g_UseWarp))
				{
					return false;
				}

				if (!CreateDevice(g_DxgiAdapter4))
				{
					return false;
				}

				m_DirectCommandQueue = CommandQueue(g_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);

				if (!CreateSwapChain(m_hWnd, m_DirectCommandQueue.GetD3D12CommandQueue(), m_Size.x, m_Size.y, g_NumSwapChainBuffers))
				{
					return false;
				}

				g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

				if (!CreateDescriptorHeap(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, g_NumSwapChainBuffers + m_ExtraRenderTargets, g_RTVDescriptorHeap))
				{
					return false;
				}
				g_RTVDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_NumSwapChainBuffers);

				if (!CreateDescriptorHeap(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, g_NumSrvBuffers + m_ExtraRenderTargets, g_SRVDescriptorHeap))
				{
					return false;
				}
				UpdateRenderTargetViews(g_Device, g_SwapChain, g_SRVDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_NumSrvBuffers);
				g_SRVDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (!CreateFence(g_Device))
				{
					return false;
				}
				if (!CreateEventHandle())
				{
					return false;
				}

				if (!CreateOffscreenRenderTarget(1920, 1080))
				{
					return false;
				}

				m_FpsCounter.Initialize();

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_DX12, "Initialized dx12 system.");

				return ThreadedSystem::InitializeThread();
			}

			void DX12System::Finalize()
			{
				// Make sure the command queue has finished all commands before closing.
				m_DirectCommandQueue.Flush();
				CleanUpRenderTargets(g_SwapChain);

				ThreadedSystem::Finalize();

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_DX12, "Destroyed dx12 system.");
			}

			void DX12System::EnableDebugLayer()
			{
#if defined(_DEBUG)
				// Always enable the debug layer before doing anything DX12 related
				// so all possible errors generated while creating DX12 objects
				// are caught by the debug layer.
				Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
				if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed retrieving debug interface.");
					return;
				}
				debugInterface->EnableDebugLayer();
#endif
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
						if (FAILED(factory5->CheckFeatureSupport(
							DXGI_FEATURE_PRESENT_ALLOW_TEARING,
							&allowTearing, sizeof(allowTearing))))
						{
							allowTearing = FALSE;
						}
					}
				}

				return allowTearing == TRUE;
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
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating DXGIFactory2.");
					return false;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created DXGIFactory2.");

				Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;

				if (a_UseWarp)
				{
					LOG(LOGSEVERITY_INFO, CATEGORY_DX12, "Using WARP.");
					if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1))))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating WARP adapter.");
						return false;
					}
					LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created WARP adapter.");
					if (FAILED(dxgiAdapter1.As(&g_DxgiAdapter4)))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed casting WARP adapter.");
						return false;
					}
					LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Casted WARP adapter.");
				}
				else
				{
					SIZE_T maxDedicatedVideoMemory = 0;
					for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
					{
						DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
						dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

						// Check to see if the adapter can create a D3D12 device without actually 
						// creating it. The adapter with the largest dedicated video memory
						// is favored.
						if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
							SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
								D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
							dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
						{
							maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
							if (FAILED(dxgiAdapter1.As(&g_DxgiAdapter4)))
							{
								LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating adapter.");
								return false;
							}
							LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created adapter.");
						}
					}
				}

				return true;
			}

			bool DX12System::CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> a_Adapter)
			{
				if (FAILED(D3D12CreateDevice(a_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_Device))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating D3D12 device.");
					return false;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created D3D12 device.");

				// Enable debug messages in debug mode.
#if defined(_DEBUG)
				Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
				if (SUCCEEDED(g_Device.As(&pInfoQueue)))
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
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed pushing storage filter to info queue on D3D12 device.");
						return false;
					}
				}
#endif
				return true;
			}

			bool DX12System::CreateSwapChain(HWND hWnd, Microsoft::WRL::ComPtr<ID3D12CommandQueue> a_CommandQueue, uint32_t a_Width, uint32_t a_Height, uint32_t a_BufferCount)
			{
				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;
				UINT createFactoryFlags = 0;
#if defined(_DEBUG)
				createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
				if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}

				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = a_Width;
				swapChainDesc.Height = a_Height;
				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.Stereo = FALSE;
				swapChainDesc.SampleDesc = { 1, 0 };
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = a_BufferCount;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				// It is recommended to always allow tearing if tearing support is available.
				swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

				Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
				if (FAILED(dxgiFactory4->CreateSwapChainForHwnd(
					a_CommandQueue.Get(),
					hWnd,
					&swapChainDesc,
					nullptr,
					nullptr,
					&swapChain1)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating swap chain for hwnd.");
					return false;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created swap chain for hwnd.");

				// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
				// will be handled manually.
				if (FAILED(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed associating hwnd with swapchain.");
					return false;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Associated hwnd with swapchain.");

				if ((swapChain1.As(&g_SwapChain)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed casting to swapchain.");
					return false;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Casted swapchain.");

				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created swap chain.");
				return true;
			}

			bool DX12System::CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, D3D12_DESCRIPTOR_HEAP_TYPE a_Type, D3D12_DESCRIPTOR_HEAP_FLAGS a_Flags, uint32_t a_NumDescriptors, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& a_DescriptorHeap)
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				desc.NumDescriptors = a_NumDescriptors;
				desc.Type = a_Type;
				desc.Flags = a_Flags;

				if (FAILED(a_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&a_DescriptorHeap))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating descriptor heap.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created descriptor heap.");
				return true;
			}

			void DX12System::UpdateRenderTargetViews(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, Microsoft::WRL::ComPtr<IDXGISwapChain4> a_SwapChain, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> a_DescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE a_Type, uint32_t a_NumDescriptors)
			{
				auto rtvDescriptorSize = a_Device->GetDescriptorHandleIncrementSize(a_Type);

				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(a_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

				for (uint32_t i = 0; i < a_NumDescriptors; ++i)
				{
					if (a_Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
					{
						ID3D12Resource* backBuffer = nullptr;
						if (FAILED(a_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
						{
							LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed retrieving buffer from swap chain.");
							return;
						}

						a_Device->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);

						g_BackBuffers[i] = backBuffer;
					}

					rtvHandle.Offset(rtvDescriptorSize);
				}
			}

			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> DX12System::CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, D3D12_COMMAND_LIST_TYPE a_Type)
			{
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				if (FAILED(a_Device->CreateCommandAllocator(a_Type, IID_PPV_ARGS(&commandAllocator))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating command allocator.");
					return nullptr;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created command allocator.");

				return commandAllocator;
			}

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DX12System::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> a_CommandAllocator, D3D12_COMMAND_LIST_TYPE a_Type)
			{
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
				if (FAILED(a_Device->CreateCommandList(0, a_Type, a_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating command list.");
					return nullptr;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created command list.");

				if (FAILED(commandList->Close()))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed closing command list.");
					return nullptr;
				}
				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created command list.");

				return commandList;
			}

			bool DX12System::CreateFence(Microsoft::WRL::ComPtr<ID3D12Device2> a_Device)
			{
				if (FAILED(a_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_Fence))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating fence.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created fence.");
				return true;
			}

			bool DX12System::CreateEventHandle()
			{
				g_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
				if (!g_FenceEvent)
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating fence event.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, CATEGORY_DX12, "Created fence event.");
				return true;
			}

			void DX12System::Wait()
			{
				m_DirectCommandQueue.Signal();
				m_DirectCommandQueue.WaitForFenceValue(g_FrameFenceValues[g_CurrentBackBufferIndex]);
				m_Ready = false;
			}

			void DX12System::StopWaiting()
			{
				m_Ready = true;
			}

			void DX12System::Resize(const glm::ivec2& a_Size)
			{
				if (m_Size != a_Size)
				{
					Wait();

					Flush();
					for (int i = 0; i < g_NumSwapChainBuffers; ++i)
					{
						g_BackBuffers[i]->Release();
						g_BackBuffers[i] = nullptr;
					}

					DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
					if (FAILED(g_SwapChain->GetDesc(&swapChainDesc)))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed getting desc from swap chain.");
						return;
					}
					if (FAILED(g_SwapChain->ResizeBuffers(g_NumSwapChainBuffers, a_Size.x,
						a_Size.y, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed resizing buffers.");
						return;
					}

					g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

					UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_NumSwapChainBuffers);

					StopWaiting();

					m_Size = glm::ivec2(a_Size.x, a_Size.y);
				}
			}

			void DX12System::Reset()
			{
				m_DirectCommandQueue.Signal();
				m_DirectCommandQueue.WaitForFenceValue(g_FrameFenceValues[g_CurrentBackBufferIndex]);
				m_Ready = false;

				Flush();
				for (int i = 0; i < g_NumSwapChainBuffers; ++i)
				{
					g_BackBuffers[i]->Release();
					g_BackBuffers[i] = nullptr;
				}

				DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
				if (FAILED(g_SwapChain->GetDesc(&swapChainDesc)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed getting desc from swap chain.");
					return;
				}

				g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

				UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_NumSwapChainBuffers);

				m_Ready = true;
			}

			Microsoft::WRL::ComPtr<ID3D12Device2> DX12System::GetDevice() const
			{
				return g_Device;
			}

			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DX12System::GetSRVDescriptorHeap() const
			{
				return g_SRVDescriptorHeap;
			}

			FPSCounter DX12System::GetFPS() const
			{
				return m_FpsCounter;
			}

			void DX12System::CleanUpRenderTargets(Microsoft::WRL::ComPtr<IDXGISwapChain4> a_SwapChain)
			{
				for (int i = 0; i < g_NumSwapChainBuffers; ++i)
				{
					if (g_BackBuffers[i])
					{
						g_BackBuffers[i]->Release();
					}
				}
			}

			void DX12System::Flush()
			{
				m_DirectCommandQueue.Flush();
			}

			bool DX12System::CreateOffscreenRenderTarget(uint32_t a_Width, uint32_t a_Height)
			{
				// Describe and create the render target texture
				D3D12_RESOURCE_DESC textureDesc = {};
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				textureDesc.Width = a_Width;
				textureDesc.Height = a_Height;
				textureDesc.DepthOrArraySize = 1;
				textureDesc.MipLevels = 1;
				textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				D3D12_CLEAR_VALUE clearValue = {};
				clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				clearValue.Color[0] = 0.0f;
				clearValue.Color[1] = 0.0f;
				clearValue.Color[2] = 0.0f;
				clearValue.Color[3] = 1.0f;

				auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				// Create the render target resource
				HRESULT hr = g_Device->CreateCommittedResource(
					&properties,
					D3D12_HEAP_FLAG_NONE,
					&textureDesc,
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					&clearValue,
					IID_PPV_ARGS(&m_RenderTargetTexture)
				);
				if (FAILED(hr))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed to create offscreen render target.");
					return false;
				}

				// Create the RTV for the texture
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
				rtvHandle.Offset(3, g_RTVDescriptorSize); // Offset for the RTV (use a valid index for your RTVs)

				g_Device->CreateRenderTargetView(m_RenderTargetTexture.Get(), nullptr, rtvHandle);

				UINT handleIncrement = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				int descriptorIndex = 1;
				m_RenderTargetSrvHandleCPU = g_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				m_RenderTargetSrvHandleCPU.ptr += (handleIncrement * descriptorIndex);
				m_RenderTargetSrvHandleGPU = g_SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				m_RenderTargetSrvHandleGPU.ptr += (handleIncrement * descriptorIndex);

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				g_Device->CreateShaderResourceView(m_RenderTargetTexture.Get(), &srvDesc, m_RenderTargetSrvHandleCPU);

				return true;
			}

			// Clear a render target.
			void DX12System::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList,
				D3D12_CPU_DESCRIPTOR_HANDLE a_Rtv, FLOAT* a_ClearColor)
			{
				a_CommandList->ClearRenderTargetView(a_Rtv, a_ClearColor, 0, nullptr);
			}

			void DX12System::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList,
				D3D12_CPU_DESCRIPTOR_HANDLE a_Dsv, FLOAT a_Depth)
			{
				a_CommandList->ClearDepthStencilView(a_Dsv, D3D12_CLEAR_FLAG_DEPTH, a_Depth, 0, 0, nullptr);
			}

			void DX12System::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList,
				Microsoft::WRL::ComPtr<ID3D12Resource> a_Resource,
				D3D12_RESOURCE_STATES a_BeforeState, D3D12_RESOURCE_STATES a_AfterState)
			{
				CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					a_Resource.Get(),
					a_BeforeState, a_AfterState);

				a_CommandList->ResourceBarrier(1, &barrier);
			}
		}
	}
}