#pragma once

#include <cstdint>

namespace coopscoop
{
	inline void* add(void* a_Ptr, size_t a_Size)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(a_Ptr) + a_Size);
	}
}