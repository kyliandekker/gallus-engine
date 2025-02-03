#ifdef __EDITOR__

#include "editor/imgui/views/Selectables/ExplorerResourceViews/TextureExplorerResourceUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/ExplorerResource.h"
#include "editor/imgui/ImGuiDefines.h"
#include "editor/imgui/ImGuiWindow.h"
#include "graphics/dx12/Texture.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			template <class T>
			ExplorerResource* ImageExplorerResourceUIView<T>::GetResource()
			{
				return &m_Resource;
			}

			template <class T>
			void ImageExplorerResourceUIView<T>::RenderSelectable()
			{
				RenderBaseSelectable(&m_Resource);

				ImGui::DisplayHeader(m_Window.GetBoldFont(), m_Window.GetFramePadding(), "Type: ");
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (m_AssetTypeDropdown.Render(IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_INSPECTOR").c_str()))
				{
					m_Resource.SetAssetType(m_AssetTypeDropdown.GetValue());
					m_Resource.SaveMetadata();

					SetData(&m_Resource);
				}
				ImGui::PopStyleVar();

				if (m_Window.GetPreviewTexture())
				{
					ImGui::DisplayHeader(m_Window.GetBoldFont(), m_Window.GetFramePadding(), "Width: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(m_Window.GetPreviewTexture()->GetResourceDesc().Width).c_str());

					ImGui::DisplayHeader(m_Window.GetBoldFont(), m_Window.GetFramePadding(), "Height: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(m_Window.GetPreviewTexture()->GetResourceDesc().Height).c_str());
				}

				EndBaseSelectable();

				RenderPreviewWindow();

				if (m_Window.GetPreviewTexture())
				{
					const float height_new = ImGui::GetContentRegionAvail().y;
					const float width = (m_Window.GetPreviewTexture()->GetResourceDesc().Width * (1.0f / m_Window.GetPreviewTexture()->GetResourceDesc().Height * height_new));

					float offset = (ImGui::GetContentRegionAvail().x - width) / 2;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
					ImVec2 image_pos = ImGui::GetCursorScreenPos();
					ImGui::Image((void*) m_Window.GetPreviewTexture()->GetGPUHandle().ptr, ImVec2(width, height_new));

					// Draw border
					ImDrawList* draw_list = ImGui::GetForegroundDrawList();
					draw_list->AddRect(image_pos, ImVec2(image_pos.x + width, image_pos.y + height_new), ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border])); // White border
				}

				EndPreviewWindow();
			}

			template <class T>
			void ImageExplorerResourceUIView<T>::SetData(ExplorerResource* a_Resource)
			{
				ExplorerResourceWithPreviewUIView::SetData(a_Resource);
				m_AssetTypeDropdown.Initialize(
					m_Resource.GetAssetType(),
					{
						assets::AssetType::Texture,
						assets::AssetType::Sprite,
						assets::AssetType::Font,
					},
					assets::AssetTypeToString
					);
			}

			template<class T>
			ImageExplorerResourceUIView<T>::ImageExplorerResourceUIView(ImGuiWindow& a_Window, T* a_Resource) : ExplorerResourceWithPreviewUIView(a_Window), m_Resource(*a_Resource), m_AssetTypeDropdown(a_Window)
			{}

			template <class T>
			ImageExplorerResourceUIView<T>::ImageExplorerResourceUIView<T>(const ImageExplorerResourceUIView& a_Other) : ExplorerResourceWithPreviewUIView(a_Other), m_Resource(a_Other.m_Resource), m_AssetTypeDropdown(a_Other.m_Window)
			{}

			void TextureExplorerResourceUIView::Select()
			{
				m_Window.SetPreviewTexture(m_Resource.GetPath());
			}

			TextureExplorerResourceUIView::TextureExplorerResourceUIView(ImGuiWindow& a_Window, TextureExplorerResource* a_Resource) : ImageExplorerResourceUIView(a_Window, a_Resource)
			{}

			TextureExplorerResourceUIView::TextureExplorerResourceUIView(const TextureExplorerResourceUIView& a_Other) : ImageExplorerResourceUIView(a_Other)
			{}

			void SpriteExplorerResourceUIView::Select()
			{
				m_Window.SetPreviewTexture(m_Resource.GetPath());
			}

			SpriteExplorerResourceUIView::SpriteExplorerResourceUIView(ImGuiWindow& a_Window, SpriteExplorerResource* a_Resource) : ImageExplorerResourceUIView(a_Window, a_Resource)
			{}

			SpriteExplorerResourceUIView::SpriteExplorerResourceUIView(const SpriteExplorerResourceUIView& a_Other) : ImageExplorerResourceUIView(a_Other)
			{}

			void FontExplorerResourceUIView::Select()
			{
				m_Window.SetPreviewTexture(m_Resource.GetPath());
			}

			FontExplorerResourceUIView::FontExplorerResourceUIView(ImGuiWindow& a_Window, FontExplorerResource* a_Resource) : ImageExplorerResourceUIView(a_Window, a_Resource)
			{}

			FontExplorerResourceUIView::FontExplorerResourceUIView(const FontExplorerResourceUIView& a_Other) : ImageExplorerResourceUIView(a_Other)
			{}
		}
	}
}

#endif // __EDITOR__