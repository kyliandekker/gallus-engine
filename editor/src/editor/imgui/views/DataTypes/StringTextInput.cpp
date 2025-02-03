#ifdef _EDITOR

#include "editor/imgui/views/DataTypes/StringTextInput.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "core/Engine.h"
#include "editor/imgui/ImGuiDefines.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			StringTextInput::StringTextInput(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
			{}

			void StringTextInput::Initialize(const std::string& a_InitialValue, size_t a_BufferSize)
			{
				m_Data = core::Data(a_BufferSize);
				SetString(a_InitialValue);
			}

			void StringTextInput::SetString(const std::string& a_String)
			{
				if (a_String.size() == 0)
				{
					return;
				}

				m_Data.Clear();
				strncpy_s(reinterpret_cast<char*>(m_Data.data()), m_Data.size(), a_String.c_str(), a_String.size());
			}

			std::string StringTextInput::GetString()
			{
				return std::string(reinterpret_cast<char*>(m_Data.data()));
			}

			bool StringTextInput::Render(const char* a_Label, ImGuiInputTextFlags flags)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
				bool success = ImGui::InputText(a_Label, reinterpret_cast<char*>(m_Data.data()), m_Data.size(), flags);
				ImGui::PopStyleVar();

				if (ImGui::IsItemHovered())
				{
					// Set the cursor to a text input
					ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
				}
				return success;
			}

			SearchBarInput::SearchBarInput(ImGuiWindow& a_Window) : StringTextInput(a_Window)
			{}

			bool SearchBarInput::Render(const char* a_Label, const ImVec2& a_Size, float a_Padding)
			{
				float x = ImGui::GetCursorPosX();
				float y = ImGui::GetCursorPosY();

				float cursorX = ImGui::GetCursorScreenPos().x;
				float cursorY = ImGui::GetCursorScreenPos().y;

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() * 2.0f, a_Padding));
				ImGui::SetNextItemWidth(a_Size.x);
				bool result = ImGui::InputText(a_Label, reinterpret_cast<char*>(m_Data.data()), m_Data.size());
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (ImGui::IsItemHovered())
				{
					// Set the cursor to a text input
					ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
				}

				ImGui::SetCursorPosX(x + (m_Window.GetFontSize() / 2));
				ImGui::SetCursorPosY(y + a_Padding);
				ImGui::PushFont(m_Window.GetSmallIconFont());
				ImGui::Text(ICON_SEARCH);
				ImGui::PopFont();

				// We manually detect clicks here because overlapping somehow does not work with ImGui.
				ImVec2 headerSize = ImVec2(m_Window.GetFontSize(), a_Size.y - (a_Padding * 3));

				ImVec2 clearButtonMin = ImVec2((cursorX + a_Size.x) - (m_Window.GetFontSize() + a_Padding), cursorY + a_Padding);
				ImVec2 clearButtonMax = ImVec2(clearButtonMin.x + headerSize.x, clearButtonMin.y + headerSize.y);

				ImVec2 mousePos = ImGui::GetMousePos();
				bool isClearButtonHovered = (mousePos.x >= clearButtonMin.x && mousePos.x <= clearButtonMax.x &&
					mousePos.y >= clearButtonMin.y && mousePos.y <= clearButtonMax.y);

				bool isClearButtonClicked = ImGui::IsMouseClicked(0) && isClearButtonHovered;

				if (isClearButtonHovered)
				{
					// Set the cursor to a text input
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}
				if (isClearButtonClicked)
				{
					ImGui::ClearActiveID();
					m_Data[0] = '\0';
					ImGui::SetKeyboardFocusHere(-1);
					result = true;
				}

				ImGui::SetCursorPosX((x + a_Size.x) - (m_Window.GetFontSize() + a_Padding));
				ImGui::SetCursorPosY(y + a_Padding);
				ImVec4 text_hovered = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				text_hovered.w = 0.5f;
				ImGui::PushFont(m_Window.GetSmallIconFont());
				ImGui::TextColored(isClearButtonHovered ? text_hovered : ImGui::GetStyleColorVec4(ImGuiCol_Text), ICON_CLOSE);
				ImGui::PopFont();

				ImGui::SetCursorPosX((x + a_Size.x) - (m_Window.GetFontSize() + a_Padding));
				ImGui::SetCursorPosY(y);
				if (ImGui::InvisibleButton("###ERASE_BUTTON", ImVec2(m_Window.GetFontSize(), a_Size.y)))
				{
				}

				return result;
			}
		}
	}
}

#endif // _EDITOR