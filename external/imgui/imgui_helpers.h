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

	bool CheckboxButton(const char* a_Label, bool* a_pValue, const ImVec2& a_Size = ImVec2(0, 0), const ImVec4& a_Color = ImVec4(1, 1, 1, 1));
	bool TextButton(const char* a_Label, const ImVec2& a_Size = ImVec2(0, 0), const ImVec4& a_Color = ImVec4(1, 1, 1, 1));

	bool IconButton(const char* a_Label, const ImVec2& a_Size, ImFont* a_Font, const ImVec4& a_Color = ImGui::GetStyle().Colors[ImGuiCol_Text]);
	bool IconCheckboxButton(const char* a_Label, bool* a_pValue, const ImVec2& a_Size, ImFont* a_Font, const ImVec4& a_Color = ImGui::GetStyle().Colors[ImGuiCol_Text]);

	void BeginToolbar(const ImVec2& a_Size);
	void EndToolbar(const ImVec2& a_Padding);
	void DisplayHeader(ImFont* a_BoldFont, const char* a_Label);

	/// <summary>
	/// Converts colors ranging 0-255 to 0-1.
	/// </summary>
	/// <param name="a_R">Red.</param>
	/// <param name="a_G">Green.</param>
	/// <param name="a_B">Blue.</param>
	/// <param name="a_A">Alpha.</param>
	/// <returns></returns>
	inline ImVec4 ConvertColorsRgba(float a_R, float a_G, float a_B, float a_A)
	{
		return ImVec4(a_R / 255.0f, a_G / 255.0f, a_B / 255.0f, a_A / 255.0f);
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