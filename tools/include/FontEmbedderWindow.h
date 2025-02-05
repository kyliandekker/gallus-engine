#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class FontEmbedderWindow
{
public:
	void Render();
	void GenerateFontFiles(const fs::path& folder);
private:
	bool m_Enabled = true;
};