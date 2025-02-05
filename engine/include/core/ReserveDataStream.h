#pragma once

#include "core/DataStream.h"

namespace gallus
{
	namespace core
	{
		class ReserveDataStream : public DataStream
		{
		public:
			ReserveDataStream() = default;
			ReserveDataStream(size_t a_Size);
			ReserveDataStream(const ReserveDataStream& rhs);
			ReserveDataStream(const DataStream& rhs);

			ReserveDataStream& operator=(const ReserveDataStream& a_Other);

			void Free() override;

			/// <summary>
			/// Writes data to the data stream.
			/// </summary>
			bool Write(void const* a_Data, size_t a_Size) override;
			bool Seek(size_t a_Offset, size_t a_Whence);
			size_t Tell() const;
		protected:
			void Reallocate(size_t a_ExtraSize);
			size_t m_ReservedSize = 0;
		};
	}
}