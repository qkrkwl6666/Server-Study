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
	// 동기 실행
	int64 sum = Calculate();
	cout << sum << endl;

	
	// 근데 하나의 함수에 사용할 때 쓰레드를 만들어서 함수를 할당하는것 보다
	// future를 만들어서 async로 사용하면 자동으로 쓰레드를 할당한다
	// future 3가지 방법
	// 1) deferred -> 지연 실행 일단 생성 하고 나중에 실행
	// 2) anysc -> 별도의 쓰레드를 만들어서 실행
	
	// std::future
	{
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// std::future_status status = future.wait_for(1ms); // 1ms 동한 대기 후 상태 반환
		// if (status == future_status::ready)
		// {
		// 
		// }

		//future.wait(); // 결과물이 필요하다 완료 될때 까지 대기

		int64 sum = future.get(); // 결과물이 필요하다 완료 될때 까지 대기
	}

	/* 멤버 함수로도 사용이 가능
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

	// 결론
	// mutex, cv 까지 가지 않고 단순한 
	// 한번만 쓰는 이벤트에 유용하게 사용이 가능하다
	// 1) async
	// 원하는 함수를 비동기로 실행 
	// 2) promise
	// 결과물을 promise를 통해 future로 받는다.
	// 3) packaged_task 
	// 원하는 함수의 결과를 packaged_task를 통해 future로 받는다

	return 0;
}