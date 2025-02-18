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
#include "gameplay/systems/EntityInfoSystem.h"
#include "editor/explorer_resources/SceneExplorerResource.h"
#include "editor/imgui/font_icon.h"

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
				core::ENGINE.GetECS().m_OnEntitiesUpdated += std::bind(&HierarchyWindow::UpdateEntities, this);
				core::ENGINE.GetECS().m_OnEntityComponentsUpdated += std::bind(&HierarchyWindow::UpdateEntityComponents, this);
				return BaseWindow::Initialize();
			}

			bool HierarchyWindow::Destroy()
			{
				core::ENGINE.GetECS().m_OnEntitiesUpdated -= std::bind(&HierarchyWindow::UpdateEntities, this);
				core::ENGINE.GetECS().m_OnEntityComponentsUpdated -= std::bind(&HierarchyWindow::UpdateEntityComponents, this);
				return BaseWindow::Destroy();
			}

			void HierarchyWindow::Render()
			{
				std::lock_guard<std::mutex> lock(core::ENGINE.GetECS().m_EntityMutex);

				if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_S) && core::ENGINE.GetEditor().GetCurrentScene() && core::ENGINE.GetEditor().GetCurrentScene()->IsDirty())
				{
					core::ENGINE.GetEditor().GetCurrentScene()->Save();
				}

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				float topPosY = ImGui::GetCursorPosY();

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CUBE), BUTTON_ID, "SPAWN_ENTITY_HIERARCHY").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					core::ENGINE.GetECS().CreateEntity(core::ENGINE.GetECS().GetUniqueName("New GameObject"));
					m_NeedsRefresh = true;
					if (core::ENGINE.GetEditor().GetCurrentScene() && !core::ENGINE.GetECS().HasStarted())
					{
						core::ENGINE.GetEditor().SetDirty();
					}
				}
				ImGui::SameLine();

				bool wasDirty = !core::ENGINE.GetEditor().GetCurrentScene() || !core::ENGINE.GetEditor().GetCurrentScene()->IsDirty();
				if (wasDirty)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_HIERARCHY").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					core::ENGINE.GetECS().CreateEntity(core::ENGINE.GetECS().GetUniqueName("New GameObject"));
					m_NeedsRefresh = true;
					if (core::ENGINE.GetEditor().GetCurrentScene() && !core::ENGINE.GetECS().HasStarted())
					{
						core::ENGINE.GetEditor().SetDirty();
					}
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				if (wasDirty)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				float searchbarWidth = 200;
				float inputPadding = m_Window.GetFramePadding().x / 2;
				ImVec2 searchBarPos = ImVec2(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (searchbarWidth + m_Window.GetWindowPadding().x),
					(topPosY + (toolbarSize.y / 2)) - (((inputPadding * 2) + m_Window.GetFontSize()) / 2)
				);
				ImGui::SetCursorPos(searchBarPos);
				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "HIERARCHY_CONSOLE").c_str(), ImVec2(searchbarWidth, toolbarSize.y), inputPadding))
				{
					m_NeedsRefresh = true;
				}

				ImGui::SetCursorPos(endPos);

				ImGui::EndToolbar(ImVec2(0, 0));

				if (m_NeedsRefresh)
				{
					bool wasEntity = dynamic_cast<EntityUIView*>(core::ENGINE.GetEditor().GetSelectable());
					if (wasEntity)
					{
						core::ENGINE.GetEditor().SetSelectable(nullptr);
					}

					m_FilteredEntities.clear();

					m_NeedsRefresh = false;

					bool isEmptyString = m_SearchBar.GetString().empty();

					for (auto& entity : core::ENGINE.GetECS().GetEntities())
					{
						gameplay::EntityInfoComponent& detailComponent = core::ENGINE.GetECS().GetSystem<gameplay::EntityInfoSystem>().GetComponent(entity);

						if (isEmptyString || string_extensions::StringToLower(detailComponent.GetName()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_FilteredEntities.emplace_back(EntityUIView(m_Window, entity));
						}
					}
					for (EntityUIView& entityView : m_FilteredEntities)
					{
						if (m_LastID == entityView.GetEntityID())
						{
							core::ENGINE.GetEditor().SetSelectable(&entityView);
						}
					}
				}

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "ENTITIES_HIERARCHY").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					for (EntityUIView& entity : m_FilteredEntities)
					{
						bool clicked = false;
						entity.Render(clicked, &entity == core::ENGINE.GetEditor().GetSelectable());
						if (clicked)
						{
							m_LastID = entity.GetEntityID();
							core::ENGINE.GetEditor().SetSelectable(&entity);
						}
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();
			}

			void HierarchyWindow::UpdateEntities()
			{
				m_NeedsRefresh = true;
			}

			void HierarchyWindow::UpdateEntityComponents()
			{
				if (EntityUIView* derivedPtr = dynamic_cast<EntityUIView*>(core::ENGINE.GetEditor().GetSelectable()))
				{
					core::ENGINE.GetEditor().GetSelectable()->Deselect();
					core::ENGINE.GetEditor().GetSelectable()->Select();
				}
			}
		}
	}
}

#endif // _EDITOR