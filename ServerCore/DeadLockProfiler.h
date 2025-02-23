#pragma once
#include <stack>
#include <map>
#include <vector>
#include <set>

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32> _nameToId;
	unordered_map<int32, const char*> _idToName;
	stack<int32> _lockStack;
	map<int32, set<int32>> _lockHistory;

	Mutex _mutex;

private:
	vector<int32> _discoverOrder; // 노드 기록 순서
	int32 _discoveredCount = 0; // 노드가 발견된 순서
	vector<bool> _finished; // Dfs(i) 종료 여부
	vector<int32> _parent;
};

