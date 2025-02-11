#pragma once

#include <mutex>
template<typename T>
class LockStack
{
public:
	LockStack()
	{}

	LockStack(const LockStack&) = delete; // 복사 생성자 제거
	LockStack& operator=(const LockStack&) = delete; // 복사 연산자 제거

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
	무작정 무한 루프 하며 stack가 pop이 가능한지 확인하는것 보다 조건 변수를 활용해서
	Push 할때 _cv.notify_one(); 호출 해서
	_stack.empty() == false 인지 즉 _stack의 데이터가 있는지 체크 하고 Pop실행
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
