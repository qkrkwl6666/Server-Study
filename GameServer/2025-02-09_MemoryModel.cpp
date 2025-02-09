#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>

atomic<bool> flag;

int main()
{
	flag = false;

	//flag.store(false);
	//flag.store(false, memory_order_seq_cst);

	//bool val = flag;
	bool val = flag.load(memory_order_seq_cst);

	// ���� flag ���� �ְ� flag ���� ����
	{
		bool prev = flag.exchange(true);

		//bool prev = flag;
		//flag = true;
	}

	// CAS ���Ǻ� ����

	bool expected = false;
	bool desired = true;
	flag.compare_exchange_strong(expected, desired);

	if (flag == expected)
	{
		flag = desired;
		return true;
	}
	else
	{
		expected = flag;
		return false;
	}

	return 0;
}