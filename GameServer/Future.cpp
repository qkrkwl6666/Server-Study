#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <future>

int64 Calculate()
{
	int64 sum = 0;

	for (int i = 0; i < 100'0000; i++)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<string> &&p)
{
	p.set_value("S M");
}

void TaskWorker(std::packaged_task<int64(void)>&& t)
{
	t();
}

int main()
{
	// ���� ����
	int64 sum = Calculate();
	cout << sum << endl;

	
	// �ٵ� �ϳ��� �Լ��� ����� �� �����带 ���� �Լ��� �Ҵ��ϴ°� ����
	// future�� ���� async�� ����ϸ� �ڵ����� �����带 �Ҵ��Ѵ�
	// future 3���� ���
	// 1) deferred -> ���� ���� �ϴ� ���� �ϰ� ���߿� ����
	// 2) anysc -> ������ �����带 ���� ����
	
	// std::future
	{
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// std::future_status status = future.wait_for(1ms); // 1ms ���� ��� �� ���� ��ȯ
		// if (status == future_status::ready)
		// {
		// 
		// }

		//future.wait(); // ������� �ʿ��ϴ� �Ϸ� �ɶ� ���� ���

		int64 sum = future.get(); // ������� �ʿ��ϴ� �Ϸ� �ɶ� ���� ���
	}

	/* ��� �Լ��ε� ����� ����
	{
		class Knight
		{
		public:
			int64 GetHp()
			{
				return 100;
			}
		};

		Knight kn;

		std::future<int64> future = std::async(std::launch::async, &Knight::GetHp, kn); // kn.GetHp();
	}
	*/

	// std::promise

	{
		std::promise<string> promise;
		std::future<string> future = promise.get_future();

		thread t(PromiseWorker, std::move(promise));

		t.join();

		string m = future.get();
		std::cout << m << std::endl;
	}

	// std::packged_task
	{
		std::packaged_task<int64(void)> task(Calculate);
		std::future<int64> future = task.get_future();

		thread t(TaskWorker, std::move(task));
	}

	// ���
	// mutex, cv ���� ���� �ʰ� �ܼ��� 
	// �ѹ��� ���� �̺�Ʈ�� �����ϰ� ����� �����ϴ�
	// 1) async
	// ���ϴ� �Լ��� �񵿱�� ���� 
	// 2) promise
	// ������� promise�� ���� future�� �޴´�.
	// 3) packaged_task 
	// ���ϴ� �Լ��� ����� packaged_task�� ���� future�� �޴´�

	return 0;
}