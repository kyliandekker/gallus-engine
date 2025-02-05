#pragma once

#include <cstdint>
#include <string>

namespace gallus
{
	namespace core
	{
		/// <summary>
		/// Represents a generic container for raw data, providing utility functions and automatic cleanup
		/// for managing and interacting with the stored data.
		/// </summary>
		class Data
		{
		public:
			Data() = default;

			/// <summary>
			/// Constructs a Data object with the specified raw data and size.
			/// </summary>
			/// <param name="a_Data">Pointer to the raw data.</param>
			/// <param name="a_Size">Size of the data in bytes.</param>
			Data(const void* a_Data, size_t a_Size);

			/// <summary>
			/// Constructs a Data object with the specified character data and size.
			/// </summary>
			/// <param name="a_Data">Pointer to the character data.</param>
			/// <param name="a_Size">Size of the data in bytes.</param>
			Data(const char* a_Data, size_t a_Size);

			/// <summary>
			/// Constructs a Data object with a specified size, allocating memory.
			/// </summary>
			/// <param name="a_Size">Size of the data in bytes.</param>
			Data(size_t a_Size);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The Data object to copy from.</param>
			Data(const Data& a_Rhs);

			/// <summary>
			/// Destructor to free allocated memory.
			/// </summary>
			virtual ~Data();

			/// <summary>
			/// Copy assignment operator.
			/// </summary>
			/// <param name="a_Other">The Data object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			Data& operator=(const Data& a_Other);

			/// <summary>
			/// Retrieves the size of the stored data in bytes.
			/// </summary>
			/// <returns>The size of the data.</returns>
			size_t size() const
			{
				return m_Size;
			}

			/// <summary>
			/// Checks whether the Data object is empty (contains no data).
			/// </summary>
			/// <returns>True if empty, otherwise false.</returns>
			bool empty() const
			{
				return m_Data == nullptr && m_Size == 0;
			}

			/// <summary>
			/// Provides a raw pointer to the stored data.
			/// </summary>
			/// <returns>A void pointer to the data.</returns>
			void* data() const
			{
				return m_Data;
			}

			/// <summary>
			/// Provides a typed pointer to the stored data.
			/// </summary>
			/// <typeparam name="T">The desired type of the pointer.</typeparam>
			/// <returns>A pointer to the data cast to type T.</returns>
			template <typename T>
			T* dataAs() const
			{
				return reinterpret_cast<T*>(m_Data);
			}

			/// <summary>
			/// Frees the allocated memory for the data.
			/// </summary>
			virtual void Free();

			/// <summary>
			/// Clears the data but keeps the size the same.
			/// </summary>
			virtual void Clear();

			/// <summary>
			/// Accesses the byte at the specified index (read-only).
			/// </summary>
			/// <param name="a_Index">The index of the byte to access.</param>
			/// <returns>The byte at the specified index.</returns>
			unsigned char operator [] (size_t a_Index) const { return reinterpret_cast<unsigned char*>(m_Data)[a_Index]; }

			/// <summary>
			/// Accesses the byte at the specified index (modifiable).
			/// </summary>
			/// <param name="a_Index">The index of the byte to access.</param>
			/// <returns>A reference to the byte at the specified index.</returns>
			unsigned char& operator [] (size_t a_Index) { return reinterpret_cast<unsigned char*>(m_Data)[a_Index]; }
		protected:
			void* m_Data = nullptr; /// Pointer to the raw data.
			size_t m_Size = 0;      /// Size of the data in bytes.
		};
	}
}