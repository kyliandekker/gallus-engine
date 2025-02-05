#ifdef _EDITOR

#include "editor/imgui/views/Selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"

#include <imgui/imgui_helpers.h>

#include "core/Engine.h"
#include "editor/imgui/ImGuiDefines.h"

constexpr float PREVIEW_SIZE = 200;

namespace gallus
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
			}

			void ExplorerResourceWithPreviewUIView::RenderPreviewWindow()
			{
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "EXPLORER_RdESOURCE_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x,
					PREVIEW_SIZE
					)
					))
				{
				}
			}

			void ExplorerResourceWithPreviewUIView::EndPreviewWindow()
			{
				ImGui::EndChild();
			}

			ExplorerResourceWithPreviewUIView::ExplorerResourceWithPreviewUIView(ImGuiWindow& a_Window) : ExplorerResourceUIView(a_Window)
			{}

			ExplorerResourceWithPreviewUIView::ExplorerResourceWithPreviewUIView(const ExplorerResourceWithPreviewUIView& a_Other) : ExplorerResourceUIView(a_Other)
			{}

			void ExplorerResourceWithoutPreviewUIView::RenderBaseSelectable(ExplorerResource* a_Resource)
			{
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
			}

			ExplorerResourceWithoutPreviewUIView::ExplorerResourceWithoutPreviewUIView(ImGuiWindow& a_Window) : ExplorerResourceUIView(a_Window)
			{}

			ExplorerResourceWithoutPreviewUIView::ExplorerResourceWithoutPreviewUIView(const ExplorerResourceWithoutPreviewUIView& a_Other) : ExplorerResourceUIView(a_Other)
			{}
		}
	}
}

#endif // _EDITOR