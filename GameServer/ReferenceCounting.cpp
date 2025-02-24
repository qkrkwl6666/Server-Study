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
#include "RefCounting.h"

CoreGlobal coreGlobal;

class Wraight : public RefCountable
{
public:
	int _hp = 100;
	int _posX = 0;
	int _posY = 0;
};

class Missile : public RefCountable
{
public:
	void SetTarget(Wraight* target)
	{
		_target = target;
		target->AddRef();
	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_posX;
		int posY = _target->_posY;

		if (_target->_hp <= 0)
		{
			_target->ReleaseRef();
			_target = nullptr;
			return true;
		}

		// TODO : 쫓아간다

		return false;
	}

private:
	Wraight* _target = nullptr;
};

int main()
{
	Wraight* wraight = new Wraight();
	Missile* missile = new Missile();
	missile->SetTarget(wraight);

	// 레이스가 피격 당함
	wraight->_hp = 0;
	//delete wraight;
	wraight->ReleaseRef();
	wraight = nullptr;

	while (true)
	{
		if (missile->Update())
		{
			missile->ReleaseRef();
			missile = nullptr;
		}
	}

	missile->ReleaseRef();
	//delete missile;

	return 0;
}