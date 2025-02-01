#pragma once

#ifdef __EDITOR__

#include "core/System.h"
#include "core/Data.h"

#include "editor/EditorSettings.h"
#include "editor/AssetDatabase.h"

#define SETTINGS_FOLDER "/coopscoop/"

struct ID3D12GraphicsCommandList;

namespace coopscoop
{
	namespace graphics
	{
		class Window;
	}
	namespace editor
	{
		class SceneExplorerResource;

		namespace imgui
		{
			class EditorSelectable;
		}

		/// <summary>
		/// Core system for managing project-related changes and editor states.
		/// </summary>
		class Editor : public core::System
		{
		public:
			/// <summary>
			/// Initializes the imgui system.
			/// </summary>
			bool Initialize() override;

			/// <summary>
			/// Initializes the editor settings.
			/// </summary>
			/// <returns>True if initialization succeeds, otherwise false.</returns>
			bool InitializeEditorSettings();

			/// <summary>
			/// Cleans up resources and destroys the editor resources.
			/// </summary>
			/// <returns>True if destruction succeeds, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// Provides access to the editor settings.
			/// </summary>
			/// <returns>A reference to the EditorSettings instance.</returns>
			EditorSettings& GetEditorSettings();

			/// <summary>
			/// Provides access to the asset database.
			/// </summary>
			/// <returns>A reference to the AssetDatabase instance.</returns>
			AssetDatabase& GetAssetDatabase();

			/// <summary>
			/// Retrieves the currently loaded scene.
			/// </summary>
			/// <returns>A pointer to the current SceneExplorerResource.</returns>
			SceneExplorerResource* GetCurrentScene() const;

			/// <summary>
			/// Sets the currently active scene in the editor.
			/// </summary>
			/// <param name="a_Scene">The new SceneExplorerResource to set as the current scene.</param>
			void SetCurrentScene(SceneExplorerResource* a_Scene);

			/// <summary>
			/// Marks the editor as having unsaved changes.
			/// </summary>
			//void SetDirty();

			/// <summary>
			/// Checks whether the editor has unsaved changes.
			/// </summary>
			/// <returns>True if there are unsaved changes, otherwise false.</returns>
			//bool GetDirty();

			/// <summary>
			/// Retrieves the currently selected object in the editor.
			/// </summary>
			/// <returns>A pointer to the currently selected EditorSelectable object.</returns>
			imgui::EditorSelectable* GetSelectable() const;

			/// <summary>
			/// Sets the currently selected object in the editor.
			/// </summary>
			/// <param name="a_Selectable">The EditorSelectable object to set as selected.</param>
			void SetSelectable(imgui::EditorSelectable* a_Selectable);

			/// <summary>
			/// Provides access to the editor clipboard data.
			/// </summary>
			/// <returns>A reference to the clipboard's Data object.</returns>
			core::Data& GetClipboard();

			/// <summary>
			/// Sets the clipboard data for the editor.
			/// </summary>
			/// <param name="a_Data">The Data object to set as the clipboard content.</param>
			void SetClipboard(core::Data& a_Data);
		private:
			core::Data m_Clipboard; /// Stores clipboard data for copy-paste operations.
			imgui::EditorSelectable* m_EditorSelectable; /// The currently selected object in the editor.

			EditorSettings m_EditorSettings; /// The settings manager for the editor.
			AssetDatabase m_AssetDatabase; /// The asset database for managing project assets.

			SceneExplorerResource* m_CurrentScene = nullptr; /// Pointer to the currently active scene in the editor.
		};
	}
}

#endif // __EDITOR__