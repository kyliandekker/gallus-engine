#ifdef _EDITOR

#include "editor/imgui/views/Selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"

#include <imgui/imgui_helpers.h>

#include "core/Engine.h"
#include "editor/imgui/font_icon.h"

constexpr float PREVIEW_SIZE = 300;

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			void ExplorerResourceWithPreviewUIView::RenderBaseSelectable(ExplorerResource* a_Resource)
			{
				ExplorerResourceUIView::RenderBaseSelectable(a_Resource);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RESOURCE_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					(ImGui::GetContentRegionAvail().y - PREVIEW_SIZE) - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{

				}
			}

			void ExplorerResourceWithPreviewUIView::EndBaseSelectable()
			{
				ExplorerResourceUIView::EndBaseSelectable();

				ImGui::EndChild();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			void ExplorerResourceWithPreviewUIView::RenderPreviewWindow()
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RESOURCE_PREVIEW_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x,
					PREVIEW_SIZE - m_Window.GetWindowPadding().y
					)
					))
				{
				}
			}

			void ExplorerResourceWithPreviewUIView::EndPreviewWindow()
			{
				ImGui::EndChild();
				ImGui::PopStyleColor();
			}

			ExplorerResourceWithPreviewUIView::ExplorerResourceWithPreviewUIView(ImGuiWindow& a_Window) : ExplorerResourceUIView(a_Window)
			{}

			ExplorerResourceWithPreviewUIView::ExplorerResourceWithPreviewUIView(const ExplorerResourceWithPreviewUIView& a_Other) : ExplorerResourceUIView(a_Other)
			{}

			void ExplorerResourceWithoutPreviewUIView::RenderBaseSelectable(ExplorerResource* a_Resource)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));

				ExplorerResourceUIView::RenderBaseSelectable(a_Resource);

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RESOURCE_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{

				}
			}

			void ExplorerResourceWithoutPreviewUIView::EndBaseSelectable()
			{
				ExplorerResourceUIView::EndBaseSelectable();

				ImGui::EndChild();

				ImGui::PopStyleVar();
			}

			ExplorerResourceWithoutPreviewUIView::ExplorerResourceWithoutPreviewUIView(ImGuiWindow& a_Window) : ExplorerResourceUIView(a_Window)
			{}

			ExplorerResourceWithoutPreviewUIView::ExplorerResourceWithoutPreviewUIView(const ExplorerResourceWithoutPreviewUIView& a_Other) : ExplorerResourceUIView(a_Other)
			{}
		}
	}
}

#endif // _EDITOR