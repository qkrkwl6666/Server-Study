#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>

thread_local int32 LThreadId = 0;

void ThreadMain(int32 threadId)
{
	LThreadId = threadId;

	while (true)
	{
		cout << "Hi I am Thread " << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;

	for (int i = 0; i < 10; i++)
	{
		int32 threadId = i + 1;
		threads.push_back(std::thread(ThreadMain, threadId));
	}

	for (thread& t : threads)
	{
		t.join();
	}

	return 0;
}