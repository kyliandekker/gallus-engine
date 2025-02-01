#ifdef __EDITOR__

#pragma once

#include "editor/imgui/views/Selectables/ExplorerResourceViews/ExplorerResourceWithPreviewUIView.h"
#include "editor/imgui/views/DataTypes/StringDropdown.h"

namespace renegade
{
	namespace editor
	{
		class ExplorerResource;
		class AudioExplorerResource;
		class SoundExplorerResource;
		class SongExplorerResource;
		class VOExplorerResource;

		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Represents a UI view for a generic audio resource in the explorer, with support for preview and selection.
			/// </summary>
			/// <typeparam name="T">The specific type of audio resource (e.g., Sound, Song, VO).</typeparam>
			template <class T>
			class AudioExplorerResourceUIView : public ExplorerResourceWithPreviewUIView
			{
			public:
				void RenderIconGrid(float a_Width, float a_Height) override;

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
				/// <param name="a_Resource">Pointer to the specific audio resource.</param>
				AudioExplorerResourceUIView(const ImGuiWindow& a_Window, T* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				AudioExplorerResourceUIView(const AudioExplorerResourceUIView& a_Other);

				T& m_Resource; /// Reference to the associated resource of type <typeparamref name="T"/>.

				StringDropdown<assets::AssetType> m_AssetTypeDropdown; /// Dropdown UI element for selecting asset types.

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Specialized view for sound resources.
			/// </summary>
			class SoundExplorerResourceUIView : public AudioExplorerResourceUIView<SoundExplorerResource>
			{
			protected:
				/// <summary>
				/// Constructor for the sound resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the sound resource.</param>
				SoundExplorerResourceUIView(const ImGuiWindow& a_Window, SoundExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				SoundExplorerResourceUIView(const SoundExplorerResourceUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Specialized view for song resources.
			/// </summary>
			class SongExplorerResourceUIView : public AudioExplorerResourceUIView<SongExplorerResource>
			{
			protected:
				/// <summary>
				/// Constructor for the sound resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the song resource.</param>
				SongExplorerResourceUIView(const ImGuiWindow& a_Window, SongExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				SongExplorerResourceUIView(const SongExplorerResourceUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};

			/// <summary>
			/// Specialized view for VO (voice over) resources.
			/// </summary>
			class VOExplorerResourceUIView : public AudioExplorerResourceUIView<VOExplorerResource>
			{
			protected:
				/// <summary>
				/// Constructor for the vo resource view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Resource">Pointer to the vo resource.</param>
				VOExplorerResourceUIView(const ImGuiWindow& a_Window, VOExplorerResource* a_Resource);

				/// <summary>
				/// Copy constructor.
				/// </summary>
				/// <param name="a_Other">The other instance to copy from.</param>
				VOExplorerResourceUIView(const VOExplorerResourceUIView& a_Other);

				friend ExplorerResourceUIView; /// Grants access to private/protected members for <see cref="ExplorerResourceUIView"/>.
			};
		}
	}
}

#endif // __EDITOR__