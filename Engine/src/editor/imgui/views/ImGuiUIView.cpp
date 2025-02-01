#ifdef __EDITOR__

#include "editor/imgui/views/ImGuiUIView.h"

// #include "editor/imgui/ImGuiWindow.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			ImGuiUIView::ImGuiUIView(const ImGuiWindow& a_Window) : m_Window(a_Window)
			{ }

			const ImGuiWindow& ImGuiUIView::Window() const
			{
				return m_Window;
			}
		}
	}
}

#endif // __EDITOR__