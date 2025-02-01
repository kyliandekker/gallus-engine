#ifdef __EDITOR__

#include "editor/imgui/views/Selectables/ExplorerResourceViews/DefaultExplorerResourceUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/ExplorerResource.h"
#include "editor/imgui/ImGuiWindow.h"

namespace renegade
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
					ImGui::DisplayHeader(m_Window.Bold(), "Type");
					ImGui::SameLine();
					ImGui::Text(assets::AssetTypeToString(m_Resource.GetAssetType()).c_str());
				}

				EndBaseSelectable();
			}

			DefaultExplorerResourceUIView::DefaultExplorerResourceUIView(const ImGuiWindow& a_Window, ExplorerResource* a_Resource) : ExplorerResourceWithoutPreviewUIView(a_Window), m_Resource(*a_Resource)
			{ }

			DefaultExplorerResourceUIView::DefaultExplorerResourceUIView(const DefaultExplorerResourceUIView& a_Other) : ExplorerResourceWithoutPreviewUIView(a_Other), m_Resource(a_Other.m_Resource)
			{ }
		}
	}
}

#endif // __EDITOR__