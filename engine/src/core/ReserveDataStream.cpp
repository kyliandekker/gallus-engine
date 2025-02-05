#include "core/ReserveDataStream.h"

#include <vcruntime_string.h>
#include <corecrt_malloc.h>

#include "core/MemoryInfo.h"

namespace gallus
{
	namespace core
	{
		ReserveDataStream::ReserveDataStream(size_t a_Size) : DataStream(a_Size), m_ReservedSize(a_Size)
		{}

		ReserveDataStream::ReserveDataStream(const ReserveDataStream& rhs) : DataStream(rhs)
		{
			m_ReservedSize = rhs.m_ReservedSize;
		}

		ReserveDataStream::ReserveDataStream(const DataStream& rhs) : DataStream(rhs)
		{
			m_ReservedSize = rhs.size();
		}

		ReserveDataStream& ReserveDataStream::operator=(const ReserveDataStream& a_Other)
		{
			if (&a_Other != this)
			{
				DataStream::operator=(a_Other);
				m_ReservedSize = a_Other.m_ReservedSize;
			}
			return *this;
		}

		void ReserveDataStream::Free()
		{
			DataStream::Free();
			m_ReservedSize = 0;
		}

		bool ReserveDataStream::Write(void const* a_Data, size_t a_Size)
		{
			size_t size = m_Size;
			if (m_Pos == m_ReservedSize || m_Pos + a_Size > m_ReservedSize)
			{
				Reallocate(a_Size);
			}
			if (m_Pos == size || m_Pos + a_Size > size)
			{
				m_Size += a_Size;
			}

			memcpy(memory::add(m_Data, m_Pos), a_Data, a_Size);
			Seek(a_Size, SEEK_CUR);
			return true;
		}

		bool ReserveDataStream::Seek(size_t a_Offset, size_t a_Whence)
		{
			switch (a_Whence)
			{
				case SEEK_SET:
				{
					m_Pos = a_Offset;
					return true;
				}
				case SEEK_CUR:
				{
					m_Pos += a_Offset;
					return true;
				}
				case SEEK_END:
				default:
				{
					m_Pos = m_Size - a_Offset;
					return true;
				}
			}
			return false;
		}

		void ReserveDataStream::Reallocate(size_t a_ExtraSize)
		{
			size_t newReservedSize = m_Size + (a_ExtraSize * 2000);
			void* newData = malloc(newReservedSize);
			memcpy(newData, m_Data, m_Size);

			if (m_Data)
			{
				free(m_Data);
			}
			m_ReservedSize = newReservedSize;
			m_Data = newData;
		}

		size_t ReserveDataStream::Tell() const
		{
			return m_Pos;
		}
	}
}