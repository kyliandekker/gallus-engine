#ifdef _EDITOR

#include "editor/imgui/views/Selectables/ExplorerResourceViews/DefaultExplorerResourceUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/ExplorerResource.h"
#include "editor/imgui/ImGuiWindow.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			ExplorerResource* DefaultExplorerResourceUIView::GetResource()
			{
				return &m_Resource;
			}

			void DefaultExplorerResourceUIView::RenderSelectable()
			{
				RenderBaseSelectable(&m_Resource);

				if (m_Resource.GetResourceType() != ExplorerResourceType::Folder)
				{
					ImGui::DisplayHeader(m_Window.GetBoldFont(), m_Window.GetFramePadding(), "Type");
					ImGui::SameLine();
					ImGui::Text(assets::AssetTypeToString(m_Resource.GetAssetType()).c_str());
				}

				EndBaseSelectable();
			}

			DefaultExplorerResourceUIView::DefaultExplorerResourceUIView(ImGuiWindow& a_Window, ExplorerResource* a_Resource) : ExplorerResourceWithoutPreviewUIView(a_Window), m_Resource(*a_Resource)
			{}

			DefaultExplorerResourceUIView::DefaultExplorerResourceUIView(const DefaultExplorerResourceUIView& a_Other) : ExplorerResourceWithoutPreviewUIView(a_Other), m_Resource(a_Other.m_Resource)
			{}
		}
	}
}

#endif // _EDITOR