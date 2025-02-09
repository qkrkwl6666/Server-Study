#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>

mutex m;
queue<int32> q;
HANDLE handle;
condition_variable cv;

void Producer()
{
	while (true)
	{
		{
			// 1. 락을 잡고
			// 2. 공유 변수 수정
			// 3. 락을 푼다
			// 4. 조건 변수를 통해 다른 쓰레드에게 통지
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		cv.notify_one(); // wait중인 쓰레드가 있으면 딱 1개를 깨운다

		//::SetEvent(handle);

		//this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	while (true)
	{
		//::WaitForSingleObject(handle, INFINITE);

		unique_lock<mutex> lock(m);

		cv.wait(lock, []()
			{
				return q.empty() == false;
			});

		// 1.락을 잡는다
		// 2.조건을 확인한다
		// 조건이 맞으면 코드 진행
		// 조건이 틀리면 락을 풀고 대기 상태

		//if (q.empty() == false)
		{
			int data = q.front();
			q.pop();
			cout << q.size() << endl;
		}
	}
}

int main()
{
	// 커널 오브젝트
	// Usage Count
	// Signal (파란불) / Non-Signal (파란불) << bool
	// Auto / Manual << bool
	//handle = ::CreateEvent(NULL/*보안 속성*/, FALSE/*bManualReset*/, FALSE/*초기 값*/, NULL/*이름*/);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	//::CloseHandle(handle);
}