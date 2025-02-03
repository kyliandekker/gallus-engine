#include "graphics/dx12/Shader.h"

#include <filesystem>
namespace fs = std::filesystem;

#include "core/logger/Logger.h"
#include "core/Engine.h"
#include "graphics/dx12/CommandList.h"
#include "core/FileUtils.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			// TODO: This all needs to be loaded from a file eventually instead of from files on the disk.
			bool Shader::Load(const std::string& a_ShaderName, void*)
			{
				m_Name = std::wstring(a_ShaderName.begin(), a_ShaderName.end());
				fs::path path = file::FileLoader::GetPath("./assets/shaders/");

				fs::path vertexPath = file::FileLoader::GetPath(path.generic_string() + a_ShaderName + "_vertexshader.hlsl");
				fs::path pixelPath = file::FileLoader::GetPath(path.generic_string() + a_ShaderName + "_pixelshader.hlsl");

				auto vertexShaderBlob = CompileShader(vertexPath.generic_wstring(), "main", "vs_5_1");
				auto pixelShaderBlob = CompileShader(pixelPath.generic_wstring(), "main", "ps_5_1");

				// Create the vertex input layout
				D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				};

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

				pipelineStateStream.pRootSignature = core::ENGINE.GetDX12().GetRootSignature().Get();
				pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
				pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
				pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
				pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				pipelineStateStream.RTVFormats = rtvFormats;

				D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
					sizeof(PipelineStateStream), &pipelineStateStream
				};
				if (FAILED(core::ENGINE.GetDX12().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating pipeline state.");
					return false;
				}
				return true;
			}

			void Shader::Bind(std::shared_ptr<CommandList> a_CommandList)
			{
				a_CommandList->GetCommandList()->SetPipelineState(m_PipelineState.Get());
				a_CommandList->GetCommandList()->SetGraphicsRootSignature(core::ENGINE.GetDX12().GetRootSignature().Get()); // Set the existing root signature
			}

			Microsoft::WRL::ComPtr<ID3DBlob> Shader::CompileShader(const std::wstring& a_FilePath, const std::string& a_EntryPoint, const std::string& a_Target)
			{
				Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
				Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
				if (D3DCompileFromFile(
					a_FilePath.c_str(),
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					a_EntryPoint.c_str(),
					a_Target.c_str(),
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
					0,
					&shaderBlob,
					&errorBlob
					))
				{
					if (errorBlob)
					{
						std::string errorMessage(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
						LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Shader Compilation Error: \"%s\".", errorMessage.c_str());
					}
					else
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading vertex shader.");
					}
					return nullptr;
				}

				std::string path = std::string(a_FilePath.begin(), a_FilePath.end());
				LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Loaded shader: \"%s\".", path.c_str());
				return shaderBlob;
			}
		}
	}
}