#ifdef _EDITOR

#include "editor/imgui/windows/HierarchyWindow.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui_helpers.h>

//#include "gameplay/EntityComponentSystem.h"
#include "editor/imgui/ImGuiWindow.h"
#include "editor/imgui/font_icon.h"
#include "core/logger/Logger.h"
#include "utils/string_extensions.h"
#include "core/Engine.h"
//#include "gameplay/systems/EntityDetailComponent.h"
//#include "gameplay/systems/EntityDetailSystem.h"
#include "editor/explorer_resources/SceneExplorerResource.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			HierarchyWindow::HierarchyWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_LIST) + " Hierarchy", "Hierarchy"), m_SearchBar(a_Window)
			{
				m_SearchBar.Initialize("");
			}

			bool HierarchyWindow::Initialize()
			{
				//core::ENGINE.GetECS().m_OnEntitiesUpdated += std::bind(&HierarchyWindow::UpdateEntities, this);
				//core::ENGINE.GetECS().m_OnEntityComponentsUpdated += std::bind(&HierarchyWindow::UpdateEntityComponents, this);
				return BaseWindow::Initialize();
			}

			bool HierarchyWindow::Destroy()
			{
				//core::ENGINE.GetECS().m_OnEntitiesUpdated -= std::bind(&HierarchyWindow::UpdateEntities, this);
				//core::ENGINE.GetECS().m_OnEntityComponentsUpdated -= std::bind(&HierarchyWindow::UpdateEntityComponents, this);
				return BaseWindow::Destroy();
			}

			void HierarchyWindow::Render()
			{}

			void HierarchyWindow::UpdateEntities()
			{
				//m_NeedsRefresh = true;
			}

			void HierarchyWindow::UpdateEntityComponents()
			{
				//if (EntityUIView* derivedPtr = dynamic_cast<EntityUIView*>(core::ENGINE.GetEditor().GetSelectable()))
				//{
				//	core::ENGINE.GetEditor().GetSelectable()->Deselect();
				//	core::ENGINE.GetEditor().GetSelectable()->Select();
				//}
			}
		}
	}
}

#endif // _EDITOR