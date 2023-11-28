#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

enum AllocOp
{
    AllocOp_Alloc = 0,
    AllocOp_Free = 1,
    AllocOp_Realloc = 2,
    AllocOp_ZeroAlloc = 3,
};

typedef void *(Allocator)(enum AllocOp operation, usize newSize, usize oldSize, void *oldPtr, void *user);

#define MemAlloc(allocator, size) ((allocator)(AllocOp_Alloc, (size), 0, 0, 0))
#define MemFree(allocator, ptr) ((allocator)(AllocOp_Free, 0, 0, (ptr), 0))
#define MemRealloc(allocator, size, oldSize, oldPtr) ((allocator)(AllocOp_Realloc, (size), (oldSize), (oldPtr), 0))
#define MemZeroAlloc(allocator, size) ((allocator)(AllocOp_ZeroAlloc, (size), 0, 0, 0))

#define MemAllocUser(allocator, size, user) ((allocator)(AllocOp_Alloc, (size), 0, 0, (user)))
#define MemFreeUser(allocator, ptr, user) ((allocator)(AllocOp_Free, 0, 0, (ptr), (user)))
#define MemReallocUser(allocator, size, oldSize, oldPtr, user) ((allocator)(AllocOp_Realloc, (size), (oldSize), (oldPtr), (user)))
#define MemZeroAllocUser(allocator, size, user) ((allocator)(AllocOp_ZeroAlloc, (size), 0, 0, (user)))

Allocator DefaultAllocator;
Allocator ArenaAllocator;

#endif /* ifndef MEMORY_H */
