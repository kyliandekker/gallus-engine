#ifdef __EDITOR__

#pragma once

#include "editor/imgui/views/Selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"
#include "editor/imgui/views/DataTypes/StringDropdown.h"

namespace coopscoop
{
	namespace editor
	{
		class ExplorerResource;
		class SceneExplorerResource;

		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Represents a UI view for a scene explorer resource.
			/// </summary>
			class SceneExplorerResourceUIView : public ExplorerResourceWithoutPreviewUIView
			{
			public:
				/// <summary>
				/// Retrieves the associated resource for this view.
				/// </summary>
				/// <returns>A pointer to the associated <see cref="ExplorerResource"/>.</returns>
				ExplorerResource* GetResource() override;

				/// <summary>
				/// Renders the selectable UI element for this resource.
				/// </summary>
				void RenderSelectable() override;
			protected:
				/// <summary>
				/// Executed when the user double clicks on the resource. Overridden.
				/// </summary>
				void DoubleClicked() override;

				/// <summary>
				/// Constructor for the audio resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the specific scene resource.</param>
				SceneExplorerResourceUIView(const ImGuiWindow& a_Window, SceneExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				SceneExplorerResourceUIView(const SceneExplorerResourceUIView& a_Other);

				SceneExplorerResource& m_Resource; /// Reference to the scene explorer resource.

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};
		}
	}
}

#endif // __EDITOR__