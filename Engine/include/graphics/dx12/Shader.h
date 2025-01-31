#pragma once

#include "graphics/dx12/DX12PCH.h"
#include <string>
#include <wrl.h>
#include <memory>

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

			class Shader
			{
			public:
				Shader() = default;
				void Bind(std::shared_ptr<CommandList> a_CommandList);

				static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& a_FilePath, const std::string& a_EntryPoint, const std::string& a_Target);

				const std::wstring& GetName() const { return m_Name; };
			private:
				bool Load(const std::string& a_Name, void*);

				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

				std::wstring m_Name;

				friend class ResourceAtlas;
			};
		}
	}
}