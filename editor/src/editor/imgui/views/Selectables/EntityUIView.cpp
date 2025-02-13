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
#include "editor/imgui/views/Components/TransformComponentUIView.h"
#include "gameplay/systems/components/EntityInfoComponent.h"
#include "gameplay/systems/EntityInfoSystem.h"

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

				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				ImVec2 pos = ImGui::GetCursorScreenPos();

				ImVec2 select_min = pos;
				ImVec2 select_max = ImVec2(select_min.x + ImGui::GetContentRegionAvail().x, select_min.y + m_Window.GetFontSize());

				bool is_hovered = ImGui::IsMouseHoveringRect(select_min, select_max);
				if (a_Selected || is_hovered)
				{
					ImVec4 color = ImGui::GetStyleColorVec4(a_Selected ? ImGuiCol_HeaderHovered : ImGuiCol_HeaderActive);
					ImU32 select_color = ImGui::ColorConvertFloat4ToU32(color);

					draw_list->AddRectFilled(select_min, select_max, select_color, 0);
				}

				pos = ImGui::GetCursorPos();

				bool active = detailComponent.IsActive();
				if (!active)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);
				}

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

				ImGui::SetCursorPos(ImVec2(pos.x + 30, pos.y));
				ImGui::Text(GetIcon().c_str());
				ImGui::SetCursorPos(ImVec2(pos.x + 65, pos.y));
				ImGui::Text(detailComponent.GetName().c_str());

				a_Clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_hovered;

				if (!active)
				{
					ImGui::PopStyleVar();
				}
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

				bool temp = detailComponent.IsActive();
				// TODO: Add these icons back.
				//if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(temp ? font::ICON_FA_CHECKMARK_CHECKED : font::ICON_FA_CHECKMARK, CHECKBOX_ID, string_extensions::StringToUpper(detailComponent.GetName()) + "_INSPECTOR").c_str(), &temp, ImVec2(core::ENGINE.GetEditor().GetImGuiWindow().HeaderSize().y, core::ENGINE.GetEditor().GetImGuiWindow().HeaderSize().y)))
				//{
				//	detailComponent.SetActive(temp);
				//	dirty = true;
				//}
				//ImGui::SameLine();
				//if (ImGui::TransparentButton(ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_DELETE), BUTTON_ID, "DELETE_INSPECTOR").c_str(), ImVec2(core::ENGINE.GetEditor().GetImGuiWindow().HeaderSize().y, core::ENGINE.GetEditor().GetImGuiWindow().HeaderSize().y)))
				//{
				//	core::ENGINE.GetECS().Delete(m_EntityID);
				//	dirty = true;
				//}

				ImGui::EndToolbar(ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (!m_EntityID.IsValid())
				{
					return;
				}

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);

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
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
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

						ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
						textColor.w = 0.5f;
						ImGui::TextColored(textColor, "Add a Component"); // TODO: Add this icon back.
						if (!transformSys.HasComponent(m_EntityID) && ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_FOLDER) + " Transform", MENU_ITEM_ID, "ADD_TRANSFORM_COMPONENT_MENU_INSPECTOR").c_str()))
						{
							transformSys.CreateComponent(m_EntityID);
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