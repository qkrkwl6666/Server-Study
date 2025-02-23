#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_mutex);

	// ���̵� ã�ų� �߱��Ѵ�.
	int32 lockId = 0;
	auto findit = _nameToId.find(name);

	if (findit == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else
	{
		lockId = findit->second;
	}

	// ����ִ� ���� �־��ٸ�
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽� ��� ����� üũ
		const int32 prevId = _lockStack.top();
		if (prevId !=lockId)
		{
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);


}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_mutex);

	if (_lockStack.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	int32 lockId = _nameToId[name];
	if (lockId != _lockStack.top())
	{
		CRASH("INVALID_UNLOCK");
	}

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoverOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int lockId = 0; lockId < lockCount; lockId++)
	{
		Dfs(lockId);
	}
	
	// ���� ������ ����
	_discoverOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoverOrder[here] != -1)
		return;

	_discoverOrder[here] = _discoveredCount++;

	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;

	for (int32 there : nextSet)
	{
		// ���� �湮�� ���� ������
		if (_discoverOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here �� there���� ���� �߰� �Ǿ����� there�� here�� �ļ� (������ ����)
		if (_discoverOrder[here] < _discoverOrder[there])
			continue;

		// �������� �ƴϰ� Dfs(there)�� ���� ���� X ��� there�� here�� ���� �̴�
		// (������ ����)

		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}
			CRASH("DEADLOCK_DETECTED");
		}
	}


}
