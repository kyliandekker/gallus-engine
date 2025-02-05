#include "FontEmbedderWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <vector>
#include <windows.h>
#include <ShlObj_core.h>
#include <core/FileUtils.h>
#include <core/ReserveDataStream.h>
#include <core/logger/Logger.h>

using namespace gallus;

struct FontPair
{
	std::string name;
	fs::path path;
	bool isIconFont = false;
	bool groupWithAbove = false;
	fs::path iconInfoPath;
};

std::vector<FontPair> m_Fonts;
std::string m_FontName, m_FontPath, m_IconInfoPath;


bool genericFileOpen(std::string& path, const IID rclsid, FILEOPENDIALOGOPTIONS options, const std::vector<COMDLG_FILTERSPEC>& filters = {})
{
	CoInitialize(nullptr);

	IFileDialog* pfd;
	HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	pfd->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
	if (SUCCEEDED(hr))
	{
		DWORD dwOptions;
		if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
		{
			pfd->SetOptions(dwOptions | options);
		}

		if (SUCCEEDED(pfd->Show(NULL)))
		{
			IShellItem* psi;
			if (SUCCEEDED(pfd->GetResult(&psi)))
			{
				LPWSTR pszPath;
				psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszPath);

				const std::wstring wide(pszPath);
				path = std::string(wide.begin(), wide.end());

				psi->Release();
				CoTaskMemFree(pszPath);

				return true;
			}
		}
		pfd->Release();
	}
	return false;
}

bool PickContainer(std::string& a_Path)
{
	return genericFileOpen(a_Path, CLSID_FileOpenDialog, FOS_PICKFOLDERS);
}

bool PickFile(std::string& a_Path)
{
	return genericFileOpen(a_Path, CLSID_FileOpenDialog, 0);
}

bool PickFont(std::string& a_Path)
{
	return genericFileOpen(a_Path, CLSID_FileOpenDialog, 0, {
		{ L"Font Files (*.ttf, *.otf)", L"*.otf;*.ttf" }
		});
}

bool PickCSS(std::string& a_Path)
{
	return genericFileOpen(a_Path, CLSID_FileOpenDialog, 0, {
		{ L"CSS Files (*.css)", L"*.css" }
		});
}

bool SaveFile(std::string& a_Path, int* a_Choice, const std::vector<COMDLG_FILTERSPEC>& a_Filters)
{
	return genericFileOpen(a_Path, CLSID_FileSaveDialog, 0, a_Filters);
}

void FontEmbedderWindow::Render()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin(
		ImGui::IMGUI_FORMAT_ID("FONT_EMBEDDER", WINDOW_ID, "FONT_EMBEDDER").c_str(),
		&m_Enabled,
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar
	);

	char fontName[256];
	memset(fontName, 0, IM_ARRAYSIZE(fontName));
	memcpy(fontName, m_FontName.c_str(), m_FontName.size());

	char fontPath[256];
	memset(fontPath, 0, IM_ARRAYSIZE(fontPath));
	memcpy(fontPath, m_FontPath.c_str(), m_FontPath.size());

	char iconInfoPath[256];
	memset(iconInfoPath, 0, IM_ARRAYSIZE(iconInfoPath));
	memcpy(iconInfoPath, m_IconInfoPath.c_str(), m_IconInfoPath.size());

	if (ImGui::BeginChild(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "SCROLL_BOX").c_str(), ImVec2(), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (ImGui::InputText(ImGui::IMGUI_FORMAT_ID("Font Name", INPUT_ID, "FONT_NAME").c_str(), fontName, sizeof(fontName)))
		{
			m_FontName = fontName;
		}

		if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("Pick Font", BUTTON_ID, "PATH_PICK").c_str()))
		{
			PickFont(m_FontPath);
		}

		ImGui::SameLine();

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		if (ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "FONT_PATH").c_str(), fontPath, sizeof(fontPath)))
		{

		}
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		if (m_FontName.empty() || m_FontPath.empty())
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("ADD FONT", BUTTON_ID, "ADD FONT").c_str()))
		{
			m_Fonts.push_back({ m_FontName, m_FontPath });
		}

		if (m_FontName.empty() || m_FontPath.empty())
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		for (size_t i = 0; i < m_Fonts.size(); i++)
		{
			FontPair& fontPair = m_Fonts[i];

			ImGui::Indent();
			if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("-", BUTTON_ID, std::string("REMOVE_FONT_" + std::to_string(i))).c_str(), ImVec2(25, 25)))
			{
				m_Fonts.erase(m_Fonts.begin() + i);
			}
			ImGui::Text("\"%s\"", fontPair.name.c_str());
			ImGui::Text("%s", fontPair.path.stem().generic_string().c_str());
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			ImGui::Checkbox(ImGui::IMGUI_FORMAT_ID("Group with Above", CHECKBOX_ID, "GROUP_WITH_ABOVE_" + std::to_string(i)).c_str(), &fontPair.groupWithAbove);
			ImGui::Checkbox(ImGui::IMGUI_FORMAT_ID("Icon Font", CHECKBOX_ID, "ICON_FONT_" + std::to_string(i)).c_str(), &fontPair.isIconFont);
			if (fontPair.isIconFont)
			{
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("Pick CSS", BUTTON_ID, "PATH_PICK_CSS_" + std::to_string(i)).c_str()))
				{
					if (PickCSS(m_IconInfoPath))
					{
						fontPair.iconInfoPath = m_IconInfoPath;
					}
				}

				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				if (ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "FONT_ICON_INFO_PATH_" + std::to_string(i)).c_str(), iconInfoPath, sizeof(iconInfoPath)))
				{

				}
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			ImGui::PopStyleVar();
			ImGui::Unindent();

			ImGui::Separator();
		}

		if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("Generate", BUTTON_ID, "Generate Font File").c_str()))
		{
			std::string folder;
			if (PickContainer(folder))
			{
				GenerateFontFiles(folder);
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void WriteToData(const std::string& a_Text, gallus::core::ReserveDataStream& a_Output)
{
	a_Output.Write(a_Text.c_str(), a_Text.size());
}

std::string toMacroName(const std::string& className)
{
	std::string macroName = className;
	std::replace(macroName.begin(), macroName.end(), '-', '_'); // Replace '-' with '_'
	std::transform(macroName.begin(), macroName.end(), macroName.begin(), ::toupper); // Uppercase
	return macroName;
}

std::string unicodeToUTF8(const std::string& unicodeValue)
{
	unsigned int codePoint = std::stoul(unicodeValue, nullptr, 16); // Parse hex value
	std::string utf8;

	if (codePoint <= 0x7F)
	{
// 1-byte UTF-8
		utf8 += static_cast<char>(codePoint);
	}
	else if (codePoint <= 0x7FF)
	{
// 2-byte UTF-8
		utf8 += static_cast<char>(0xC0 | ((codePoint >> 6) & 0x1F));
		utf8 += static_cast<char>(0x80 | (codePoint & 0x3F));
	}
	else if (codePoint <= 0xFFFF)
	{
// 3-byte UTF-8
		utf8 += static_cast<char>(0xE0 | ((codePoint >> 12) & 0x0F));
		utf8 += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
		utf8 += static_cast<char>(0x80 | (codePoint & 0x3F));
	}
	else if (codePoint <= 0x10FFFF)
	{
// 4-byte UTF-8
		utf8 += static_cast<char>(0xF0 | ((codePoint >> 18) & 0x07));
		utf8 += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
		utf8 += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
		utf8 += static_cast<char>(0x80 | (codePoint & 0x3F));
	}

	return utf8;
}

std::string formatAsCString(const std::string& utf8)
{
	std::string formatted = "\""; // Start with opening quote
	for (unsigned char c : utf8)
	{
		char buf[5];
		snprintf(buf, sizeof(buf), "\\x%02X", c);
		formatted += buf; // Append \xNN for each byte
	}
	formatted += "\""; // End with closing quote
	return formatted;
}

void WriteCSSInfo(const FontPair& a_FontPair, gallus::core::ReserveDataStream& a_Output)
{
	FILE* cssFile = nullptr;
	fopen_s(&cssFile, a_FontPair.iconInfoPath.generic_string().c_str(), "rb");
	if (!cssFile)
	{
		LOG(LOGSEVERITY_ERROR, "FONT_EMBEDDER", "Could not open css file: \"%s\"", fontPair.iconInfoPath.generic_string().c_str());
		return;
	}

	char buffer[1024]; // Buffer for reading lines
	while (fgets(buffer, sizeof(buffer), cssFile))
	{
		std::string line(buffer);

		// Look for lines starting with ".icon-" and containing ":before"
		size_t iconPos = line.find(".icon-");
		size_t beforePos = line.find(":before");

		if (iconPos != std::string::npos && beforePos != std::string::npos)
		{
			char contentBuffer[1024]; // Buffer for reading lines
			while (fgets(contentBuffer, sizeof(contentBuffer), cssFile))
			{
				std::string contentLine(contentBuffer);

				size_t contentPos = contentLine.find("content:");
				if (contentPos != std::string::npos)
				{
					// Extract class name
					size_t classNameStart = iconPos + 6; // Skip ".icon-"
					size_t classNameEnd = line.find(":before", classNameStart);
					std::string className = line.substr(classNameStart, classNameEnd - classNameStart);

					// Extract Unicode value
					size_t unicodeStart = contentLine.find("\"\\", contentPos) + 2; // Skip "content: \"\"
					size_t unicodeEnd = contentLine.find("\"", unicodeStart);
					std::string unicodeValue = contentLine.substr(unicodeStart, unicodeEnd - unicodeStart);

					// Convert class name to macro name
					std::string macroName = toMacroName("ICON_" + className);

					std::string utf8 = unicodeToUTF8(unicodeValue);
					std::string formattedCString = formatAsCString(utf8);

					WriteToData("constexpr char " + macroName + "[] = \"" + formattedCString + "\";\n", a_Output);

					break;
				}
			}
		}
	}
}

char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
void WriteFont(const FontPair& a_FontPair, gallus::core::ReserveDataStream& a_Output)
{
	gallus::core::DataStream fontData;
	if (!gallus::file::FileLoader::LoadFile(a_FontPair.path, fontData))
	{
		LOG(LOGSEVERITY_ERROR, "FONT_EMBEDDER", "Could not open font file: \"%s\"", fontPair.path.generic_string().c_str());
		return;
	}

	if (a_FontPair.isIconFont)
	{
		WriteCSSInfo(a_FontPair, a_Output);
		WriteToData("\n", a_Output);
	}

	WriteToData("\tunsigned char ", a_Output);
	WriteToData(a_FontPair.name.c_str(), a_Output);
	WriteToData("[", a_Output);
	WriteToData(std::to_string(fontData.size()), a_Output);
	WriteToData("] = {\n\t\t", a_Output);

	for (size_t i = 0; i < fontData.size(); i++)
	{
		unsigned char byte = fontData[i];

		std::string hex_code = "0x";
		hex_code += hex_chars[(byte & 0xF0) >> 4];
		hex_code += hex_chars[(byte & 0x0F) >> 0];

		WriteToData(hex_code, a_Output);
		if (i < fontData.size() - 1)
		{
			WriteToData(",", a_Output);
		}
	}

	WriteToData("\n\t};\n", a_Output);
}

void FontEmbedderWindow::GenerateFontFiles(const fs::path& folder)
{
	for (size_t i = 0; i < m_Fonts.size(); i++)
	{
		FontPair& fontPair = m_Fonts[i];

		gallus::core::ReserveDataStream output;

		WriteToData("#pragma once\n\n", output);
		WriteToData("namespace font\n", output);
		WriteToData("{\n", output);

		WriteFont(fontPair, output);
		for (size_t j = i + 1; j < m_Fonts.size(); j++)
		{
			FontPair& fontPair2 = m_Fonts[j];
			if (fontPair2.groupWithAbove)
			{
				WriteFont(fontPair2, output);
				i++;
			}
			else
			{
				break;
			}
		}

		WriteToData("}\n", output);

		fs::path path = folder.generic_string() + "/" + fontPair.name + ".h";
		if (!gallus::file::FileLoader::SaveFile(path, output))
		{
			LOG(LOGSEVERITY_ERROR, "FONT_EMBEDDER", "Could not save file: \"%s\"", path.generic_string().c_str());
			break;
		}
	}
}