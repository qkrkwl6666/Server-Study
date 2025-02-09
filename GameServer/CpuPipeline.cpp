#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>

// 가시성, 코드 재배치
int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;

volatile bool ready;

void Thread_1()
{
	while (!ready)
		;

	y = 1; // Store y
	r1 = x; // Load y
}

void Thread_2()
{
	while (!ready)
		;

	x = 1;
	r2 = y;
}

int main()
{
	int count = 0;

	while (true)
	{
		ready = false;
		count++;
		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}

	std::cout << count << "만에 빠져 나옴" << endl;

	return 0;
}