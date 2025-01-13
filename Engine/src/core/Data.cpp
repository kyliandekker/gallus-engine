#include "core/Data.h"

#include <corecrt_malloc.h>
#include <stdio.h>
#include <cassert>
#include <vcruntime_string.h>
#include <string>

namespace coopscoop
{
	namespace core
	{
		Data::Data(const void* a_Data, size_t a_Size) : Data(reinterpret_cast<const char*>(a_Data), a_Size)
		{ }

		Data::Data(const char* a_Data, size_t a_Size) : m_Size(a_Size)
		{
			assert(a_Size > 0);
			m_Data = malloc(a_Size);
			if (!m_Data)
			{
				return;
			}
			memcpy(m_Data, a_Data, a_Size);
		}

		Data::Data(size_t a_Size) : m_Size(a_Size)
		{
			assert(a_Size > 0);
			m_Data = malloc(a_Size);
			if (m_Data)
			{
				memset(m_Data, 0, a_Size);
			}
		}

		Data::Data(const Data& a_Rhs)
		{
			m_Size = a_Rhs.m_Size;
			m_Data = malloc(m_Size);
			if (m_Data)
			{
				memcpy(m_Data, a_Rhs.m_Data, m_Size);
			}
		}

		Data::~Data()
		{
			if (m_Data)
			{
				free(m_Data);
			}
		}

		Data& Data::operator=(const Data& a_Other)
		{
			if (&a_Other != this)
			{
				if (m_Data)
				{
					free(m_Data);
				}
				m_Size = a_Other.m_Size;
				m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
				if (m_Data)
				{
					memcpy(m_Data, a_Other.m_Data, m_Size);
				}
			}
			return *this;
		}

		void Data::Free()
		{
			if (m_Data)
			{
				free(m_Data);
				m_Size = 0;
			}
		}

		void Data::Clear()
		{
			memset(reinterpret_cast<char*>(m_Data), 0, m_Size);
		}
	}
}