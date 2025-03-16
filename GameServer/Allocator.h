#pragma once

class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void *ptr);
};

//  메모리 오염 확인
/// StompAllocator

class StompAllocator
{
	enum
	{
		PAGE_SIZE = 0x1000,
	};
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};