#pragma once

#include "editor/imgui/views/selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"
#include "editor/imgui/views/DataTypes/StringDropdown.h"

namespace renegade
{
	namespace editor
	{
		class ExplorerResource;
		class ImageExplorerResource;
		class TextureExplorerResource;
		class SpriteExplorerResource;
		class FontExplorerResource;

		namespace imgui
		{
			class ImGuiWindow;

			// TODO: This class is basically the same as AudioExplorerResourceUIView. Maybe make another parent class for both?

			/// <summary>
			/// Represents a UI view for a generic image resource in the explorer, with support for preview and selection.
			/// </summary>
			/// <typeparam name="T">The specific type of audio resource (e.g., Sound, Song, VO).</typeparam>
			template <class T>
			class ImageExplorerResourceUIView : public ExplorerResourceWithPreviewUIView
			{
			public:
				/// <summary>
				/// Retrieves the associated resource for this view.
				/// </summary>
				/// <returns>A pointer to the associated <see cref="ExplorerResource"/>.</returns>
				ExplorerResource* GetResource() override;

				/// <summary>
				/// Renders the selectable UI element for this resource.
				/// </summary>
				void RenderSelectable() override;
			protected:
				/// <summary>
				/// Sets the data for this resource view.
				/// </summary>
				/// <param name="a_Resource">Pointer to the resource to associate with this view.</param>
				void SetData(ExplorerResource* a_Resource) override;

				/// <summary>
				/// Constructor for the audio resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the specific image resource.</param>
				ImageExplorerResourceUIView(const ImGuiWindow& a_Window, T* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				ImageExplorerResourceUIView(const ImageExplorerResourceUIView& a_Other);

				T& m_Resource; /// Reference to the associated resource of type <typeparamref name="T"/>.

				StringDropdown<assets::AssetType> m_AssetTypeDropdown; /// Dropdown UI element for selecting asset types.

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Specialized view for texture resources.
			/// </summary>
			class TextureExplorerResourceUIView : public ImageExplorerResourceUIView<TextureExplorerResource>
			{
			protected:
				/// <summary>
				/// Constructor for the texture resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the texture resource.</param>
				TextureExplorerResourceUIView(const ImGuiWindow& a_Window, TextureExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				TextureExplorerResourceUIView(const TextureExplorerResourceUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Specialized view for sprite resources.
			/// </summary>
			class SpriteExplorerResourceUIView : public ImageExplorerResourceUIView<SpriteExplorerResource>
			{
			protected:
				/// <summary>
				/// Constructor for the sprite resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the sprite resource.</param>
				SpriteExplorerResourceUIView(const ImGuiWindow& a_Window, SpriteExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				SpriteExplorerResourceUIView(const SpriteExplorerResourceUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Specialized view for font resources.
			/// </summary>
			class FontExplorerResourceUIView : public ImageExplorerResourceUIView<FontExplorerResource>
			{
			protected:
				/// <summary>
				/// Constructor for the font resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the font resource.</param>
				FontExplorerResourceUIView(const ImGuiWindow& a_Window, FontExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				FontExplorerResourceUIView(const FontExplorerResourceUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};
		}
	}
}