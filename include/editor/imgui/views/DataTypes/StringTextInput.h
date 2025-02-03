#pragma once

#ifdef _EDITOR

#include "editor/imgui/views/ImGuiUIView.h"

#include <string>
#include <imgui/imgui.h>

#include "core/Data.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// A simple text input field for string values in the ImGui interface.
			/// </summary>
			class StringTextInput : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructor for StringTextInput.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				StringTextInput(ImGuiWindow& a_Window);

				/// <summary>
				/// Initializes the text input with an initial value and number of characters.
				/// </summary>
				/// <param name="a_InitialValue">The initial value for the text input.</param>
				/// <param name="a_BufferSize">The maximum number of characters allowed in the input.</param>
				void Initialize(const std::string& a_InitialValue, size_t a_BufferSize = 256);

				/// <summary>
				/// Sets the current string value of the input field.
				/// </summary>
				/// <param name="a_String">The string to set as the input value.</param>
				void SetString(const std::string& a_String);

				/// <summary>
				/// Retrieves the current string value of the input field.
				/// </summary>
				/// <returns>The current string value in the input field.</returns>
				std::string GetString();

				void Render() override
				{}

/// <summary>
/// Renders the string input field in the editor window.
/// </summary>
/// <param name="a_Label">The label to display next to the input field.</param>
/// <param name="flags">Optional ImGui input text flags to customize the behavior.</param>
				bool Render(const char* a_Label, ImGuiInputTextFlags flags = 0);
			protected:
				core::Data m_Data;
			};

			/// <summary>
			/// A specialized version of StringTextInput for a search bar input field.
			/// </summary>
			class SearchBarInput : public StringTextInput
			{
			public:
				/// <summary>
				/// Constructor for SearchBarInput.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SearchBarInput(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the search bar input field with custom sizing and padding.
				/// </summary>
				/// <param name="a_Label">The label for the search bar.</param>
				/// <param name="a_Size">The size of the input field.</param>
				/// <param name="a_Padding">Additional padding around the search bar.</param>
				bool Render(const char* a_Label, const ImVec2& a_Size, float a_Padding);

				int m_PreviousResult = 0; /// Keeps track of the previous search result.
			};
		}
	}
}

#endif // _EDITOR