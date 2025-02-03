#include "imgui/imgui_helpers.h"

#ifdef __EDITOR__

#include <cmath>
#include <cstdio>
#include <string>
#include <imgui/imgui_internal.h>
#include <editor/imgui/ImguiDefines.h>
#include <algorithm>
#include <editor/imgui/ImGuiWindow.h>

namespace ImGui
{
	ImVec2 operator+(const ImVec2& a, const ImVec2& b)
	{
		return ImVec2(a.x + b.x, a.y + b.y);
	}

	ImVec2 operator-(const ImVec2& a, const ImVec2& b)
	{
		return ImVec2(a.x - b.x, a.y - b.y);
	}

	ImVec2 operator/(const ImVec2& a, const ImVec2& b)
	{
		return ImVec2(a.x / b.x, a.y / b.y);
	}

	ImVec2 operator*(const ImVec2& a, const ImVec2& b)
	{
		return ImVec2(a.x * b.x, a.y * b.y);
	}

	ImVec2& operator+=(ImVec2& a, const ImVec2& b)
	{
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	ImVec2& operator-=(ImVec2& a, const ImVec2& b)
	{
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	ImVec2& operator/=(ImVec2& a, const ImVec2& b)
	{
		a.x /= b.x;
		a.y /= b.y;
		return a;
	}

	ImVec2& operator*=(ImVec2& a, const ImVec2& b)
	{
		a.x *= b.x;
		a.y *= b.y;
		return a;
	}

	bool CheckboxButton(const char* label, bool* p_value, const ImVec2& size_arg)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImVec4 color = *p_value ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : ImVec4(0, 0, 0, 0);
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		bool b = ImGui::Button(label, size_arg);
		if (b)
		{
			*p_value = !(*p_value);
		}
		ImGui::PopStyleColor();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + size_arg.x, pos.y + size_arg.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool TransparentButton(const char* label, const ImVec2& size_arg, ImVec4 color)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f)); // Set button color to white
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f)); // Disable active effect
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.0f)); // Disable active effect

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		bool b = ImGui::Button(label, size_arg);
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + size_arg.x, pos.y + size_arg.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool TransparentCheckboxButton(const char* label, bool* p_value, const ImVec2& size_arg, ImVec4 color)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.0f)); // Disable active effect

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		bool b = ImGui::CheckboxButton(label, p_value, size_arg);
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + size_arg.x, pos.y + size_arg.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool EngineTreeNodeExS(const char* id, const char* icon, const char* label, bool& clicked, bool& right_clicked, bool selected, const ImVec2& size, ImGuiTreeNodeFlags flags)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImVec2 select_min = pos;
		ImVec2 select_max = ImVec2(pos.x + size.x, pos.y + size.y);

		if (selected)
		{
			ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			ImU32 select_color = ImGui::ColorConvertFloat4ToU32(color);

			draw_list->AddRectFilled(select_min, select_max, select_color, 0);
		}

		ImGui::SetNextItemAllowOverlap();
		const bool fold = ImGui::TreeNodeExS(id, size, flags);

		clicked = ImGui::IsItemHovered() && ImGui::IsItemClicked(0);
		right_clicked = ImGui::IsItemHovered() && ImGui::IsItemClicked(1);

		// Adjust icon and label positions after the tree node is drawn
		float iconPos = pos.x + 35;
		ImGui::SetCursorScreenPos(ImVec2(iconPos, pos.y)); // Set icon position using screen coordinates
		ImGui::Text(icon);
		ImGui::SetCursorScreenPos(ImVec2(iconPos + 25, pos.y)); // Set label position
		ImGui::Text(label);

		// Return whether the node is open/closed
		return fold;
	}

	bool EngineResourceNode(const char* id, const char* icon, const char* label, const char* label2, bool& clicked, bool& right_clicked, bool& double_clicked, bool selected)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();

		clicked = ImGui::Selectable(id, &selected);
		right_clicked = ImGui::IsItemHovered() && ImGui::IsItemClicked(1);
		double_clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);

		ImGui::SetCursorScreenPos(pos);
		ImGui::SetCursorScreenPos(ImVec2(pos.x + 10, pos.y));
		ImGui::Text(icon);
		ImGui::SetCursorScreenPos(ImVec2(pos.x + 35, pos.y));
		ImGui::Text(label);

		ImGui::SetCursorScreenPos(ImVec2(pos.x + 300, pos.y));
		ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		textColor.w = 0.5f;
		ImGui::TextColored(textColor, label2);

		return clicked;
	}

	void BeginToolbar(const ImVec2& a_Size)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 header_pos = ImGui::GetCursorScreenPos();
		ImVec2 header_size = a_Size;

		ImVec2 header_min = header_pos;
		ImVec2 header_max = ImVec2(header_pos.x + header_size.x, header_pos.y + header_size.y);

		draw_list->AddRectFilled(header_min, header_max, ImGui::ImColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Button)), 0);
	}

	void EndToolbar(const ImVec2& a_Padding)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + a_Padding.y);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_Padding.x);

		ImGui::PopStyleVar();
	}

	void DisplayHeader(ImFont* boldFont, const ImVec2& padding, const char* label)
	{
		ImGui::PushFont(boldFont);
		ImGui::Text(label);
		ImGui::PopFont();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding.x);
	}

	bool InvisButton(const char* label, const ImVec2& size_arg, ImU32 button_color_hover)
	{
		// Get the current draw list (this is where you'll draw your custom button)
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Define the position and size of your button
		ImVec2 button_pos = ImGui::GetCursorScreenPos();
		ImVec2 button_size = ImVec2(size_arg.x, size_arg.y); // Width, Height

		// Calculate the rectangle for the button
		ImVec2 button_min = button_pos;
		ImVec2 button_max = ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y);

		// Detect if the mouse is hovering over the button rectangle
		bool is_hovered = ImGui::IsMouseHoveringRect(button_min, button_max);

		// Detect if the mouse is clicked
		bool is_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_hovered;

		// Set button color based on hover or click state
		ImU32 button_color = is_hovered ? button_color_hover : IM_COL32(150, 150, 150, 0);

		// Draw the button
		draw_list->AddRectFilled(button_min, button_max, button_color, 0);  // 10.0f is the rounding radius

		return is_clicked;
	}

	void CheckboxText(const char* label, bool* p_value, const std::string& checked_text, const std::string& unchecked_text, const ImVec2& size_arg)
	{
		ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		ImVec4 color_inactive = ImVec4(0, 0, 0, 0);
		bool b = InvisButton(label, size_arg, IM_COL32(0, 0, 0, 0));
		std::string test = (*p_value ? checked_text : unchecked_text);
		bool temp = false;
		ImGui::Text(test.c_str());
		if (b)
		{
			*p_value = !(*p_value);
		}
	}

	bool IconButton(const char* label, const ImVec2& size_arg, ImFont* font, ImVec4 color)
	{
		ImGui::PushFont(font);
		bool success = ImGui::TransparentButton(label, size_arg, color);
		ImGui::PopFont();
		return success;
	}

	bool IconCheckboxButton(const char* label, bool* p_value, const ImVec2& size_arg, ImFont* font, ImVec4 color)
	{
		ImGui::PushFont(font);
		bool success = ImGui::TransparentCheckboxButton(label, p_value, size_arg, color);
		ImGui::PopFont();
		return success;
	}
}

#endif // __EDITOR__