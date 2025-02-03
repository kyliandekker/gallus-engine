#pragma once

#include "editor/imgui/views/Selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"

namespace coopscoop
{
	namespace editor
	{
		class ExplorerResource;

		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Represents a UI view for a generic explorer resource.
			/// </summary>
			class DefaultExplorerResourceUIView : public ExplorerResourceWithoutPreviewUIView
			{
			public:
				/// <summary>
				/// Retrieves the associated resource for this view.
				/// </summary>
				/// <returns>A pointer to the resource.</returns>
				ExplorerResource* GetResource() override;

				/// <summary>
				/// Renders the selectable UI element for this resource.
				/// </summary>
				void RenderSelectable() override;
			protected:
				/// <summary>
				/// Constructor for the resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the resource.</param>
				DefaultExplorerResourceUIView(ImGuiWindow& a_Window, ExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				DefaultExplorerResourceUIView(const DefaultExplorerResourceUIView& a_Other);

				ExplorerResource& m_Resource; /// A reference to the resource.

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};
		}
	}
}