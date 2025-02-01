#pragma once

#ifdef __EDITOR__

#include <string>

#include "editor/ExplorerResource.h"
#include "core/System.h"
#include "core/Event.h"
#include "core/FileUtils.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			class EditorSelectable;
		}

		/// <summary>
		/// A system that displays and manages the asset database of the project.
		/// </summary>
		class AssetDatabase : public core::System
		{
		public:
			/// <summary>
			/// Initializes the imgui system.
			/// </summary>
			bool Initialize() override;

			/// <summary>
			/// Cleans up resources and destroys the asset database resources.
			/// </summary>
			/// <returns>True if destruction succeeds, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// Loads a project.
			/// </summary>
			/// <param name="a_Path">Path to the project folder.</param>
			/// <returns>True if loading succeeds, otherwise false.</returns>
			bool LoadProject(const std::string& a_Path);

			/// <summary>
			/// Rescans the asset database of a project.
			/// </summary>
			void Rescan();

			/// <summary>
			/// Checks whether the database needs a rescan.
			/// </summary>
			void CheckAssetDatabase();

			/// <summary>
			/// Retrieves the root folder of the project.
			/// </summary>
			/// <returns>A reference to the root folder.</returns>
			ExplorerResource& GetRoot();

			/// <summary>
			/// Retrieves the project path of the asset database.
			/// </summary>
			/// <returns>An absolute path of the project path.</returns>
			const fs::path& GetProjectPath() const;

			SimpleEvent<> m_OnScanCompleted; /// Simple event that gets called when the asset database has completed a scan.
			SimpleEvent<> m_OnBeforeScan; /// Simple event that gets called before the asset database starts a scan.
			Event<> m_OnProjectLoaded; /// Simple event that gets called when the asset database has finished loading a project.
		private:
			bool m_Rescan; /// Whether the asset database needs to be scanned again.
			ExplorerResource m_AssetsRoot; /// The root of the asset database.

			fs::path m_ProjectPath; // The project path.

			bool Scan(); /// Function that scans the database.
		};
	}
}

#endif // __EDITOR__