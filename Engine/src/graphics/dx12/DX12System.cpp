#include "graphics/dx12/DX12System.h"

#include <d3dcompiler.h>
#include <memory>

#include "core/logger/Logger.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
#pragma region DX12_COMMAND_QUEUE
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

#pragma endregion DX12_COMMAND_QUEUE

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

			struct VertexPosColor
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Color;
			};

			static VertexPosColor g_Vertices[8] = {
				{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
				{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
				{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
				{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
				{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
				{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
				{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
				{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
			};

			static WORD g_Indicies[36] =
			{
				0, 1, 2, 0, 2, 3,
				4, 6, 5, 4, 7, 6,
				4, 5, 1, 4, 1, 0,
				3, 2, 6, 3, 6, 7,
				1, 5, 6, 1, 6, 2,
				4, 0, 3, 4, 3, 7
			};

			bool DX12System::Initialize(bool a_Wait, HWND a_hWnd, const glm::ivec2 a_Size)
			{
				m_hWnd = a_hWnd;
				m_Size = a_Size;

				LOG(LOGSEVERITY_INFO, CATEGORY_DX12, "Initializing dx12 system.");
				return ThreadedSystem::Initialize(a_Wait);
			}

			Microsoft::WRL::ComPtr<ID3D12Resource> DX12System::GetCurrentBackBuffer() const
			{
				return m_d3d12BackBuffers[m_CurrentBackBufferIndex];
			}

			UINT DX12System::GetCurrentBackBufferIndex() const
			{
				return m_CurrentBackBufferIndex;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE DX12System::GetCurrentRenderTargetView() const
			{
				return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_d3d12RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
					m_CurrentBackBufferIndex, m_RTVDescriptorSize);
			}

			void DX12System::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
				Microsoft::WRL::ComPtr<ID3D12Resource> resource,
				D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
			{
				CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					resource.Get(),
					beforeState, afterState);

				commandList->ResourceBarrier(1, &barrier);
			}

			// Clear a render target.
			void DX12System::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
				D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
			{
				commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
			}

			void DX12System::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
				D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
			{
				commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
			}

			void DX12System::Loop()
			{
				m_FpsCounter.Update();

				// Update the model matrix.
				float angle = static_cast<float>(m_FpsCounter.GetTotalTime() * 90.0);
				const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
				m_ModelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

				// Update the view matrix.
				const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
				const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
				const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
				m_ViewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

				// Update the projection matrix.
				float aspectRatio = m_Size.x / static_cast<float>(m_Size.y);
				m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_FoV), aspectRatio, 0.1f, 100.0f);

				auto commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
				auto commandList = commandQueue->GetCommandList();

				UINT currentBackBufferIndex = GetCurrentBackBufferIndex();
				auto backBuffer = GetCurrentBackBuffer();
				auto rtv = GetCurrentRenderTargetView();
				auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

				// Clear the render targets.
				{
					TransitionResource(commandList, backBuffer,
						D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

					FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

					ClearRTV(commandList, rtv, clearColor);
					ClearDepth(commandList, dsv);
				}

				commandList->SetPipelineState(m_PipelineState.Get());
				commandList->SetGraphicsRootSignature(m_RootSignature.Get());

				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
				commandList->IASetIndexBuffer(&m_IndexBufferView);

				commandList->RSSetViewports(1, &m_Viewport);
				commandList->RSSetScissorRects(1, &m_ScissorRect);

				commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

				// Update the MVP matrix
				DirectX::XMMATRIX mvpMatrix = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
				mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
				commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

				commandList->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);

				// Present
				{
					TransitionResource(commandList, backBuffer,
						D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

					m_FenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

					UINT syncInterval = m_VSync ? 1 : 0;
					UINT presentFlags = m_IsTearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
					if (FAILED(m_dxgiSwapChain->Present(syncInterval, presentFlags)))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed presenting.");
						return;
					}
					m_CurrentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

					commandQueue->WaitForFenceValue(m_FenceValues[m_CurrentBackBufferIndex]);
				}
			}

			bool DX12System::Destroy()
			{
				LOG(LOGSEVERITY_INFO, CATEGORY_DX12, "Destroying dx12 system.");
				return ThreadedSystem::Destroy();
			}

			void DX12System::IncreaseFov()
			{
				m_FoV -= 0.1f;

				if (m_FoV > 90)
				{
					m_FoV = 90;
				}
				else if (m_FoV < 12)
				{
					m_FoV = 12;
				}
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
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed retrieving debug interface.");
					return false;
				}
				debugInterface->EnableDebugLayer();
#endif
				if (!GetAdapter(false))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed retrieving adapter.");
					return false;
				}

				if (!CreateDevice())
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating device.");
					return false;
				}

				if (m_d3d12Device)
				{
					m_DirectCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
					m_ComputeCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
					m_CopyCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_COPY);

					m_IsTearingSupported = CheckTearingSupport();
				}

				m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
				m_FoV = 45.0;

				if (!DirectX::XMVerifyCPUSupport())
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed verifying DirectX Math library support.");
					return false;
				}

				if (!CreateSwapChain())
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}

				m_d3d12RTVDescriptorHeap = CreateDescriptorHeap(BufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				m_RTVDescriptorSize = GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

				UpdateRenderTargetViews();

				m_FpsCounter.Initialize();

				Microsoft::WRL::ComPtr<ID3D12Device2> device = GetDevice();
				std::shared_ptr<CommandQueue> commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList = commandQueue->GetCommandList();

				// Upload vertex buffer data.
				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;
				UpdateBufferResource(commandList,
					&m_VertexBuffer, &intermediateVertexBuffer,
					_countof(g_Vertices), sizeof(VertexPosColor), g_Vertices);

				// Create the vertex buffer view.
				m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
				m_VertexBufferView.SizeInBytes = sizeof(g_Vertices);
				m_VertexBufferView.StrideInBytes = sizeof(VertexPosColor);

				// Upload index buffer data.
				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
				UpdateBufferResource(commandList,
					&m_IndexBuffer, &intermediateIndexBuffer,
					_countof(g_Indicies), sizeof(WORD), g_Indicies);

				// Create index buffer view.
				m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
				m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
				m_IndexBufferView.SizeInBytes = sizeof(g_Indicies);

				// Create the descriptor heap for the depth-stencil view.
				D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
				dsvHeapDesc.NumDescriptors = 1;
				dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed to create descriptor heap.");
					return false;
				}

				// Load the vertex shader.
				// Load the vertex shader.
				Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
				Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
				if (D3DCompileFromFile(
					L"./resources/shaders/vertexshader.hlsl",
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					"main",
					"vs_5_1",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
					0,
					&vertexShaderBlob,
					&errorBlob
				))
				{
					if (errorBlob)
					{
						std::string errorMessage(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());

						LOGF(LOGSEVERITY_ERROR, CATEGORY_DX12, "Shader Compilation Error: %s", errorMessage.c_str());
					}
					else
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed loading vertex shader.");
					}
					return false;
				}

				// Load the pixel shader.
				Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
				if (D3DCompileFromFile(
					L"./resources/shaders/pixelshader.hlsl",
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					"main",
					"ps_5_1",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
					0,
					&pixelShaderBlob,
					&errorBlob
				))
				{
					if (errorBlob)
					{
						std::string errorMessage(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());

						LOGF(LOGSEVERITY_ERROR, CATEGORY_DX12, "Shader Compilation Error: %s", errorMessage.c_str());
					}
					else
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed loading pixel shader.");
					}
					return false;
				}

				// Create the vertex input layout
				D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				};

				// Create a root signature.
				D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
				if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
				{
					featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
				}

				// Allow input layout and deny unnecessary access to certain pipeline stages.
				D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

				// A single 32-bit constant root parameter that is used by the vertex shader.
				CD3DX12_ROOT_PARAMETER1 rootParameters[1];
				rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
				rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

				// Serialize the root signature.
				Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
				if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
					featureData.HighestVersion, &rootSignatureBlob, &errorBlob)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed serializing vesioned root signature.");
					return false;
				}
				// Create the root signature.
				if (FAILED(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
					rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating root signature.");
					return false;
				}

				struct PipelineStateStream
				{
					CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
					CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
					CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
					CD3DX12_PIPELINE_STATE_STREAM_VS VS;
					CD3DX12_PIPELINE_STATE_STREAM_PS PS;
					CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
					CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
				} pipelineStateStream;

				D3D12_RT_FORMAT_ARRAY rtvFormats = {};
				rtvFormats.NumRenderTargets = 1;
				rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

				pipelineStateStream.pRootSignature = m_RootSignature.Get();
				pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
				pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
				pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
				pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				pipelineStateStream.RTVFormats = rtvFormats;

				D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
					sizeof(PipelineStateStream), &pipelineStateStream
				};
				if (FAILED(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating pipeline state.");
					return false;
				}

				auto fenceValue = commandQueue->ExecuteCommandList(commandList);
				commandQueue->WaitForFenceValue(fenceValue);

				// Resize/Create the depth buffer.
				ResizeDepthBuffer(m_Size);

				return ThreadedSystem::InitializeThread();
			}

			void DX12System::Finalize()
			{
				ThreadedSystem::Finalize();

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_DX12, "Destroyed dx12 system.");
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
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating DXGI Factory.");
					return false;
				}

				Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;

				if (a_UseWarp)
				{
					if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1))))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating warp adapter.");
						return false;
					}
					if (FAILED(dxgiAdapter1.As(&m_dxgiAdapter)))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed casting adapter.");
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

						// Check to see if the adapter can create a D3D12 device without actually 
						// creating it. The adapter with the largest dedicated video memory
						// is favored.
						if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
							SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
								D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
							dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
						{
							maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
							if ((dxgiAdapter1.As(&m_dxgiAdapter)))
							{
								LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed casting adapter.");
								return false;
							}
						}
					}
				}

				return true;
			}

			bool DX12System::CreateDevice()
			{
				if (FAILED(D3D12CreateDevice(m_dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12Device))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating device.");
					return false;
				}
				//    NAME_D3D12_OBJECT(d3d12Device2);

					// Enable debug messages in debug mode.
#if defined(_DEBUG)
				Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
				if (SUCCEEDED(m_d3d12Device.As(&pInfoQueue)))
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
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed pushing storage filter.");
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

			Microsoft::WRL::ComPtr<ID3D12Device2> DX12System::GetDevice() const
			{
				return m_d3d12Device;
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

			void DX12System::Flush()
			{
				m_DirectCommandQueue->Flush();
				m_ComputeCommandQueue->Flush();
				m_CopyCommandQueue->Flush();
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
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating DXGI factory.");
					return false;
				}

				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = m_Size.x;
				swapChainDesc.Height = m_Size.y;
				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.Stereo = FALSE;
				swapChainDesc.SampleDesc = { 1, 0 };
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = BufferCount;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				// It is recommended to always allow tearing if tearing support is available.
				swapChainDesc.Flags = m_IsTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
				ID3D12CommandQueue* pCommandQueue = GetCommandQueue()->GetD3D12CommandQueue().Get();

				Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
				if (FAILED(dxgiFactory4->CreateSwapChainForHwnd(
					pCommandQueue,
					m_hWnd,
					&swapChainDesc,
					nullptr,
					nullptr,
					&swapChain1)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}

				// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
				// will be handled manually.
				if (FAILED(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed associating window.");
					return false;
				}

				if (FAILED(swapChain1.As(&m_dxgiSwapChain)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed casting swapchain.");
					return false;
				}

				m_CurrentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

				return m_dxgiSwapChain;
			}

			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DX12System::CreateDescriptorHeap(UINT a_NumDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE a_Type)
			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				desc.Type = a_Type;
				desc.NumDescriptors = a_NumDescriptors;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				desc.NodeMask = 0;

				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
				if (FAILED(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating descriptor heap.");
					return nullptr;
				}

				return descriptorHeap;
			}

			UINT DX12System::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE a_Type) const
			{
				return m_d3d12Device->GetDescriptorHandleIncrementSize(a_Type);
			}

			void DX12System::UpdateRenderTargetViews()
			{
				auto device = GetDevice();

				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_d3d12RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

				for (int i = 0; i < BufferCount; ++i)
				{
					Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
					if (FAILED(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed retrieving buffer.");
						return;
					}

					device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

					m_d3d12BackBuffers[i] = backBuffer;

					rtvHandle.Offset(m_RTVDescriptorSize);
				}
			}

			void DX12System::UpdateBufferResource(
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList,
				ID3D12Resource** a_pDestinationResource,
				ID3D12Resource** a_pIntermediateResource,
				size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData,
				D3D12_RESOURCE_FLAGS a_Flags)
			{
				auto device = GetDevice();

				size_t bufferSize = a_NumElements * a_ElementSize;

				CD3DX12_HEAP_PROPERTIES defaultHeaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, a_Flags);
				// Create a committed resource for the GPU resource in a default heap.
				if (FAILED(device->CreateCommittedResource(
					&defaultHeaptype,
					D3D12_HEAP_FLAG_NONE,
					&buffer,
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(a_pDestinationResource))))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating committed resource.");
					return;
				}

				// Create an committed resource for the upload.
				if (a_BufferData)
				{
					CD3DX12_HEAP_PROPERTIES uploadHeaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					CD3DX12_RESOURCE_DESC buff = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
					if (FAILED(device->CreateCommittedResource(
						&uploadHeaptype,
						D3D12_HEAP_FLAG_NONE,
						&buff,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(a_pIntermediateResource))))
					{
						LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating committed resource.");
						return;
					}

					D3D12_SUBRESOURCE_DATA subresourceData = {};
					subresourceData.pData = a_BufferData;
					subresourceData.RowPitch = bufferSize;
					subresourceData.SlicePitch = subresourceData.RowPitch;

					UpdateSubresources(a_CommandList.Get(),
						*a_pDestinationResource, *a_pIntermediateResource,
						0, 0, 1, &subresourceData);
				}
			}

			void DX12System::ResizeDepthBuffer(const glm::ivec2& a_Size)
			{
				// Flush any GPU commands that might be referencing the depth buffer.
				Flush();

				m_Size = a_Size;

				auto device = GetDevice();

				// Resize screen dependent resources.
				// Create a depth buffer.
				D3D12_CLEAR_VALUE optimizedClearValue = {};
				optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
				optimizedClearValue.DepthStencil = { 1.0f, 0 };

				CD3DX12_HEAP_PROPERTIES heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Size.x, m_Size.y,
					1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
				if (FAILED(device->CreateCommittedResource(
					&heapType,
					D3D12_HEAP_FLAG_NONE,
					&tex,
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					&optimizedClearValue,
					IID_PPV_ARGS(&m_DepthBuffer)
				)))
				{
					LOG(LOGSEVERITY_ERROR, CATEGORY_DX12, "Failed creating committed resource.");
					return;
				}

				// Update the depth-stencil view.
				D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
				dsv.Format = DXGI_FORMAT_D32_FLOAT;
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsv.Texture2D.MipSlice = 0;
				dsv.Flags = D3D12_DSV_FLAG_NONE;

				device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv,
					m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
			}

#pragma endregion DX12_SYSTEM
		}
	}
}