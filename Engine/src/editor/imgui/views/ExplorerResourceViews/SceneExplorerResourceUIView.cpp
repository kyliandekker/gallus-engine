#ifdef __EDITOR__

#include "editor/imgui/views/Selectables/ExplorerResourceViews/SceneExplorerResourceUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/ExplorerResource.h"
#include "editor/explorer_resources/SceneExplorerResource.h"
#include "editor/imgui/ImGuiWindow.h"
#include "core/Engine.h"
#include "utils/string_extensions.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			ExplorerResource* SceneExplorerResourceUIView::GetResource()
			{
				return &m_Resource;
			}

			void SceneExplorerResourceUIView::RenderSelectable()
			{
				RenderBaseSelectable(&m_Resource);

				ImGui::DisplayHeader(m_Window.Bold(), "Type");
				ImGui::SameLine();
				ImGui::Text(assets::AssetTypeToString(m_Resource.GetAssetType()).c_str());

				EndBaseSelectable();
			}

			void SceneExplorerResourceUIView::DoubleClicked()
			{
				core::ENGINE.GetEditor().SetCurrentScene(&m_Resource);
				m_Resource.ResetDirty();
			}

			SceneExplorerResourceUIView::SceneExplorerResourceUIView(const ImGuiWindow& a_Window, SceneExplorerResource* a_Resource) : ExplorerResourceWithoutPreviewUIView(a_Window), m_Resource(*a_Resource)
			{ }

			SceneExplorerResourceUIView::SceneExplorerResourceUIView(const SceneExplorerResourceUIView& a_Other) : ExplorerResourceWithoutPreviewUIView(a_Other), m_Resource(a_Other.m_Resource)
			{ }
		}
	}
}

#endif // __EDITOR__