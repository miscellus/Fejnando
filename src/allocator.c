#include "allocator.h"
#include <stdlib.h>

void *DefaultAllocator(enum AllocOp op, usize newSize, usize oldSize, void *oldPtr, void *user)
{
    UNUSED(oldSize);
    UNUSED(user);

    switch (op)
    {
    case AllocOp_Alloc:
        return malloc(newSize);
    case AllocOp_Free:
        return (free(oldPtr), NULL);
    case AllocOp_Realloc:
        return realloc(oldPtr, newSize);
    case AllocOp_ZeroAlloc:
        return calloc(1, newSize);
    }

    return NULL;
}
