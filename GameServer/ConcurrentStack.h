#pragma once

#include "pch.h"
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

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(value)
		{

		}

		T data;
		Node* next;
	};

public:
	// [ ][ ][ ][ ][ ][ ]
	// [head]
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		/*
		if (_head == node->next)
		{
			_head = node;
			return true;
		}
		else
		{
			node->next = _head;
			return false;
		}
		*/
		
		while (_head.compare_exchange_weak(node->next, node) == false)
		{
			//node->next = _head;
		}
		
	}

	// [ ][ ][ ][ ][ ][ ]
	// [head]
	bool TryPop(T& value)
	{
		_popCount++;

		Node* oldNode = _head;

		/*
		if (_head == oldNode)
		{
			_head = oldNode->next;
			return true;
		}
		else
		{
			oldNode = _head;
			return false;
		}
		*/


		while (oldNode && _head.compare_exchange_weak(oldNode, oldNode->next) == false)
		{

		}

		if (oldNode == nullptr)
		{
			_popCount--;
			return false;
		}

		value = oldNode->data;
		TryDelete(oldNode);

		// 잠시 삭제 보류 1 번 2번 쓰레드가 거의 동시에 TryPop를 실행하면 
		// 한쪽에서 oldNode를 삭제하면 다른 노드는 이미 삭제한 oldNode에 접근해서 문제 발생 또한 delete를 2번 할 수 있음
		//delete oldNode;
		return true;
	}

	void TryDelete(Node* oldNode)
	{
		// 현재 Pop을 진행중인 쓰레드가 1인지 체크 즉 나만 진행중인지 체크
		if (_popCount == 1)
		{
			// 나 혼자 면 삭제
			// 이왕 혼자이면, 삭제 예약된 다른 데이터 들도 삭제 해보자
			Node* node = _pendingList.exchange(nullptr);
			
			// 중간에 끼어든 애가 없음 -> 삭제 진행
			if (--_popCount == 0)
			{
				DeleteNodes(node);
			}
			else if(node)
			{
				// 누가 끼어 들었으니 다시 갖다 놓자
				ChainPendingNodeList(node);
			}

			delete oldNode;
		}
		else
		{
			// 누가 이미 있으면 삭제 하지 말고 삭제 예약만
			ChainPendingNode(oldNode);
			_popCount--;
		}
	}

	//          last	_pendingList
	// [ ][ ][ ][ ]  -> [ ][ ][ ][ ][ ][ ][ ]
	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{

		}
	}

	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)
		{
			last = last->next;
		}

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	std::atomic<Node*> _head;
	std::atomic<int32> _popCount; // pop을 실행중인 쓰레드 개수
	std::atomic<Node*> _pendingList; // 삭제 되어야 할 노드들 (첫번째 노드)
};