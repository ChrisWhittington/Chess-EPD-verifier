#pragma once

#define TRUE 1
#define FALSE 0

// if using Visual Studio C Compiler ...
#define VSTUDIO TRUE

#define ONE 1ULL

#if 0
#define u64		uint64_t
#define u32		uint32_t
#define u16		uint16_t
#define u8		uint8_t

#define int64	int64_t
#define int32	int32_t
#define int16	int16_t
#define int8	int8_t
#else

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
#endif


#if VSTUDIO
inline int popcount64(u64 x)
{
	return ((int)(__popcnt64(x)));
}
//
inline int scanforward64(u64 x)
{
	assert(x);
	unsigned long idx;
	_BitScanForward64(&idx, x);
	return (int)idx;
}
//
inline int scanbackward64(u64 x)
{
	assert(x);
	return ((int)__lzcnt64(x));
}

#else
// extremely slow and non machine specific popcount (use your own if you want)
// from computer chess wiki
inline int popcount64(u64 bb)
{
	int count = 0;
	for (int i = 0; i < 64; i++, bb >>= 1)
		count += (int)bb & 1;
	return count;
}

// doesn't like being given a zero bb
inline int scanforward64(u64 bb)
{
	for (int i = 0; i < 64; i++)
	{
		if (bb & ONE)
			return i;
		bb = bb >> 1;
	}
	printf("Don't like bb=0, error\n");
	return 0;
}
#endif

#define swap(a,b) (a = a ^ b, b = a ^ b, a = a ^ b)
#define swapcontents(a,b) (*a = *a ^ *b, *b = *a ^ *b, *a = *a ^ *b)


inline int minint(int a, int b)
{
	if (a < b) return a;
	return b;
}

inline int maxint(int a, int b)
{
	if (a > b) return a;
	return b;
}


