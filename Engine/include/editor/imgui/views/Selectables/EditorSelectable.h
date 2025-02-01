#pragma once

#ifdef __EDITOR__

#include <string>

#include "editor/imgui/views/ImGuiUIView.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Represents a selectable UI element in the editor, providing an interface for rendering, selection, and deselection.
			/// </summary>
			class EditorSelectable : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructor for the selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				EditorSelectable(const ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the selectable UI element for this resource.
				/// </summary>
				virtual void RenderSelectable() { };

				/// <summary>
				/// Executes logic when the element is selected. Override to define selection behavior.
				/// </summary>
				virtual void Select() {};

				/// <summary>
				/// Executes logic when the element is deselected. Override to define deselection behavior.
				/// </summary>
				virtual void Deselect() {};
			};
		}
	}
}

#endif // __EDITOR__