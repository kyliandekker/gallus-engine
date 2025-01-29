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
				bool Load(const std::string& a_ShaderName, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

				void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList);

				static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& a_FilePath, const std::string& a_EntryPoint, const std::string& a_Target);

				const std::wstring& GetName() const { return m_Name; };
			private:
				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

				std::wstring m_Name;
			};
		}
	}
}