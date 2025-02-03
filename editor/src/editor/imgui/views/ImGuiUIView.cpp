#ifdef _EDITOR

#include "editor/imgui/views/ImGuiUIView.h"

// #include "editor/imgui/ImGuiWindow.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			ImGuiUIView::ImGuiUIView(ImGuiWindow& a_Window) : m_Window(a_Window)
			{}

			ImGuiWindow& ImGuiUIView::Window() const
			{
				return m_Window;
			}
		}
	}
}

#endif // _EDITOR