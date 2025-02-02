#ifdef __EDITOR__

#include "editor/imgui/views/Selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"

#include <imgui/imgui_helpers.h>

#include "core/Engine.h"
#include "editor/imgui/ImGuiDefines.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			void ExplorerResourceWithPreviewUIView::RenderBaseSelectable(ExplorerResource* a_Resource)
			{
				ExplorerResourceUIView::RenderBaseSelectable(a_Resource);

				ImVec2 framePadding = m_Window.GetFramePadding();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(framePadding.x * 2, framePadding.y * 2));
				if (ImGui::BeginChild(
					imgui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RESOURCE_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x,
					ImGui::GetContentRegionAvail().y - (ImGui::GetStyle().ItemSpacing.y + 200)
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
			}

			void ExplorerResourceWithPreviewUIView::RenderPreviewWindow()
			{
				ImVec2 framePadding = m_Window.GetFramePadding();

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(framePadding.x * 2, framePadding.y * 2));
				if (ImGui::BeginChild(
					imgui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RdESOURCE_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x,
					200
					)
					))
				{
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					ImVec2 p0 = ImGui::GetItemRectMin();  // Get top-left corner of the child
					ImVec2 p1 = ImVec2(ImGui::GetItemRectMax().x, p0.y); // Extend to the right width of the child (adjust width if needed)

					draw_list->AddLine(p0, p1, ImGui::ImColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Border]), 1.0f);

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
				}
			}

			void ExplorerResourceWithPreviewUIView::EndPreviewWindow()
			{
				ImGui::EndChild();

				ImGui::PopStyleVar();
			}

			ExplorerResourceWithPreviewUIView::ExplorerResourceWithPreviewUIView(const ImGuiWindow& a_Window) : ExplorerResourceUIView(a_Window)
			{}

			ExplorerResourceWithPreviewUIView::ExplorerResourceWithPreviewUIView(const ExplorerResourceWithPreviewUIView& a_Other) : ExplorerResourceUIView(a_Other)
			{}

			void ExplorerResourceWithoutPreviewUIView::RenderBaseSelectable(ExplorerResource* a_Resource)
			{
				ExplorerResourceUIView::RenderBaseSelectable(a_Resource);

				ImVec2 framePadding = m_Window.GetFramePadding();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(framePadding.x * 2, framePadding.y * 2));
				if (ImGui::BeginChild(
					imgui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RESOURCE_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x,
					ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y
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

			ExplorerResourceWithoutPreviewUIView::ExplorerResourceWithoutPreviewUIView(const ImGuiWindow& a_Window) : ExplorerResourceUIView(a_Window)
			{}

			ExplorerResourceWithoutPreviewUIView::ExplorerResourceWithoutPreviewUIView(const ExplorerResourceWithoutPreviewUIView& a_Other) : ExplorerResourceUIView(a_Other)
			{}
		}
	}
}

#endif // __EDITOR__