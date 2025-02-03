#pragma once

#ifdef _EDITOR

#include "editor/imgui/views/ImGuiUIView.h"

#include <imgui/imgui.h>
#include <string>

#include "editor/imgui/ImGuiDefines.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// A UI view for rendering and interacting with vectors.
			/// Can be used for both glm-based types and ImVec types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec2 or ImVec.</typeparam>
			template <class T>
			class Vector2View : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a Vector2View that renders a vector.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				Vector2View(ImGuiWindow& a_Window) : ImGuiUIView(a_Window), m_Value(0)
				{ }

				/// <summary>
				/// Sets the value of the 2D vector. Updates the value if it's different from the current one.
				/// </summary>
				/// <param name="a_Value">The new vector value to set.</param>
				void SetValue(const T& a_Value)
				{
					if (m_Value != a_Value)
					{
						m_Value = a_Value;
					}
				}

				/// <summary>
				/// Retrieves the current vector value.
				/// </summary>
				/// <returns>The current vector value.</returns>
				const T& GetValue() const
				{
					return m_Value;
				}

				void Render() override { }

				/// <summary>
				/// Renders the 2D vector as dragable float inputs in ImGui (X and Y components).
				/// </summary>
				/// <param name="a_Label">The label displayed next to the input fields.</param>
				virtual bool Render(const char* a_Label)
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("X");
					ImGui::SameLine();
					bool changedValue = ImGui::DragFloat(IMGUI_FORMAT_ID("", INPUT_ID, std::string("X_") + a_Label).c_str(), &m_Value.x);
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SameLine();
					changedValue |= ImGui::DragFloat(IMGUI_FORMAT_ID("", INPUT_ID, std::string("Y_") + a_Label).c_str(), &m_Value.y);

					return changedValue;
				}
			protected:
				T m_Value; /// The current value of the vector.
			};

			/// <summary>
			/// A UI view for rendering and interacting with vectors.
			/// Can be used for both glm-based types and ImVec types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec3 or ImVec.</typeparam>
			template <class T>
			class Vector3View : public Vector2View<T>
			{
			public:
				/// <summary>
				/// Constructor for Vector3View.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				Vector3View(ImGuiWindow& a_Window) : Vector2View<T>(a_Window)
				{ }

				void Render() override { }

				/// <summary>
				/// Renders the 3D vector as dragable float inputs in ImGui (X, Y and Z components).
				/// </summary>
				/// <param name="a_Label">The label displayed next to the input fields.</param>
				bool Render(const char* a_Label) override
				{
					bool changedValue = Vector2View<T>::Render(a_Label);
					ImGui::SameLine();
					ImGui::Text("Z");
					ImGui::SameLine();
					changedValue |= ImGui::DragFloat(IMGUI_FORMAT_ID("", INPUT_ID, std::string("Z_") + a_Label).c_str(), &this->m_Value.z);

					return changedValue;
				}
			};
		}
	}
}

#endif // _EDITOR