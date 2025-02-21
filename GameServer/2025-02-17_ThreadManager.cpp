#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

//mutex m;
//
//list<int32> GetRewardList(int questId)
//{
//	list<int32> item = TODO;
//
//	return item;
//}

CoreGlobal Core;

void ThreadMain()
{
	while (true)
	{
		cout << "Hello I am ThreadId : " 
			<< LThreadId << endl;

		this_thread::sleep_for(1s);
	}
}

int main()
{
	//CRASH("Test");
	//int a = 3;
	//ASSERT_CRASH(a != 3);

	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}
	GThreadManager->Join();
	return 0;
}