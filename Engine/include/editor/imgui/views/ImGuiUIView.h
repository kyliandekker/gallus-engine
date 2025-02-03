#pragma once

#ifdef __EDITOR__

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Generic UI view class used by all elements in the editor.
			/// </summary>
			class ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs an <see cref="ExplorerResourceUIView"/> for a given window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ImGuiUIView(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the element.
				/// </summary>
				virtual void Render() = 0;

				/// <summary>
				/// Retrieves the window the element is being rendered to.
				/// </summary>
				/// <returns>Reference to the window.</returns>
				ImGuiWindow& Window() const;
			protected:
				ImGuiWindow& m_Window; // The window the element is being rendered to.
			};
		}
	}
}

#endif // __EDITOR__