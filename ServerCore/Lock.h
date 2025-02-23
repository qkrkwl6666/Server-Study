#pragma once

#include "Types.h"

// RW SpinLock

/*

[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
*/

// W -> R (O)
// R -> W (X)

class Lock
{
public:
	enum : int32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000,
	};

	void WriteLock(const char* name);
	void WriteUnLock(const char* name);
	void ReadLock(const char* name);
	void ReadUnLock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;

};

// LockGuard

class ReadLockGuard
{
public :
	ReadLockGuard(Lock& lock, const char* name) 
		: _lock(lock), _name(name)
	{
		_lock.ReadLock(_name);
	}

	~ReadLockGuard()
	{
		_lock.ReadUnLock(_name);
	}

private:
	const char* _name;
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) 
		: _lock(lock), _name(name)
	{
		_lock.WriteLock(_name);
	}

	~WriteLockGuard()
	{
		_lock.WriteUnLock(_name);
	}

private:
	const char* _name;
	Lock& _lock;
};
