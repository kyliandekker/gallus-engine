#pragma once

#ifdef _EDITOR

#include "editor/imgui/views/ImGuiUIView.h"

#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include <imgui/imgui.h>

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			// TODO: Use a toString function pointer instead.

			/// <summary>
			/// A drop-down UI element that allows the user to select from a list of string options.
			/// This class supports associating each option with a specific value of type T.
			/// </summary>
			/// <typeparam name="T">The type associated with each drop-down option (e.g., int, float, custom types).</typeparam>
			template <class T>
			class StringDropdown : public ImGuiUIView
			{
			public:
				StringDropdown() = default;
				/// <summary>
				/// Constructor for StringDropdown.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				StringDropdown(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				/// <summary>
				/// Initializes the drop-down with an initial value, list of options, and their corresponding string labels.
				/// </summary>
				/// <param name="a_InitialValue">The initial value for the drop-down selection.</param>
				/// <param name="a_Options">A vector containing the options to display in the drop-down.</param>
				/// <param name="a_ToString">A function that converts the associated type to a string.</param>
				void Initialize(T a_InitialValue, std::vector<T> a_Options, std::function<std::string(T)> a_ToString)
				{
					m_Options = a_Options;
					ToString = a_ToString;

					SetValue(a_InitialValue);
				}

				/// <summary>
				/// Sets the current value of the drop-down.
				/// </summary>
				/// <param name="a_Value">The new value to set.</param>
				void SetValue(T a_Value)
				{
					m_Selected = std::distance(m_Options.begin(), std::find(m_Options.begin(), m_Options.end(), a_Value));
				}

				/// <summary>
				/// Retrieves the current value of the drop-down.
				/// </summary>
				/// <returns>The current selected value.</returns>
				T GetValue() const
				{
					return m_Options[m_Selected];
				}

				void Render() override
				{}

				/// <summary>
				/// Renders the drop-down UI in the editor window.
				/// </summary>
				/// <param name="a_Label">The label for the drop-down UI element.</param>
				/// <returns>True if the drop-down menu is open, otherwise false.</returns>
				bool Render(const char* a_Label)
				{
					bool b = ImGui::BeginCombo(a_Label, ToString(GetValue()).c_str());
					if (b)
					{
						for (int n = 0; n < m_Options.size(); n++)
						{
							bool is_selected = (m_Selected == n);
							if (ImGui::Selectable(ToString(m_Options[n]).c_str(), is_selected))
							{
								m_Selected = n;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					return b;
				}
			private:
				std::vector<T> m_Options; /// The options to display.
				std::function<std::string(T)> ToString; /// Function pointer that converts the option to a string.
				int m_Selected = 0; /// The index of the currently selected option.
			};
		}
	}
}

#endif // _EDITOR