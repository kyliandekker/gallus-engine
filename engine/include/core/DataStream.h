#pragma once

#include "core/Data.h"

#include <cstdint>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

namespace gallus
{
	namespace core
	{
		/// <summary>
		/// Represents a generic container for raw data, providing utility functions and automatic cleanup
		/// for managing and interacting with the stored data. Allows the user to move the offset for reading and writing data at specific points.
		/// </summary>
		class DataStream : public Data
		{
		public:
			DataStream() = default;

			/// <summary>
			/// Constructs a DataStream object with the specified raw data and size.
			/// </summary>
			/// <param name="a_Data">Pointer to the raw data.</param>
			/// <param name="a_Size">Size of the data in bytes.</param>
			DataStream(void* a_Data, size_t a_Size);

			/// <summary>
			/// Constructs a DataStream object with the specified character data and size.
			/// </summary>
			/// <param name="a_Data">Pointer to the character data.</param>
			/// <param name="a_Size">Size of the data in bytes.</param>
			DataStream(const char* a_Data, size_t a_Size);

			/// <summary>
			/// Constructs a DataStream object with a specified size, allocating memory.
			/// </summary>
			/// <param name="a_Size">Size of the data in bytes.</param>
			DataStream(size_t a_Size);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The DataStream object to copy from.</param>
			DataStream(const DataStream& a_Rhs);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The DataStream object to copy from.</param>
			DataStream(const Data& a_Rhs);

			/// <summary>
			/// Copy assignment operator.
			/// </summary>
			/// <param name="a_Other">The DataStream object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			DataStream& operator=(const DataStream& a_Other);

			/// <summary>
			/// Frees the allocated memory for the data.
			/// </summary>
			void Free() override;

			/// <summary>
			/// Stores the data from the DataStream in a memory stream provided by the user.
			/// </summary>
			bool Read(void* a_Data, size_t a_DataSize, size_t a_Size);

			/// <summary>
			/// Writes data to the data stream.
			/// </summary>
			virtual bool Write(void const* a_Data, size_t a_Size);

			/// <summary>
			/// Adjusts the current position within the data buffer based on the specified offset and origin.
			/// </summary>
			/// <param name="a_Offset">The offset, in bytes, to move the current position.</param>
			/// <param name="a_Whence">
			/// The reference point for the offset. Possible values:
			/// - SEEK_SET/0: Beginning of the data.
			/// - SEEK_CUR/1: Current position.
			/// - SEEK_END/2: End of the data.
			/// </param>
			/// <returns>True if the seek operation is successful, otherwise false.</returns>
			bool Seek(size_t a_Offset, size_t a_Whence);

			/// <summary>
			/// Retrieves the current position within the data buffer.
			/// </summary>
			/// <returns>The current position, in bytes, within the data buffer.</returns>
			size_t Tell() const;
		protected:
			size_t m_Pos = 0; /// The current position within the data buffer.
		};
	}
}