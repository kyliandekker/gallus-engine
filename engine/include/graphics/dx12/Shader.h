#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <string>
#include <wrl.h>
#include <memory>

#include "core/FileUtils.h"

namespace gallus
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

				const std::wstring& GetName() const
				{
					return m_Name;
				};

				const fs::path& GetPath() const
				{
					return m_Name;
				};

				bool IsValid()
				{
					return m_PipelineState.Get();
				};

				bool LoadByName(const std::wstring& a_VertexShader, const std::wstring& a_PixelShader);
				bool LoadByPath(const fs::path& a_VertexShaderPath, const fs::path& a_PixelShaderPath);
			private:

				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;

				std::wstring m_Name;
				fs::path m_Path;

				friend class ResourceAtlas;
			};
		}
	}
}