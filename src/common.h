#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define func static
#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;

typedef size_t usize;
typedef ssize_t ssize;

#endif //COMMON_H
