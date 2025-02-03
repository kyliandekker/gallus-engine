#ifdef _EDITOR

#include "editor/imgui/windows/ExplorerWindow.h"

#include "editor/imgui/ImGuiDefines.h"
#include "utils/string_extensions.h"
#include "core/FileUtils.h"
#include "editor/imgui/ImGuiWindow.h"
#include "imgui/imgui_helpers.h"
#include "editor/Editor.h"
#include "core/Engine.h"
#include "editor/ExplorerResource.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			ExplorerWindow::ExplorerWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, "Explorer", "Explorer"), m_SearchBar(a_Window), m_PreviousFolder(a_Window)
			{
				m_SearchBar.Initialize("");

				m_PreviousFolder.SetIcon(ICON_FOLDER);
				m_PreviousFolder.SetName("../");
				m_PreviousFolder.SetExplorerResourceUIType(ExplorerResourceUIType::ExplorerResourceType_PreviousFolder);
				m_PreviousFolder.SetExplorerResourceType(ExplorerResourceType::Folder);
			}

			bool ExplorerWindow::Initialize()
			{
				OnScanCompleted();

				core::ENGINE.GetEditor().GetAssetDatabase().m_OnBeforeScan += std::bind(&ExplorerWindow::OnBeforeScan, this);
				core::ENGINE.GetEditor().GetAssetDatabase().m_OnScanCompleted += std::bind(&ExplorerWindow::OnScanCompleted, this);

				return BaseWindow::Destroy();
			}

			bool ExplorerWindow::Destroy()
			{
				core::ENGINE.GetEditor().GetAssetDatabase().m_OnBeforeScan -= std::bind(&ExplorerWindow::OnBeforeScan, this);
				core::ENGINE.GetEditor().GetAssetDatabase().m_OnScanCompleted -= std::bind(&ExplorerWindow::OnScanCompleted, this);

				return BaseWindow::Destroy();
			}

			void ExplorerWindow::SetExplorerRoot(ExplorerResourceUIView* a_Resource)
			{
				if (!a_Resource)
				{
					return;
				}

				m_FolderRoot = a_Resource;
			}

			void ExplorerWindow::RenderFolder(ExplorerResourceUIView* a_Resource, int a_Indent, const ImVec2& a_InitialPos)
			{
				bool
					clicked = false;

				a_Resource->RenderTree(clicked, m_FolderRoot == a_Resource, a_Indent, a_InitialPos);

				if (clicked)
				{
					m_NewFolderRoot = a_Resource;
				}

				if (a_Resource->IsFoldedOut())
				{
					for (ExplorerResourceUIView* child : a_Resource->GetChildren())
					{
						if (child->GetExplorerResourceType() == ExplorerResourceType::Folder)
						{
							RenderFolder(child, a_Indent + 1, a_InitialPos);
						}
					}
				}
			}

			void ExplorerWindow::Render()
			{
				// If the asset database is not ready, do not continue.
				if (!core::ENGINE.GetEditor().GetAssetDatabase().Ready())
				{
					return;
				}

				// This needs to be done at the start of the frame to avoid errors.
				// We set the root directory of the second window that shows the assets.
				if (m_NewFolderRoot)
				{
					if (m_NewFolderRoot != m_FolderRoot)
					{
						if (ExplorerResourceUIView* derivedPtr = dynamic_cast<ExplorerResourceUIView*>(core::ENGINE.GetEditor().GetSelectable()))
						{
							core::ENGINE.GetEditor().SetSelectable(nullptr);
						}

						SetExplorerRoot(m_NewFolderRoot);
						m_NeedsRefresh = true;
					}
					m_NewFolderRoot = nullptr;
				}

				// This needs to be done at the start of the frame to avoid errors.
				// We refresh the assets that show up based on the search bar and the root directory.
				if (m_NeedsRefresh)
				{
					m_FilteredResources.clear();

					m_NeedsRefresh = false;

					bool isEmptyString = m_SearchBar.GetString().empty();

					// Add "../" if the resource has a parent.
					if (m_FolderRoot->GetParent())
					{
						m_FilteredResources.push_back(&m_PreviousFolder);
					}

					for (auto& resource : m_FolderRoot->GetChildren())
					{
						if (isEmptyString || string_extensions::StringToLower(resource->GetName()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_FilteredResources.push_back(resource);
						}
					}
				}

				if (!m_FolderRoot)
				{
					return;
				}

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				float topPosY = ImGui::GetCursorPosY();

				if (ImGui::IconButton(
					IMGUI_FORMAT_ID(std::string(ICON_REFRESH), BUTTON_ID, "RESCAN_EXPLORER").c_str(), ImVec2(50, toolbarSize.y), m_Window.GetIconFont()))
				{
					core::ENGINE.GetEditor().GetAssetDatabase().Rescan();
				}

				ImGui::SameLine();

				bool list = m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_List;
				bool grid = m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_Grid;
				if (ImGui::IconCheckboxButton(
					IMGUI_FORMAT_ID(std::string(ICON_LIST), BUTTON_ID, "LIST_EXPLORER").c_str(), &list, ImVec2(50, toolbarSize.y), m_Window.GetIconFont()))
				{
					if (list)
					{
						m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_List;
					}
				}

				ImGui::SameLine();

				if (ImGui::IconCheckboxButton(
					IMGUI_FORMAT_ID(std::string(ICON_GRID), BUTTON_ID, "GRID_EXPLORER").c_str(), &grid, ImVec2(50, toolbarSize.y), m_Window.GetIconFont()))
				{
					if (grid)
					{
						m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_Grid;
					}
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				float searchbarWidth = 200;
				float inputPadding = m_Window.GetFramePadding().x / 2;
				ImVec2 searchBarPos = ImVec2(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (searchbarWidth + m_Window.GetWindowPadding().x),
					(topPosY + (toolbarSize.y / 2)) - (((inputPadding * 2) + m_Window.GetFontSize()) / 2)
				);
				ImGui::SetCursorPos(searchBarPos);
				if (m_SearchBar.Render(IMGUI_FORMAT_ID("", INPUT_ID, "EXPLORER_CONSOLE").c_str(), ImVec2(searchbarWidth, toolbarSize.y), inputPadding))
				{
					m_NeedsRefresh = true;
				}

				ImGui::SetCursorPos(endPos);

				ImGui::EndToolbar(ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
				if (ImGui::BeginChild(
					IMGUI_FORMAT_ID("", CHILD_ID, "DIRECTORIES_EXPLORER").c_str(),
					ImVec2(
					0,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX
					))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					RenderFolder(m_AssetRoot, 0, ImGui::GetCursorScreenPos());
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y));
				if (ImGui::BeginChild(
					IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{
					if (m_FolderRoot)
					{
						if (ImGui::BeginChild(
							IMGUI_FORMAT_ID("", CHILD_ID, "FILES_INNER_EXPLORER").c_str(),
							ImVec2(
							0,
							0
							)
							))
						{
							if (m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_List)
							{
								for (ExplorerResourceUIView* item : m_FilteredResources)
								{
									if (!item)
									{
										continue;
									}

									bool
										clicked = false,
										right_clicked = false,
										double_clicked = false;

									item->Render(clicked, right_clicked, double_clicked, item == core::ENGINE.GetEditor().GetSelectable());

									if (clicked && item->GetExplorerResourceUIType() != ExplorerResourceUIType::ExplorerResourceType_PreviousFolder)
									{
										core::ENGINE.GetEditor().SetSelectable(item);
									}
									if (double_clicked && item->GetExplorerResourceType() == ExplorerResourceType::Folder)
									{
										if (item->GetExplorerResourceUIType() == ExplorerResourceUIType::ExplorerResourceType_PreviousFolder)
										{
											m_NewFolderRoot = m_FolderRoot->GetParent();
										}
										else
										{
											m_NewFolderRoot = item;
										}
									}
								}
							}
							else
							{
								// Get the available space in the window
								float availableWidth = ImGui::GetContentRegionAvail().x;
								float availableHeight = ImGui::GetContentRegionAvail().y;

								// Set a scaling factor for height and width
								float heightScaleFactor = 0.5f; // Icons slightly smaller than available height
								float maxIconHeight = availableHeight * heightScaleFactor; // Maximum icon height

								// Calculate the maximum size of a square icon based on height
								float maxIconSize = maxIconHeight;

								// Calculate the maximum number of icons that can fit horizontally as squares
								int iconsPerRow = static_cast<int>(availableWidth / maxIconSize);
								iconsPerRow = std::max(1, iconsPerRow); // Ensure at least 1 icon fits

								// Recalculate the square icon size to fit exactly in the row
								float iconSize = (availableWidth - (iconsPerRow - 1)) / iconsPerRow;
								iconSize = std::min(iconSize, maxIconSize); // Constrain to max height

								int count = 0; // Track the number of items rendered
								for (ExplorerResourceUIView* item : m_FilteredResources)
								{
									if (!item)
									{
										continue;
									}

									bool
										clicked = false,
										right_clicked = false,
										double_clicked = false;

									item->RenderGrid(iconSize, iconSize, clicked, right_clicked, double_clicked, item == core::ENGINE.GetEditor().GetSelectable());

									if (clicked && item->GetExplorerResourceUIType() != ExplorerResourceUIType::ExplorerResourceType_PreviousFolder)
									{
										core::ENGINE.GetEditor().SetSelectable(item);
									}
									if (double_clicked && item->GetExplorerResourceType() == ExplorerResourceType::Folder)
									{
										if (item->GetExplorerResourceUIType() == ExplorerResourceUIType::ExplorerResourceType_PreviousFolder)
										{
											m_NewFolderRoot = m_FolderRoot->GetParent();
										}
										else
										{
											m_NewFolderRoot = item;
										}
									}

									count++;
									if (count % iconsPerRow != 0)
									{
										ImGui::SameLine();
									}
									else
									{
									}
								}
							}
							ImGui::EndChild();
						}
					}
					ImGui::EndChild();
				}
			}

			void ExplorerWindow::OnScanCompleted()
			{
				if (m_AssetRoot)
				{
					delete m_AssetRoot;
				}
				m_AssetRoot = ExplorerResourceUIView::CreateViewFromExplorerResource(&core::ENGINE.GetEditor().GetAssetDatabase().GetRoot(), m_Window);
				m_NewFolderRoot = m_AssetRoot;
			}

			void ExplorerWindow::OnBeforeScan()
			{
				m_FilteredResources.clear();
				if (ExplorerResourceUIView* derivedPtr = dynamic_cast<ExplorerResourceUIView*>(core::ENGINE.GetEditor().GetSelectable()))
				{
					core::ENGINE.GetEditor().SetSelectable(nullptr);
				}
			}
		}
	}
}

#endif // _EDITOR