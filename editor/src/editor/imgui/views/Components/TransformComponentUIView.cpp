#ifdef _EDITOR

#include "editor/imgui/views/Components/TransformComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "editor/imgui/font_icon.h"
#include "editor/imgui/ImGuiWindow.h"
#include "gameplay/systems/components/TransformComponent.h"
#include "gameplay/systems/TransformSystem.h"
#include "core/Engine.h"
#include "editor/explorer_resources/SceneExplorerResource.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			std::string TransformComponentUIView::GetName() const
			{
				return font::ICON_AXIS + std::string(" Transform");
			}

			void TransformComponentUIView::RenderInner()
			{
				float fontSize = m_Window.GetFontSize();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Position");
				ImGui::Indent();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(fontSize / 2, fontSize / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushItemWidth(75);

				m_PositionView.SetValue(m_Component.Transform().GetPosition());
				if (m_PositionView.Render("TRANSFORM_POSITION_INSPECTOR"))
				{
					m_Component.Transform().SetPosition(m_PositionView.GetValue());
					core::ENGINE.GetEditor().SetDirty();
				}

				ImGui::Unindent();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Rotation");
				ImGui::Indent();

				m_RotationView.SetValue(m_Component.Transform().GetRotation());
				if (m_RotationView.Render("TRANSFORM_ROTATION_INSPECTOR"))
				{
					m_Component.Transform().SetRotation(m_RotationView.GetValue());
					core::ENGINE.GetEditor().SetDirty();
				}

				ImGui::Unindent();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Scale");
				ImGui::Indent();

				m_ScaleView.SetValue(m_Component.Transform().GetScale());
				if (m_ScaleView.Render("TRANSFORM_SCALE_INSPECTOR"))
				{
					m_Component.Transform().SetScale(m_ScaleView.GetValue());
					core::ENGINE.GetEditor().SetDirty();
				}

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::Unindent();
			}
		}
	}
}

#endif // _EDITOR