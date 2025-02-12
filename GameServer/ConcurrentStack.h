#pragma once

#include "pch.h"
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

		// ��� ���� ���� 1 �� 2�� �����尡 ���� ���ÿ� TryPop�� �����ϸ� 
		// ���ʿ��� oldNode�� �����ϸ� �ٸ� ���� �̹� ������ oldNode�� �����ؼ� ���� �߻� ���� delete�� 2�� �� �� ����
		//delete oldNode;
		return true;
	}

	void TryDelete(Node* oldNode)
	{
		// ���� Pop�� �������� �����尡 1���� üũ �� ���� ���������� üũ
		if (_popCount == 1)
		{
			// �� ȥ�� �� ����
			// �̿� ȥ���̸�, ���� ����� �ٸ� ������ �鵵 ���� �غ���
			Node* node = _pendingList.exchange(nullptr);
			
			// �߰��� ����� �ְ� ���� -> ���� ����
			if (--_popCount == 0)
			{
				DeleteNodes(node);
			}
			else if(node)
			{
				// ���� ���� ������� �ٽ� ���� ����
				ChainPendingNodeList(node);
			}

			delete oldNode;
		}
		else
		{
			// ���� �̹� ������ ���� ���� ���� ���� ���ุ
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
	std::atomic<int32> _popCount; // pop�� �������� ������ ����
	std::atomic<Node*> _pendingList; // ���� �Ǿ�� �� ���� (ù��° ���)
};