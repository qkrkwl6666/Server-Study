#pragma once

#include <mutex>
template<typename T>
class LockStack
{
public:
	LockStack()
	{}

	LockStack(const LockStack&) = delete; // ���� ������ ����
	LockStack& operator=(const LockStack&) = delete; // ���� ������ ����

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_cv.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		value = std::move(_stack.top());
		_stack.pop();

		return true;
	}

	/*
	������ ���� ���� �ϸ� stack�� pop�� �������� Ȯ���ϴ°� ���� ���� ������ Ȱ���ؼ�
	Push �Ҷ� _cv.notify_one(); ȣ�� �ؼ�
	_stack.empty() == false ���� �� _stack�� �����Ͱ� �ִ��� üũ �ϰ� Pop����
	*/

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_cv.wait(lock, [this]()
			{
				_stack.empty() == false;
			});

		value = std::move(_stack.top());
		_stack.pop();
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _cv;
};
