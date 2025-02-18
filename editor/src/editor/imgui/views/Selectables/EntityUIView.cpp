#ifdef _EDITOR

#include "editor/imgui/views/Selectables/EntityUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/explorer_resources/SceneExplorerResource.h"
#include "editor/imgui/views/Selectables/EditorSelectable.h"
#include "core/Engine.h"
#include "editor/imgui/font_icon.h"
#include "utils/string_extensions.h"
#include "editor/imgui/ImGuiWindow.h"

#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/EntityInfoSystem.h"

#include "editor/imgui/views/Components/TransformComponentUIView.h"
#include "editor/imgui/views/Components/MeshComponentUIView.h"

#include "gameplay/systems/components/EntityInfoComponent.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			EntityUIView::EntityUIView(ImGuiWindow& a_Window, const gameplay::EntityID& a_EntityID) : EditorSelectable(a_Window), m_EntityID(a_EntityID), m_NameInput(a_Window)
			{
				m_NameInput.Initialize("");
			}

			std::string EntityUIView::GetIcon() const
			{
				return font::ICON_CUBE;
			}

			void EntityUIView::Render(bool& a_Clicked, bool a_Selected)
			{
				gameplay::EntityInfoComponent& detailComponent = core::ENGINE.GetECS().GetSystem<gameplay::EntityInfoSystem>().GetComponent(m_EntityID);

				// Set the size of each child
				ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);
				ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
				ImVec2 cursorPos = screenCursorPos;
				ImVec2 childEnd = ImVec2(cursorPos.x + childSize.x, cursorPos.y + childSize.y);

				if (a_Selected)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
				}

				ImVec2 buttonCursorPos = ImVec2(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 3), ImGui::GetCursorPosY());
				if (ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_FOLDERS_INVIS_BUTTON_" + detailComponent.GetName()).c_str(), childSize))
				{
					a_Clicked = true;
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
				}

				ImGui::PushFont(m_Window.GetIconFont());

				// Dynamically calculate the size of the icon
				ImVec2 iconSize = ImVec2(m_Window.GetFontSize(), m_Window.GetFontSize()); // Replace this with your icon size calculation.

				float iconOffset = m_Window.GetIconFont()->FontSize * 2.0f;

				// Calculate offsets for centering
				float verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				ImVec2 centerPos = ImVec2(0, buttonCursorPos.y);
				centerPos.y += verticalOffset;

				std::string	folderIcon = font::ICON_CUBE;

				// Dynamically calculate the size of the icon
				iconSize = ImGui::CalcTextSize(folderIcon.c_str()); // Replace this with your icon size calculation.

				iconOffset = m_Window.GetIconFont()->FontSize * 2.0f;

				// Calculate offsets for centering
				verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				centerPos = ImVec2(centerPos.x + iconOffset, buttonCursorPos.y);
				centerPos.y += verticalOffset;

				// Set cursor to the calculated position and render the icon
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(folderIcon.c_str());

				ImGui::PopFont();

				ImVec2 textSize = ImGui::CalcTextSize(detailComponent.GetName().c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					centerPos.x + iconOffset,
					buttonCursorPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(detailComponent.GetName().c_str());

				centerPos = ImVec2(0, buttonCursorPos.y);
				ImGui::SetCursorPos(centerPos);
				bool temp = detailComponent.IsActive();
				ImGui::SetNextItemAllowOverlap();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				if (ImGui::Checkbox(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, string_extensions::StringToUpper(detailComponent.GetName()) + "_HIERARCHY").c_str(), &temp))
				{
					detailComponent.SetIsActive(temp);
					if (core::ENGINE.GetEditor().GetCurrentScene() && !core::ENGINE.GetECS().HasStarted())
					{
						core::ENGINE.GetEditor().SetDirty();
					}
				}
				ImGui::PopStyleVar();

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}

			void EntityUIView::RenderSelectable()
			{
				bool dirty = false;

				gameplay::EntityInfoComponent& detailComponent = core::ENGINE.GetECS().GetSystem<gameplay::EntityInfoSystem>().GetComponent(m_EntityID);

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y * 2.5f);
				ImGui::BeginToolbar(toolbarSize);

				float y = ImGui::GetCursorPosY();
				float x = ImGui::GetCursorPosX() + m_Window.GetFontSize();
				ImGui::SetCursorPos(ImVec2(x, y + m_Window.GetFontSize()));
				ImGui::PushFont(m_Window.GetBigIconFont());
				ImGui::Text(GetIcon().c_str());
				ImGui::PopFont();

				ImGui::SetCursorPosY(y + (m_Window.GetFontSize() / 2));
				ImGui::SetCursorPosX(x + (m_Window.GetFontSize() * 3));

				m_NameInput.SetString(detailComponent.GetName());
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
				if (m_NameInput.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "NAME_INSPECTOR").c_str(), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					detailComponent.SetName(m_NameInput.GetString());
				}

				ImGui::SetCursorPosY(y + (m_Window.GetHeaderSize().y * 1.5f));

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_DELETE), BUTTON_ID, "DELETE_INSPECTOR").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					core::ENGINE.GetECS().Delete(m_EntityID);
					dirty = true;
				}

				ImGui::EndToolbar(ImVec2(0, 0));

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (!m_EntityID.IsValid())
				{
					return;
				}

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + m_Window.GetFramePadding().y);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "COMPONENTS_INSPECTOR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x,
					ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y
					),
					ImGuiChildFlags_Borders
					))
				{
					for (ComponentBaseUIView* component : m_Components)
					{
						component->Render();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetWindowPadding().y);
					}

					ImGui::Separator();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);

					float width = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x;
					// TODO: Add these icons back. font::ICON_COMPONENT (puzzle)
					if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
					{
						ImVec2 buttonPos = ImGui::GetItemRectMin();

						// Set the popup window position just below the button
						ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

						ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str());
					}

					gameplay::EntityComponentSystem& ecs = core::ENGINE.GetECS();
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));
					ImGui::SetNextWindowSize(ImVec2(width, 0));
					if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
					{
						gameplay::TransformSystem& transformSys = ecs.GetSystem<gameplay::TransformSystem>();
						gameplay::MeshSystem& meshSys = ecs.GetSystem<gameplay::MeshSystem>();

						ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
						textColor.w = 0.5f;
						ImGui::TextColored(textColor, "Add a Component"); // TODO: Add this icon back.
						if (!transformSys.HasComponent(m_EntityID) && ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_FOLDER) + " Transform", MENU_ITEM_ID, "ADD_TRANSFORM_COMPONENT_MENU_INSPECTOR").c_str()))
						{
							transformSys.CreateComponent(m_EntityID);
							core::ENGINE.GetEditor().SetSelectable(this);
							dirty = true;
						}
						if (!meshSys.HasComponent(m_EntityID) && ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CUBE) + " Mesh", MENU_ITEM_ID, "ADD_MESH_COMPONENT_MENU_INSPECTOR").c_str()))
						{
							meshSys.CreateComponent(m_EntityID);
							core::ENGINE.GetEditor().SetSelectable(this);
							dirty = true;
						}
						ImGui::EndPopup();
					}
					ImGui::PopStyleVar();
					if (dirty)
					{
						if (core::ENGINE.GetEditor().GetCurrentScene() && !core::ENGINE.GetECS().HasStarted())
						{
							core::ENGINE.GetEditor().SetDirty();
						}
					}
				}
				ImGui::PopStyleVar();
				ImGui::EndChild();
			}

			void EntityUIView::Select()
			{
				gameplay::EntityComponentSystem& ecs = core::ENGINE.GetECS();
				gameplay::TransformSystem& transformSys = ecs.GetSystem<gameplay::TransformSystem>();
				if (ecs.GetSystem<gameplay::TransformSystem>().ContainsID(m_EntityID))
				{
					m_Components.push_back(new TransformComponentUIView(m_Window, m_EntityID, core::ENGINE.GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(m_EntityID), core::ENGINE.GetECS().GetSystem<gameplay::TransformSystem>()));
				}
				if (ecs.GetSystem<gameplay::MeshSystem>().ContainsID(m_EntityID))
				{
					m_Components.push_back(new MeshComponentUIView(m_Window, m_EntityID, core::ENGINE.GetECS().GetSystem<gameplay::MeshSystem>().GetComponent(m_EntityID), core::ENGINE.GetECS().GetSystem<gameplay::MeshSystem>()));
				}
			}

			void EntityUIView::Deselect()
			{
				for (ComponentBaseUIView* component : m_Components)
				{
					delete component;
				}
				m_Components.clear();
			}

			gameplay::EntityID EntityUIView::GetEntityID() const
			{
				return m_EntityID;
			}
		}
	}
}

#endif // _EDITOR