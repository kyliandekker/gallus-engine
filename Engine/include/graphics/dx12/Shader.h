#pragma once

#include "graphics/dx12/DX12PCH.h"
#include <string>
#include <wrl.h>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class Shader
			{
			public:
				Shader() = default;
				Shader(const std::wstring& a_VertexShaderPath, const std::wstring& a_PixelShaderPath);

				void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

				static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& a_FilePath, const std::string& a_EntryPoint, const std::string& a_Target);
			private:
				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
			};
		}
	}
}