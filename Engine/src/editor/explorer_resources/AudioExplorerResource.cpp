#ifdef __EDITOR__

#include "editor/explorer_resources/AudioExplorerResource.h"

//#include <uaudio_wave_reader/WaveReader.h>
//#include <uaudio_wave_reader/WaveChunks.h>
//#include <uaudio_wave_reader/ChunkCollection.h>

//#include "core/datatypes/DataStream.h"
//#include "logger/Logger.h"

constexpr int MAX_SAMPLES = 70560;

namespace coopscoop
{
	namespace editor
	{
		//double* ToSample(unsigned char* data, size_t buffersize, uint16_t bitsPerSample, uint16_t blockAlign, uint16_t, uint16_t audioFormat, size_t numSamples, bool left)
		//{
		//	if (data == nullptr)
		//	{
		//		return nullptr;
		//	}

		//	unsigned char* pData = data;
		//	size_t realNumSamples = buffersize / blockAlign;

		//	if (realNumSamples == 0)
		//	{
		//		return nullptr;
		//	}

		//	size_t div = realNumSamples / numSamples;

		//	double* samples = reinterpret_cast<double*>(malloc(numSamples * sizeof(double)));
		//	if (!samples)
		//	{
		//		return nullptr;
		//	}

		//	if (!left)
		//	{
		//		pData += blockAlign / 2;
		//	}

		//	for (size_t i = 0; i < numSamples; i++)
		//	{
		//		if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16)
		//		{
		//			int16_t sample = *(int16_t*)pData;
		//			samples[i] = static_cast<double>(sample) / INT16_MAX;
		//		}

		//		pData += div * blockAlign;
		//	}

		//	return samples;
		//}

		AudioExplorerResource::~AudioExplorerResource()
		{
			//if (m_LeftSamples)
			//{
			//	free(m_LeftSamples);
			//}
			//if (m_RightSamples)
			//{
			//	free(m_RightSamples);
			//}
		}

		bool AudioExplorerResource::Initialize()
		{
			//size_t size = 0;
			//uaudio::wave_reader::WaveReader::FTell(m_Path.c_str(), size);

			//core::Data data(size);
			//uaudio::wave_reader::ChunkCollection chunkCollection(data.data(), size);

			//uaudio::wave_reader::WaveReader::LoadWave(m_Path.c_str(), chunkCollection);

			//uaudio::wave_reader::FMT_Chunk fmt_chunk;
			//uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = chunkCollection.GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);

			//if (UAUDIOWAVEREADERFAILED(result))
			//{
			//	return false;
			//}

			//if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16)
			//{
			//	LOG(LOGSEVERITY_ERROR, "TODO", "Failed loading wave file: Cannot have a higher bits per sample than 16.");
			//	return false;
			//}

			//uaudio::wave_reader::DATA_Chunk data_chunk;
			//uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result2 = chunkCollection.GetChunkFromData<uaudio::wave_reader::DATA_Chunk>(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);

			//if (UAUDIOWAVEREADERFAILED(result))
			//{
			//	return false;
			//}

			//uint32_t data_chunk_size = 0;
			//chunkCollection.GetChunkSize(data_chunk_size, uaudio::wave_reader::DATA_CHUNK_ID);

			//m_LeftSamples = ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, MAX_SAMPLES, true);
			//m_RightSamples = ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, MAX_SAMPLES, false);
			//m_NumSamples = data_chunk_size / fmt_chunk.blockAlign;
			//if (m_NumSamples > MAX_SAMPLES)
			//{
			//	m_NumSamples = MAX_SAMPLES;
			//}

			return true;
		}
	}
}

#endif // __EDITOR__