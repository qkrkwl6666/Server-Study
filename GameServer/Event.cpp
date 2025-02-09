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
			// 1. ���� ���
			// 2. ���� ���� ����
			// 3. ���� Ǭ��
			// 4. ���� ������ ���� �ٸ� �����忡�� ����
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		cv.notify_one(); // wait���� �����尡 ������ �� 1���� �����

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

		// 1.���� ��´�
		// 2.������ Ȯ���Ѵ�
		// ������ ������ �ڵ� ����
		// ������ Ʋ���� ���� Ǯ�� ��� ����

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
	// Ŀ�� ������Ʈ
	// Usage Count
	// Signal (�Ķ���) / Non-Signal (�Ķ���) << bool
	// Auto / Manual << bool
	//handle = ::CreateEvent(NULL/*���� �Ӽ�*/, FALSE/*bManualReset*/, FALSE/*�ʱ� ��*/, NULL/*�̸�*/);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	//::CloseHandle(handle);
}