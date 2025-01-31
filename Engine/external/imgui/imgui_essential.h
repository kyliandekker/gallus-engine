struct ImVec2
{
	float                                   x, y;
	constexpr ImVec2() : x(0.0f), y(0.0f) { }
	constexpr ImVec2(float _x, float _y) : x(_x), y(_y) { }
	float& operator[] (size_t idx) { IM_ASSERT(idx == 0 || idx == 1); return ((float*)(void*)(char*)this)[idx]; } // We very rarely use this [] operator, so the assert overhead is fine.
	float  operator[] (size_t idx) const { IM_ASSERT(idx == 0 || idx == 1); return ((const float*)(const void*)(const char*)this)[idx]; }
#ifdef IM_VEC2_CLASS_EXTRA
	IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// ImVec4: 4D vector used to store clipping rectangles, colors etc. [Compile-time configurable type]
struct ImVec4
{
	float                                                     x, y, z, w;
	constexpr ImVec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	constexpr ImVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
#ifdef IM_VEC4_CLASS_EXTRA
	IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};