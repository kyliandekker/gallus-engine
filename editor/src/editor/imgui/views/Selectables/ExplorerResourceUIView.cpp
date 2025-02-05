#ifdef _EDITOR

#include "editor/imgui/views/Selectables/ExplorerResourceUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/imgui/ImGuiDefines.h"
#include "utils/string_extensions.h"
#include "assets/AssetType.h"
#include "editor/ExplorerResource.h"
#include "editor/explorer_resources/AudioExplorerResource.h"
#include "editor/explorer_resources/SceneExplorerResource.h"
#include "core/Engine.h"
#include "core/FileUtils.h"

#include "editor/imgui/views/Selectables/ExplorerResourceViews/DefaultExplorerResourceUIView.h"
#include "editor/imgui/views/Selectables/ExplorerResourceViews/TextureExplorerResourceUIView.h"
#include "editor/imgui/views/Selectables/ExplorerResourceViews/AudioExplorerResourceUIView.h"
#include "editor/imgui/views/Selectables/ExplorerResourceViews/SceneExplorerResourceUIView.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			std::vector<std::string> RESOURCE_ICONS =
			{
				ICON_FILE_SETTINGS,
				ICON_FILE_SCENE,
				ICON_FILE_MATERIAL,
				ICON_FILE_IMAGE,
				ICON_FILE_IMAGE,
				ICON_FILE_FONT,
				ICON_FILE_AUDIO,
				ICON_FILE_MUSIC,
				ICON_FILE_VO,
				ICON_FILE, // TODO: FIND ICON ANIMATION
				ICON_FILE_MODEL,
				ICON_FILE, // TODO: FIND ICON PREFAB
			};

			ExplorerResourceUIView::~ExplorerResourceUIView()
			{
				ClearChildren();
			}

			ExplorerResourceUIView* ExplorerResourceUIView::CreateViewFromExplorerResource(ExplorerResource* a_Resource, ImGuiWindow& a_Window)
			{
				ExplorerResourceUIView* explorerView = nullptr;
				switch (a_Resource->GetAssetType())
				{
					case assets::AssetType::Texture:
					{
						explorerView = new TextureExplorerResourceUIView(a_Window, dynamic_cast<TextureExplorerResource*>(a_Resource));
						break;
					}
					case assets::AssetType::Sprite:
					{
						explorerView = new SpriteExplorerResourceUIView(a_Window, dynamic_cast<SpriteExplorerResource*>(a_Resource));
						break;
					}
					case assets::AssetType::Font:
					{
						explorerView = new FontExplorerResourceUIView(a_Window, dynamic_cast<FontExplorerResource*>(a_Resource));
						break;
					}
					case assets::AssetType::Sound:
					{
						explorerView = new SoundExplorerResourceUIView(a_Window, dynamic_cast<SoundExplorerResource*>(a_Resource));
						break;
					}
					case assets::AssetType::Song:
					{
						explorerView = new SongExplorerResourceUIView(a_Window, dynamic_cast<SongExplorerResource*>(a_Resource));
						break;
					}
					case assets::AssetType::VO:
					{
						explorerView = new VOExplorerResourceUIView(a_Window, dynamic_cast<VOExplorerResource*>(a_Resource));
						break;
					}
					case assets::AssetType::Scene:
					{
						explorerView = new SceneExplorerResourceUIView(a_Window, dynamic_cast<SceneExplorerResource*>(a_Resource));
						break;
					}
					default:
					{
						explorerView = new DefaultExplorerResourceUIView(a_Window, a_Resource);
						break;
					}
				}
				explorerView->SetData(a_Resource);

				return explorerView;
			}

			void ExplorerResourceUIView::ClearChildren()
			{
				for (ExplorerResourceUIView* resource : m_Resources)
				{
					delete resource;
				}
				m_Resources.clear();
			}

			ExplorerResourceUIView* ExplorerResourceUIView::GetParent() const
			{
				return m_Parent;
			}

			std::vector<ExplorerResourceUIView*>& ExplorerResourceUIView::GetChildren()
			{
				return m_Resources;
			}

			ExplorerResourceUIType ExplorerResourceUIView::GetExplorerResourceUIType() const
			{
				return m_ExplorerResourceUIType;
			}

			void ExplorerResourceUIView::SetExplorerResourceUIType(ExplorerResourceUIType a_ExplorerResourceUIType)
			{
				m_ExplorerResourceUIType = a_ExplorerResourceUIType;
			}

			ExplorerResourceType ExplorerResourceUIView::GetExplorerResourceType() const
			{
				return m_ResourceType;
			}

			void ExplorerResourceUIView::SetExplorerResourceType(ExplorerResourceType a_ExplorerResourceType)
			{
				m_ResourceType = a_ExplorerResourceType;
			}

			ExplorerResourceUIView::ExplorerResourceUIView(ImGuiWindow& a_Window) : EditorSelectable(a_Window), m_NameInput(a_Window)
			{
				m_NameInput.Initialize("");
			}

			const std::string& ExplorerResourceUIView::GetName() const
			{
				return m_Name;
			}

			void ExplorerResourceUIView::SetName(const std::string& a_Name)
			{
				m_Name = a_Name;
			}

			const std::string& ExplorerResourceUIView::GetIcon() const
			{
				return m_Icon;
			}

			void ExplorerResourceUIView::SetIcon(const std::string& a_Icon)
			{
				m_Icon = a_Icon;
			}

			bool ExplorerResourceUIView::IsFoldedOut() const
			{
				return m_FoldedOut;
			}

			void ExplorerResourceUIView::SetFoldedOut(bool a_FoldedOut)
			{
				m_FoldedOut = a_FoldedOut;
			}

			void ExplorerResourceUIView::CreateExplorerResourceUIViewChild(ExplorerResourceUIView* a_ResourceView, ExplorerResource* a_Resource, ImGuiWindow& a_Window)
			{
				a_ResourceView->ClearChildren();
				for (ExplorerResource* resource : a_Resource->m_Resources)
				{
					ExplorerResourceUIView* resourceToAdd = ExplorerResourceUIView::CreateViewFromExplorerResource(resource, a_Window);
					resourceToAdd->m_Parent = a_ResourceView;
					CreateExplorerResourceUIViewChild(resourceToAdd, resource, a_Window);
					a_ResourceView->m_Resources.push_back(resourceToAdd);
				}
			}

			void ExplorerResourceUIView::SetData(ExplorerResource* a_Resource)
			{
				m_Name = a_Resource->GetPath().stem().generic_string();
				m_Icon = a_Resource->GetResourceType() == ExplorerResourceType::Folder ? ICON_FOLDER : RESOURCE_ICONS[(int) a_Resource->GetAssetType()];
				m_StrResourceType = a_Resource->GetResourceType() == ExplorerResourceType::Folder ? "" : assets::AssetTypeToString(a_Resource->GetAssetType());
				m_ResourceType = a_Resource->GetResourceType();

				CreateExplorerResourceUIViewChild(this, a_Resource, m_Window);
			}

			void ExplorerResourceUIView::Render(bool& a_Clicked, bool& a_RightClicked, bool& a_DoubleClicked, bool a_Selected)
			{
				// Set the size of each child
				ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);
				ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
				ImVec2 cursorPos = screenCursorPos;
				ImVec2 childEnd = ImVec2(cursorPos.x + childSize.x, cursorPos.y + childSize.y);

				ImGui::SetCursorScreenPos(cursorPos);
				if (a_Selected)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
				}

				// Manually add padding.
				ImVec2 iconPos = ImVec2(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 3), ImGui::GetCursorPosY());
				if (ImGui::InvisibleButton(IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_INNER_EXPLORER_LIST_INVIS_BUTTON_" + m_Name).c_str(), childSize))
				{
					a_Clicked = true;
				}
				a_DoubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);
				if (ImGui::IsItemHovered())
				{
					ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
				}

				ImGui::PushFont(m_Window.GetIconFont());

				// Dynamically calculate the size of the icon
				ImVec2 iconSize = ImGui::CalcTextSize(m_Icon.c_str()); // Replace this with your icon size calculation.

				float iconOffset = m_Window.GetIconFont()->FontSize * 1.5f;

				// Calculate offsets for centering
				float verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				ImVec2 centerPos = iconPos;
				centerPos.y += verticalOffset;

				// Set cursor to the calculated position and render the icon
				ImGui::SetCursorPos(centerPos);
				RenderIcon();

				ImGui::PopFont();

				ImVec2 textSize = ImGui::CalcTextSize(m_Name.c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					iconPos.x + iconOffset,
					iconPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(m_Name.c_str());

				textSize = ImGui::CalcTextSize(m_StrResourceType.c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					centerPos.x + 300,
					iconPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				textColor.w = 0.5f;
				ImGui::TextColored(textColor, m_StrResourceType.c_str());

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}

			void ExplorerResourceUIView::RenderGrid(float a_Width, float a_Height, bool& a_Clicked, bool& a_RightClicked, bool& a_DoubleClicked, bool a_Selected)
			{
				// Set the size of each child
				ImVec2 childSize = ImVec2(a_Width, a_Height);
				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				ImVec2 childEnd = ImVec2(cursorPos.x + childSize.x, cursorPos.y + childSize.y);

				if (ImGui::BeginChild(
					IMGUI_FORMAT_ID("", CHILD_ID, "FILES_INNER_EXPLORER_GRID_" + m_Name).c_str(),
					childSize,
					false, // Borderless child
					ImGuiWindowFlags_None
					))
				{
					ImGui::SetCursorScreenPos(cursorPos);
					if (a_Selected)
					{
						ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
					}

					ImGui::SetCursorScreenPos(cursorPos);
					if (ImGui::InvisibleButton(IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_INNER_EXPLORER_GRID_INVIS_BUTTON_" + m_Name).c_str(), childSize))
					{
						a_Clicked = true;
					}
					a_DoubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);
					if (ImGui::IsItemHovered())
					{
						ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
					}

					ImGui::SetCursorScreenPos(cursorPos);

					ImVec2 textWidth = ImGui::CalcTextSize(m_Name.c_str());
					ImVec2 contentRegion = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - (textWidth.y + m_Window.GetFramePadding().x)); // Available space

					// Render the icon
					RenderIconGrid(contentRegion.x, contentRegion.y);

					// Center and display the item name
					float availWidth = ImGui::GetContentRegionAvail().x;
					float textOffsetX = (availWidth - textWidth.x) / 2.0f;

					ImGui::SetCursorScreenPos(cursorPos);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + contentRegion.y);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffsetX);
					ImGui::Text(m_Name.c_str());
				}
				ImGui::EndChild();
			}

			void ExplorerResourceUIView::RenderTree(bool& a_Clicked, bool a_Selected, int a_Indent, const ImVec2& a_InitialPos)
			{
				ImGui::SetCursorScreenPos(ImVec2(a_InitialPos.x, ImGui::GetCursorScreenPos().y));

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
				if (ImGui::InvisibleButton(IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_FOLDERS_INVIS_BUTTON_" + m_Name).c_str(), childSize))
				{
					a_Clicked = true;
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
				}

				std::string foldIcon = m_FoldedOut ? ICON_FOLDED_OUT : ICON_FOLDED_IN;

				float offsetPos = a_InitialPos.x + (ImGui::GetStyle().IndentSpacing * a_Indent);
				ImGui::PushFont(m_Window.GetIconFont());

				// Dynamically calculate the size of the icon
				ImVec2 iconSize = ImGui::CalcTextSize(foldIcon.c_str()); // Replace this with your icon size calculation.

				float iconOffset = m_Window.GetIconFont()->FontSize * 1.25f;

				// Calculate offsets for centering
				float verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				ImVec2 centerPos = ImVec2(offsetPos, buttonCursorPos.y);
				centerPos.y += verticalOffset;

				if (HasFolders())
				{
					// Set cursor to the calculated position and render the icon
					ImGui::SetCursorPos(centerPos);
					ImGui::Text(foldIcon.c_str());

					ImGui::SetCursorPos(centerPos);
					ImVec2 foldButtonScreenPos = ImGui::GetCursorScreenPos();

					ImVec2 foldButtonMin = foldButtonScreenPos;
					ImVec2 foldButtonMax = ImVec2(foldButtonMin.x + iconSize.x, foldButtonMin.y + iconSize.y);

					ImVec2 mousePos = ImGui::GetMousePos();
					bool isFoldButtonHovered = (mousePos.x >= foldButtonMin.x && mousePos.x <= foldButtonMax.x &&
						mousePos.y >= foldButtonMin.y && mousePos.y <= foldButtonMax.y);

					bool isFoldButtonClicked = ImGui::IsMouseClicked(0) && isFoldButtonHovered;
					if (isFoldButtonClicked)
					{
						m_FoldedOut = !m_FoldedOut;
					}
				}

				// Dynamically calculate the size of the icon
				iconSize = ImGui::CalcTextSize(m_Icon.c_str()); // Replace this with your icon size calculation.

				iconOffset = m_Window.GetIconFont()->FontSize * 1.25f;

				// Calculate offsets for centering
				verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				centerPos = ImVec2(centerPos.x + iconOffset, buttonCursorPos.y);
				centerPos.y += verticalOffset;

				// Set cursor to the calculated position and render the icon
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(m_Icon.c_str());

				ImGui::PopFont();

				ImVec2 textSize = ImGui::CalcTextSize(m_Name.c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					centerPos.x + iconOffset,
					buttonCursorPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(m_Name.c_str());

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}

			void ExplorerResourceUIView::RenderIcon()
			{
				ImGui::Text(m_Icon.c_str());
			}

			void ExplorerResourceUIView::RenderIconGrid(float a_Width, float a_Height)
			{
				ImGui::PushFont(m_Window.GetBigIconFont());

				ImVec2 iconSize = ImGui::CalcTextSize(m_Icon.c_str()); // Replace with your icon size

				// Calculate position to center the icon
				ImVec2 cursorPos2 = ImGui::GetCursorPos();
				ImVec2 centerPos = ImVec2(
					cursorPos2.x + (a_Width - iconSize.x) * 0.5f,
					cursorPos2.y + (a_Height - iconSize.y) * 0.5f
				);

				// Move the cursor to the center position
				ImGui::SetCursorPos(centerPos);

				ImGui::Text(m_Icon.c_str());

				ImGui::PopFont();
			}

			void ExplorerResourceUIView::RenderBaseSelectable(ExplorerResource* a_Resource)
			{
				float x = ImGui::GetCursorPosX();
				float y = ImGui::GetCursorPosY();
				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y * 2.5f + m_Window.GetFramePadding().y);
				ImGui::BeginToolbar(toolbarSize);

				ImGui::PushFont(m_Window.GetBigIconFont());
				ImVec2 iconSize = ImGui::CalcTextSize(m_Icon.c_str()); // Replace this with your icon size calculation.
				ImGui::PopFont();

				ImVec2 iconPos = ImVec2(x + m_Window.GetFramePadding().x, y + m_Window.GetFramePadding().x);

				ImGui::SetCursorPos(ImVec2(iconPos.x + m_Window.GetFramePadding().x + iconSize.x, iconPos.y));

				m_NameInput.SetString(m_Name);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x * 2, ImGui::GetStyle().FramePadding.y));
				if (m_NameInput.Render(imgui::IMGUI_FORMAT_ID("", INPUT_ID, "NAME_INSPECTOR").c_str(), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (a_Resource->Rename(m_NameInput.GetString()))
					{
						SetData(a_Resource);
					}
				}
				ImGui::PopStyleVar();

				ImVec2 inputSize = ImGui::GetItemRectSize();

				float dif = (m_Window.GetHeaderSize().y - inputSize.y) / 2;
				iconPos = ImVec2(x + m_Window.GetFramePadding().x, iconPos.y - dif);

				ImGui::SetCursorPos(iconPos);
				ImGui::PushFont(m_Window.GetBigIconFont());
				ImGui::Text(m_Icon.c_str());
				ImGui::PopFont();

				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				ImGui::SetCursorPosY(y + (m_Window.GetHeaderSize().y * 1.5f) + m_Window.GetFramePadding().y);
				ImGui::SetCursorPosX(x);

				// TODO: Find icon for this.
				//if (ImGui::TransparentButton(IMGUI_FORMAT_ID(std::string(ICON_FA_ARROW_TURN_UP), BUTTON_ID, "SHOW_IN_EXPLORER_INSPECTOR").c_str(), ImVec2(core::ENGINE.GetEditor().GetImGuiWindow().HeaderSize().y, core::ENGINE.GetEditor().GetImGuiWindow().HeaderSize().y)))
				//{
				//	core::ENGINE.GetFileLoader().EnqueueTask([&a_Resource]() mutable
				//	{
				//		return file::FileLoader::OpenInExplorer(a_Resource->GetParent()->GetPath().c_str());
				//	});
				//}
				if (ImGui::IconButton(
					IMGUI_FORMAT_ID(std::string(ICON_DELETE), BUTTON_ID, "DELETE_INSPECTOR").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					core::ENGINE.GetEditor().SetSelectable(nullptr);

					a_Resource->Delete();

					core::ENGINE.GetEditor().GetAssetDatabase().Rescan();
				}

				ImGui::SetCursorPosY(y + toolbarSize.y);

				ImGui::EndToolbar(m_Window.GetFramePadding());
			}

			void ExplorerResourceUIView::EndBaseSelectable()
			{}

			bool ExplorerResourceUIView::HasFolders() const
			{
				for (ExplorerResourceUIView* resource : m_Resources)
				{
					if (resource->m_ResourceType == ExplorerResourceType::Folder)
					{
						return true;
					}
				}
				return false;
			}
		}
	}
}

#endif // _EDITOR