#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_mutex);

	// 아이디를 찾거나 발급한다.
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

	// 잡고있는 락이 있었다면
	if (_lockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스 라면 데드락 체크
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
	
	// 연산 끝나면 정리
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
		// 아직 방문한 적이 없으면
		if (_discoverOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here 가 there보다 먼저 발견 되었으면 there는 here의 후손 (순방향 간선)
		if (_discoverOrder[here] < _discoverOrder[there])
			continue;

		// 순방향이 아니고 Dfs(there)가 아직 종료 X 라면 there는 here의 선조 이다
		// (역방향 간선)

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
