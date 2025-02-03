#ifdef _EDITOR

#pragma once

#include "editor/ExplorerResource.h"

namespace coopscoop
{
	namespace editor
	{
		/// <summary>
		/// Base class for audio-related resources in the editor, handling common functionality
		/// for managing audio data such as left and right audio samples.
		/// </summary>
		class AudioExplorerResource : public ExplorerResource
		{
		public:
			/// <summary>
			/// Destructor for cleaning up resources associated with the audio explorer resource.
			/// </summary>
			virtual ~AudioExplorerResource() override;

			/// <summary>
			/// Initializes the audio resource, preparing the left and right audio samples.
			/// </summary>
			/// <returns>True if initialization is successful, otherwise false.</returns>
			bool Initialize() override;

			double* m_LeftSamples = nullptr; /// Pointer to the left audio channel samples.
			double* m_RightSamples = nullptr; /// Pointer to the right audio channel samples.
			size_t m_NumSamples = 0; /// The number of audio samples in the resource.
		};

		/// <summary>
		/// Represents a sound audio resource in the editor, inheriting from AudioExplorerResource.
		/// </summary>
		class SoundExplorerResource : public AudioExplorerResource
		{
		public:
		};

		/// <summary>
		/// Represents a song audio resource in the editor, inheriting from AudioExplorerResource.
		/// </summary>
		class SongExplorerResource : public AudioExplorerResource
		{
		public:
		};

		/// <summary>
		/// Represents a voice over (VO) audio resource in the editor, inheriting from AudioExplorerResource.
		/// </summary>
		class VOExplorerResource : public AudioExplorerResource
		{
		public:
		};
	}
}

#endif // _EDITOR