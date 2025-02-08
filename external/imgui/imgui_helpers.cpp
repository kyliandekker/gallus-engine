#include "imgui/imgui_helpers.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <imgui/imgui_internal.h>
#include <algorithm>

namespace ImGui
{
	bool CheckboxButton(const char* a_Label, bool* a_pValue, const ImVec2& a_Size, const ImVec4& a_Color)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImVec4 background_color = *a_pValue ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : ImVec4(0, 0, 0, 0);
		ImGui::PushStyleColor(ImGuiCol_Button, background_color);
		bool b = TextButton(a_Label, a_Size, a_Color);
		if (b)
		{
			*a_pValue = !(*a_pValue);
		}
		ImGui::PopStyleColor();

		ImVec2 buttonSize = ImGui::GetItemRectSize();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = *a_pValue ? ImGui::GetColorU32(ImGuiCol_HeaderActive) : ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = *a_pValue ? (ImGui::GetStyle().FrameBorderSize * 2) : ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + buttonSize.x, pos.y + buttonSize.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool TextButton(const char* a_Label, const ImVec2& a_Size, const ImVec4& a_Color)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImGui::PushStyleColor(ImGuiCol_Text, a_Color);
		bool b = ImGui::Button(a_Label, a_Size);
		ImGui::PopStyleColor();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + a_Size.x, pos.y + a_Size.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool IconButton(const char* a_Label, const ImVec2& a_Size, ImFont* a_Font, const ImVec4& a_Color)
	{
		ImGui::PushFont(a_Font);
		bool success = ImGui::TextButton(a_Label, a_Size, a_Color);
		ImGui::PopFont();
		return success;
	}

	bool IconCheckboxButton(const char* a_Label, bool* a_pValue, const ImVec2& a_Size, ImFont* a_Font, const ImVec4& a_Color)
	{
		ImGui::PushFont(a_Font);
		bool success = ImGui::CheckboxButton(a_Label, a_pValue, a_Size, a_Color);
		ImGui::PopFont();
		return success;
	}

	void BeginToolbar(const ImVec2& a_Size)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 header_pos = ImGui::GetCursorScreenPos();
		ImVec2 header_size = a_Size;

		ImVec2 header_min = header_pos;
		ImVec2 header_max = ImVec2(header_pos.x + header_size.x, header_pos.y + header_size.y);

		draw_list->AddRectFilled(header_min, header_max, ImGui::GetColorU32(ImGuiCol_Button), 0);
	}

	void EndToolbar(const ImVec2& a_Padding)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + a_Padding.y);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_Padding.x);
	}

	void DisplayHeader(ImFont* a_BoldFont, const char* a_Label)
	{
		ImGui::PushFont(a_BoldFont);
		ImGui::Text(a_Label);
		ImGui::PopFont();
	}

	bool InvisButton(const char* a_Label, const ImVec2& a_Size, ImU32 a_ButtonColorHover)
	{
		// Get the current draw list (this is where you'll draw your custom button)
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Define the position and size of your button
		ImVec2 button_pos = ImGui::GetCursorScreenPos();
		ImVec2 button_size = ImVec2(a_Size.x, a_Size.y); // Width, Height

		// Calculate the rectangle for the button
		ImVec2 button_min = button_pos;
		ImVec2 button_max = ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y);

		// Detect if the mouse is hovering over the button rectangle
		bool is_hovered = ImGui::IsMouseHoveringRect(button_min, button_max);

		// Detect if the mouse is clicked
		bool is_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_hovered;

		// Set button color based on hover or click state
		ImU32 button_color = is_hovered ? a_ButtonColorHover : IM_COL32(150, 150, 150, 0);

		// Draw the button
		draw_list->AddRectFilled(button_min, button_max, button_color, 0);  // 10.0f is the rounding radius

		return is_clicked;
	}

	std::string IMGUI_FORMAT_ID(const std::string& a_Text, const char* a_ID, const std::string& a_IDName)
	{
		std::string full_id = a_Text + a_ID + a_IDName;
		return full_id;
	}
}