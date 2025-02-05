#ifdef _EDITOR

#pragma once

#include "editor/ExplorerResource.h"

namespace gallus
{
	namespace editor
	{
		/// <summary>
		/// Represents a scene resource in the editor, providing methods for managing the
		/// scene's state, including saving, loading, and marking it as dirty.
		/// </summary>
		class SceneExplorerResource : public ExplorerResource
		{
		public:
			/// <summary>
			/// Sets the window title for the scene in the editor.
			/// </summary>
			void SetWindowTitle();

			/// <summary>
			/// Marks the scene resource as "dirty," indicating that changes have been made
			/// and need to be saved.
			/// </summary>
			void SetDirty();

			/// <summary>
			/// Resets the "dirty" state of the scene, indicating that no changes have been made
			/// since the last save.
			/// </summary>
			void ResetDirty();

			/// <summary>
			/// Saves the current scene to disk.
			/// </summary>
			/// <returns>True if the scene was successfully saved, otherwise false.</returns>
			bool Save();

			/// <summary>
			/// Loads the scene from disk.
			/// </summary>
			/// <returns>True if the scene was successfully loaded, otherwise false.</returns>
			bool Load();

			/// <summary>
			/// Checks whether the resource has been modified and requires saving.
			/// </summary>
			/// <returns>
			/// True if the resource has been marked as dirty (modified), otherwise false.
			/// </returns>
			bool IsDirty() const;
		private:
			bool m_IsDirty = false;
		};
	}
}

#endif // _EDITOR