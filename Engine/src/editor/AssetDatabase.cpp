#ifdef __EDITOR__

#include "editor/AssetDatabase.h"

#include "editor/Editor.h"
#include "core/logger/Logger.h"
#include "core/FileUtils.h"
#include "core/Engine.h"

namespace coopscoop
{
	namespace editor
	{
		bool AssetDatabase::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized asset database.");
			return System::Initialize();
		}

		bool AssetDatabase::Destroy()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed asset database.");
			return System::Destroy();
		}

		bool AssetDatabase::LoadProject(const std::string& a_Path)
		{
			m_ProjectPath = a_Path;

			core::ENGINE.GetEditor().GetEditorSettings().AddToPreviousProjects(m_ProjectPath.generic_string());

			Scan();
			m_OnProjectLoaded();

			return true;
		}

		void AssetDatabase::Rescan()
		{
			m_Rescan = true;
		}

		void AssetDatabase::CheckAssetDatabase()
		{
			if (m_Rescan)
			{
				m_Rescan = false;

				Scan();
			}
		}

		ExplorerResource& AssetDatabase::GetRoot()
		{
			return m_AssetsRoot;
		}

		const fs::path& AssetDatabase::GetProjectPath() const
		{
			return m_ProjectPath;
		}

		bool AssetDatabase::Scan()
		{
			m_OnBeforeScan();

			m_Ready = false;
			fs::path path = file::FileLoader::GetPath(m_ProjectPath.generic_string() + "/assets");
			if (!fs::exists(path))
			{
				file::FileLoader::CreateFolder(path);
			}

			m_AssetsRoot.m_Path = path;
			if (m_AssetsRoot.Scan())
			{
				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Scanned asset database.");
				m_Ready = true;
				m_OnScanCompleted();
			}
			return m_Ready;
		}
	}
}

#endif // __EDITOR__