#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "AccountManager.h"
#include "PlayerManager.h"

CoreGlobal coreGlobal;

int main()
{
	GThreadManager->Launch([=]()
		{
			while (true)
			{
				cout << "PlayerThenAccount" << endl;
				GPlayerManager.PlayerThenAccount();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Launch([=]()
		{
			while (true)
			{
				cout << "AccountThenPlayer" << endl;
				GAccountManager.AccountThenPlayer();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Join();

	return 0;
}