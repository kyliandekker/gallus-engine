#pragma once

#include <imgui/imgui.h>

#include <stdint.h>
#include <string>
#include <map>

namespace ImGui
{
#define POPUP_WINDOW_ID "###POPUP_"
#define POPUP_WINDOW_BUTTON_ID "###POPUP_DIALOG_SAVE_BUTTON_"
#define CHILD_ID "###CHILD_"
#define BUTTON_ID "###BUTTON_"
#define COMBO_ID "###COMBO_"
#define WINDOW_ID "###WINDOW_"
#define MENU_ITEM_ID "###MENU_ITEM_"
#define TREE_NODE_ID "###TREE_NODE_"
#define MENU_ID "###MENU_"
#define INPUT_ID "###INPUT_"
#define CHECKBOX_ID "###CHECKBOX_"
#define FOLD_ID "###CHECKBOX_"
#define DRAG_ITEM_ID "###DRAG_ITEM_"
#define SELECTABLE_ID "###SELECTABLE_"
#define FOLDOUT_ID "###FOLDOUT_"
#define COLOR_WHEEL_ID "###COLOR_WHEEL_"

	ImVec2 operator+(const ImVec2& a, const ImVec2& b);
	ImVec2 operator-(const ImVec2& a, const ImVec2& b);
	ImVec2 operator/(const ImVec2& a, const ImVec2& b);
	ImVec2 operator*(const ImVec2& a, const ImVec2& b);

	ImVec2 operator+=(const ImVec2& a, const ImVec2& b);
	ImVec2 operator-=(const ImVec2& a, const ImVec2& b);
	ImVec2 operator/=(const ImVec2& a, const ImVec2& b);
	ImVec2 operator*=(const ImVec2& a, const ImVec2& b);

	bool CheckboxButton(const char* label, bool* p_value, const ImVec2& size_arg = ImVec2(0, 0));
	bool TransparentButton(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImVec4 color = ImVec4(1, 1, 1, 1));
	bool TransparentCheckboxButton(const char* label, bool* p_value, const ImVec2& size_arg = ImVec2(0, 0), ImVec4 color = ImVec4(1, 1, 1, 1));
	//bool HierarchyItem(const char *label, const ImVec2 &size_arg = ImVec2(0, 0), bool selected = false, ImU32 button_color_hover = IM_COL32(200, 200, 200, 100));
	bool EngineTreeNodeExS(const char* id, const char* icon, const char* label, bool& clicked, bool& right_clicked, bool selected, const ImVec2& size, ImGuiTreeNodeFlags flags);
	bool EngineResourceNode(const char* id, const char* icon, const char* label, const char* label2, bool& clicked, bool& right_clicked, bool& double_clicked, bool selected);
	void BeginToolbar(const ImVec2& a_Size);
	void EndToolbar(const ImVec2& a_Padding);
	void DisplayHeader(ImFont* boldFont, const char* label);
	void CheckboxText(const char* label, bool* p_value, const std::string& checked_text, const std::string& unchecked_text, const ImVec2& size_arg);
	bool IconButton(const char* label, const ImVec2& size_arg, ImFont* font, ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_Text]);
	bool IconCheckboxButton(const char* label, bool* p_value, const ImVec2& size_arg, ImFont* font, ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_Text]);

	inline ImU32 ImColorConvertFloat4ToU32(const ImVec4& in)
	{
		ImU32 out;
		out = ((int) (in.x * 255.0f)) << IM_COL32_R_SHIFT;  // Red
		out |= ((int) (in.y * 255.0f)) << IM_COL32_G_SHIFT;  // Green
		out |= ((int) (in.z * 255.0f)) << IM_COL32_B_SHIFT;  // Blue
		out |= ((int) (in.w * 255.0f)) << IM_COL32_A_SHIFT;  // Alpha
		return out;
	}

	inline ImVec4 ConvertColorsRgba(float r, float g, float b, float a)
	{
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	/// <summary>
	/// Formats a string to combine a base text, an identifier, and a name, which can be used 
	/// for ImGui widgets to ensure unique IDs and descriptive labels.
	/// </summary>
	/// <param name="a_Text">The base text to display.</param>
	/// <param name="a_ID">A unique identifier for the widget, typically used for ImGui ID scoping.</param>
	/// <param name="a_IDName">An additional name to append, adding context or detail.</param>
	/// <returns>
	/// A formatted string combining the base text, ID, and name in a manner suitable for ImGui.
	/// </returns>
	std::string IMGUI_FORMAT_ID(const std::string& a_Text, const char* a_ID, const std::string& a_IDName);
}