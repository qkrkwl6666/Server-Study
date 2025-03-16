#include "pch.h"
#include "Allocator.h"

void* BaseAllocator::Alloc(int32 size)
{
    return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
    ::free(ptr);
}

void* StompAllocator::Alloc(int32 size)
{
    const int64 pageCount = (PAGE_SIZE + size - 1) / PAGE_SIZE;
    const int64 dataOffset = PAGE_SIZE * pageCount - size;

    void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    return static_cast<void*>
        (static_cast<int8*>(baseAddress) + dataOffset);


}

void StompAllocator::Release(void* ptr)
{
    int64 address = reinterpret_cast<int64>(ptr);
    int64 baseAddress = address - (address % PAGE_SIZE);
    ::VirtualFree(reinterpret_cast<void*>
        (baseAddress), 0, MEM_RELEASE);
}
