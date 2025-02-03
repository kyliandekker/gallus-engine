#ifdef _EDITOR

#include "editor/Editor.h"

#include "core/Engine.h"
//#include "editor/imgui/views/Selectables/EditorSelectable.h"
#include "utils/string_extensions.h"
#include "core/logger/Logger.h"
#include "editor/explorer_resources/SceneExplorerResource.h"

namespace coopscoop
{
	namespace editor
	{
		bool Editor::Initialize(bool a_Wait)
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Initializing editor.");
			return ThreadedSystem::Initialize(a_Wait);
		}

		void Editor::Loop()
		{
			m_AssetDatabase.CheckAssetDatabase();
		}

		bool Editor::InitializeEditorSettings()
		{
			return m_EditorSettings.Load();
		}

		bool Editor::Destroy()
		{
			bool success = m_AssetDatabase.Destroy();
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed editor.");
			return success && System::Destroy();
		}

		//void Editor::OnRender()
		//{
		//	m_AssetDatabase.CheckAssetDatabase();
		//}

		EditorSettings& Editor::GetEditorSettings()
		{
			return m_EditorSettings;
		}

		AssetDatabase& Editor::GetAssetDatabase()
		{
			return m_AssetDatabase;
		}

		SceneExplorerResource* Editor::GetCurrentScene() const
		{
			return m_CurrentScene;
		}

		void Editor::SetCurrentScene(SceneExplorerResource* a_Scene)
		{
			m_CurrentScene = a_Scene;
			m_CurrentScene->Load();
		}

		//void Editor::SetDirty()
		//{
		//	if (!m_CurrentScene)
		//	{
		//		return;
		//	}

		//	if (m_CurrentScene)
		//	{
		//		m_CurrentScene->SetDirty();
		//	}
		//}

		//bool Editor::GetDirty()
		//{
		//	return m_CurrentScene ? m_CurrentScene->IsDirty() : false;
		//}

		imgui::EditorSelectable* Editor::GetSelectable() const
		{
			return m_EditorSelectable;
		}

		void Editor::SetSelectable(imgui::EditorSelectable* a_Selectable)
		{
			if (m_EditorSelectable)
			{
				m_EditorSelectable->Deselect();
			}
			m_EditorSelectable = a_Selectable;
			if (m_EditorSelectable)
			{
				m_EditorSelectable->Select();
			}
		}

		core::Data& Editor::GetClipboard()
		{
			return m_Clipboard;
		}

		void Editor::SetClipboard(core::Data& a_Data)
		{
			m_Clipboard = a_Data;
		}

		bool Editor::InitializeThread()
		{
			if (!m_AssetDatabase.Initialize())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed initializing asset database.");
				return false;
			}
			m_AssetDatabase.LoadProject("./");
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized editor.");
			return ThreadedSystem::InitializeThread();
		}

		void Editor::Finalize()
		{
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_INPUT, "Destroyed editor.");
			ThreadedSystem::Finalize();
		}
	}
}

#endif // _EDITOR