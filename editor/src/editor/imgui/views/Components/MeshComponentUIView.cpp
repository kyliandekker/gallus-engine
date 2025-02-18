#include "editor/imgui/views/Components/MeshComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "editor/imgui/font_icon.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Material.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			void MeshComponentUIView::RenderInner()
			{
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Mesh");

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Text(m_Component.GetTexture() ? std::string(m_Component.GetTexture()->GetName().begin(), m_Component.GetTexture()->GetName().end()).c_str() : "None");
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();

				ImGui::NewLine();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Shader");

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Text(m_Component.GetShader() ? std::string(m_Component.GetShader()->GetName().begin(), m_Component.GetShader()->GetName().end()).c_str() : "None");
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();

				ImGui::NewLine();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture");

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Text(m_Component.GetTexture() ? std::string(m_Component.GetTexture()->GetName().begin(), m_Component.GetTexture()->GetName().end()).c_str() : "None");
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();

				ImGui::NewLine();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Material");

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Text(m_Component.GetMaterial() ? std::string(m_Component.GetMaterial()->GetName().begin(), m_Component.GetMaterial()->GetName().end()).c_str() : "None");
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			std::string MeshComponentUIView::GetName() const
			{
				return font::ICON_CUBE + std::string(" Mesh");
			}
		}
	}
}