#ifdef __EDITOR__

#pragma once

#include "editor/imgui/views/Selectables/ExplorerResourceUIView.h"

namespace coopscoop
{
	namespace editor
	{
		class ExplorerResource;

		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Represents a UI view for a generic explorer resource with preview UI at the bottom.
			/// </summary>
			class ExplorerResourceWithPreviewUIView : public ExplorerResourceUIView
			{
			public:
				/// <summary>
				/// Renders base UI elements for the resource in the inspector.
				/// </summary>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
				void RenderBaseSelectable(ExplorerResource* a_Resource) override;

				/// <summary>
				/// Finalizes rendering of the base selectable UI.
				/// </summary>
				void EndBaseSelectable() override;

				/// <summary>
				/// Renders the preview part of the selectable.
				/// </summary>
				void RenderPreviewWindow();

				/// <summary>
				/// Marks the end of the preview part of the selectable.
				/// </summary>
				void EndPreviewWindow();
			protected:
				/// <summary>
				/// Constructor for the resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ExplorerResourceWithPreviewUIView(const ImGuiWindow& a_Window);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				ExplorerResourceWithPreviewUIView(const ExplorerResourceWithPreviewUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Represents a UI view for a generic explorer resource without preview UI at the bottom.
			/// </summary>
			class ExplorerResourceWithoutPreviewUIView : public ExplorerResourceUIView
			{
			public:
				/// <summary>
				/// Renders base UI elements for the resource in the inspector.
				/// </summary>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
				void RenderBaseSelectable(ExplorerResource* a_Resource) override;

				/// <summary>
				/// Finalizes rendering of the base selectable UI.
				/// </summary>
				void EndBaseSelectable() override;
			protected:
				/// <summary>
				/// Constructor for the resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ExplorerResourceWithoutPreviewUIView(const ImGuiWindow& a_Window);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				ExplorerResourceWithoutPreviewUIView(const ExplorerResourceWithoutPreviewUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};
		}
	}
}

#endif // __EDITOR__