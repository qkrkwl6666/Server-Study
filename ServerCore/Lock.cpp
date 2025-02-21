#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	// 재귀적으로 호출 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) 
		>> 16;
	if (lockThreadId == LThreadId)
	{
		_writeCount++;
		return;
	}

	// 아무도 Write하지 않고 Read 하고 있지 않을 때
	// 경합해서 소유권을 얻는다

	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);

	while (true)
	{
		for (int spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG; // 예상 값
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LockTimeOut");
		}

		this_thread::yield();
	}
}

void Lock::WriteUnLock()
{
	// ReadLock 풀기전에는 WriteLock 풀기 불가능
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
	{
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
	{
		_lockFlag.store(EMPTY_FLAG);
	}
}

void Lock::ReadLock()
{
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공

	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK)
		>> 16;
	if (lockThreadId == LThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// 아무도 소유하고 있지 않을때 경합 해서 공유 카운트 올린다
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);

			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
			{
				return;
			}

			
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LockTimeOut");
		}

		this_thread::yield();
	}
}

void Lock::ReadUnLock()
{
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE_UNLOCK");
	}
}
