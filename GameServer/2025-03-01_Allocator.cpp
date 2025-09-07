#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"

class Knight
{
public:
	Knight()
	{
		cout << "Knight" << endl;
	}
	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight" << endl;
	}

private:
	int32 _hp;
	int32 _attack;
};

// int main()
// {
// 	Knight* knight = xnew<Knight>(100);
// 
// 	xdelete(knight);
// 
// 	return 0;
// }