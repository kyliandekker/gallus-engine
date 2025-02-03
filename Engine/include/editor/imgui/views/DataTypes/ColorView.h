#pragma once

#ifdef __EDITOR__

#include "editor/imgui/views/ImGuiUIView.h"

#include <imgui/imgui.h>
#include <string>

#include "editor/imgui/ImGuiDefines.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// A UI view for rendering and interacting with RGB color values.
			/// Can be used for both glm-based types and ImColor types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec3 or ImColor.</typeparam>
			template <class T>
			class ColorRGBView : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a ColorRGBView that renders an RGB color.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ColorRGBView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{ }

				/// <summary>
				/// Sets the value of the RGB color. Updates the value if it's different from the current one.
				/// </summary>
				/// <param name="a_Value">The new color value to set.</param>
				void SetValue(const T& a_Value)
				{
					if (m_Value != a_Value)
					{
						m_Value = a_Value;
					}
				}

				/// <summary>
				/// Retrieves the current RGB color value.
				/// </summary>
				/// <returns>The current color value.</returns>
				const T& GetValue() const
				{
					return m_Value;
				}

				void Render() override { }

				/// <summary>
				/// Renders the color editor with the specified label.
				/// </summary>
				/// <param name="a_Label">The label for the color editor.</param>
				/// <returns>True if the color was modified, otherwise false.</returns>
				virtual bool Render(const char* a_Label)
				{
					return ImGui::ColorEdit3(IMGUI_FORMAT_ID("", COLOR_WHEEL_ID, std::string(a_Label) + "COLOR_WHEEL").c_str(), (float*)&this->m_Value);
				}
			protected:
				T m_Value;
			};

			/// <summary>
			/// A UI view for rendering and interacting with RGBA color values.
			/// Inherits from Vector2View for shared functionality.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec4 or ImColor.</typeparam>
			template <class T>
			class ColorRGBAView : public Vector2View<T>
			{
			public:
				/// <summary>
				/// Constructs a ColorRGBAView that renders an RGB color.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ColorRGBAView(ImGuiWindow& a_Window) : Vector2View<T>(a_Window)
				{ }

				void Render() override { }

				/// <summary>
				/// Renders the RGBA color editor with the specified label.
				/// </summary>
				/// <param name="a_Label">The label for the color editor.</param>
				/// <returns>True if the color was modified, otherwise false.</returns>
				bool Render(const char* a_Label) override
				{
					return ImGui::ColorEdit4(IMGUI_FORMAT_ID("", COLOR_WHEEL_ID, std::string(a_Label) + "COLOR_WHEEL").c_str(), (float*)&this->m_Value);
				}
			};
		}
	}
}

#endif // __EDITOR__