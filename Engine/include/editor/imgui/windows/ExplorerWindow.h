#pragma once

#ifdef __EDITOR__

#include <vector>
#include <string>

#include "editor/imgui/windows/BaseWindow.h"
#include "editor/imgui/views/DataTypes/StringTextInput.h"
#include "editor/imgui/views/Selectables/ExplorerResourceUIView.h"

namespace renegade
{
	namespace editor
	{
		class ExplorerResource;

		namespace imgui
		{
			class ImGuiWindow;

			enum class ExplorerViewMode
			{
				ExplorerViewMode_List,
				ExplorerViewMode_Grid
			};

			/// <summary>
			/// A window that displays and manages the asset database of the project.
			/// Allows navigation, inspection, and organization of assets in a folder-like structure.
			/// </summary>
			class ExplorerWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs an explorer window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ExplorerWindow(const ImGuiWindow& a_Window);

				/// <summary>
				/// Initializes all values and behaviours associated with the explorer window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

				/// <summary>
				/// Destroys and disables the explorer window.
				/// </summary>
				/// <returns>True if destruction is successful, otherwise false.</returns>
				bool Destroy() override;

                /// <summary>
                /// Sets the root folder of the explorer that shows the files.
                /// </summary>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
                void SetExplorerRoot(ExplorerResourceUIView* a_Resource);

				/// <summary>
				/// Renders a folder in the explorer.
				/// </summary>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
				/// <param name="a_Indent">Amount of indents.</param>
				/// <param name="a_InitialPos">The starting pos for indent 0.</param>
				void RenderFolder(ExplorerResourceUIView* a_Resource, int a_Indent, const ImVec2& a_InitialPos);

				/// <summary>
				/// Renders the explorer window.
				/// </summary>
				void Render() override;

				/// <summary>
				/// Callback function for when the scanning of the explorer has been completed.
				/// </summary>
				void OnScanCompleted();

				/// <summary>
				/// Callback function for when the scanning of the explorer has been called.
				/// </summary>
				void OnBeforeScan();
			private:
				bool m_NeedsRefresh = true; /// Whether the explorer needs to refresh the results shown in the explorer window.

				ExplorerViewMode m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_List; // How are explorer resources shown?
				std::vector<ExplorerResourceUIView*> m_FilteredResources; /// List of explorer items shown in the explorer window.
				ExplorerResourceUIView* m_AssetRoot = nullptr; /// Root of the asset database.
				ExplorerResourceUIView* m_FolderRoot = nullptr; /// Root of the folder that is shown in the explorer window.
				ExplorerResourceUIView* m_NewFolderRoot = nullptr; /// Temporary variable that detects changes in root folder structures.
				ExplorerResourceUIView m_PreviousFolder; /// Temporary variable that refers to the previous folder.

				SearchBarInput m_SearchBar; /// Search bar to filter specific explorer items in the explorer window.
			};
		}
	}
}

#endif // __EDITOR__