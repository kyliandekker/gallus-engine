#include "core/DataStream.h"

#include <vcruntime_string.h>

#include "core/MemoryInfo.h"

namespace gallus
{
	namespace core
	{
		DataStream::DataStream(void* a_Data, size_t a_Size) : Data(a_Data, a_Size)
		{ }

		DataStream::DataStream(const char* a_Data, size_t a_Size) : Data(a_Data, a_Size)
		{ }

		DataStream::DataStream(size_t a_Size) : Data(a_Size)
		{ }

		DataStream::DataStream(const DataStream& a_Rhs) : Data(a_Rhs)
		{
			m_Pos = a_Rhs.m_Pos;
		}

		DataStream::DataStream(const Data& a_Rhs) : Data(a_Rhs)
		{
			m_Pos = 0;
		}

		DataStream& DataStream::operator=(const DataStream& a_Other)
		{
			if (&a_Other != this)
			{
				Data::operator=(a_Other);
				m_Pos = a_Other.m_Pos;
			}
			return *this;
		}

		void DataStream::Free()
		{
			Data::Free();
			m_Pos = 0;
		}

		bool DataStream::Read(void* a_Data, size_t a_DataSize, size_t a_Size)
		{
			if (m_Pos == m_Size || m_Pos + a_Size > m_Size)
			{
				return false;
			}

			if (a_DataSize < a_Size)
			{
				return false;
			}

			memcpy(a_Data, memory::addAs(m_Data, m_Pos), a_Size);
			Seek(a_Size, SEEK_CUR);
			return true;
		}

		bool DataStream::Write(void const* a_Data, size_t a_Size)
		{
			if (m_Pos == m_Size || m_Pos + a_Size > m_Size)
			{
				return false;
			}

			memcpy(memory::addAs(m_Data, m_Pos), a_Data, a_Size);
			Seek(a_Size, SEEK_CUR);
			return true;
		}

		bool DataStream::Seek(size_t a_Offset, size_t a_Whence)
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
		}

		size_t DataStream::Tell() const
		{
			return m_Pos;
		}
	}
}