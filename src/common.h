#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define UNUSED(x) ((void)(x))
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define OFFSET_OF(struc, member) ((size_t)((char *)&((struc *)0)->member))

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;

typedef size_t usize;

typedef float f32;
typedef double f64;

#define PVEC2_FMT "{%4.2f, %4.2f}"
#define PVEC2_SPREAD(v) (v)[0],(v)[1]
#define PVEC2(v) printf(PVEC2_FMT"\n", PVEC2_SPREAD(v))

#endif // COMMON_H
