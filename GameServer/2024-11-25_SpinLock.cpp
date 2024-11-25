#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>

class SpinLock
{
public:
	void lock()
	{
		// CAS (Compare-And-Swap)

		bool expected = false;
		bool desired = true;

		while (isLock.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;
		}

	}

	void unlock()
	{
		//isLock = false;
		isLock.store(false);
	}
	
private:
	std::atomic<bool> isLock = false;
};

int32 sum = 0;
mutex m;
SpinLock sp;

void Sum()
{
	for (int i = 0; i < 100000; i++)
	{
		//lock_guard<mutex> guard(m);
		lock_guard<SpinLock> guard(sp);
		
		sum++;
	}
}

void Sub()
{
	for (int i = 0; i < 100000; i++)
	{
		//lock_guard<mutex> guard(m);
		lock_guard<SpinLock> guard(sp);
		sum--;
	}
}

int main()
{
	//int a = 1;

	//a = 2;
	//a = 3;
	//a = 4;
	//a = 5;

	std::thread t1(Sum);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	std::cout << sum << std::endl;

	return 0;
}