#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

class TestLock
{
	USE_LOCK;
	//Lock _locks[1]
public:
	int64 TestRead()
	{
		READ_LOCK;

		if (_queue.empty() == true)
			return -1;

		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK;

		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK;

		if(_queue.empty() == false)
			_queue.pop();
	}

private:
	queue<int64> _queue;
};

CoreGlobal coreGlobal;
TestLock testLock;

void ThreadWrite()
{
	while (true)
	{	
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value << endl;

		this_thread::sleep_for(1ms);
	}
}

int main()
{
	for (int i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}
	
	GThreadManager->Join();
}