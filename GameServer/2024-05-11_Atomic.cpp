#include "pch.h"
#include <thread>
#include <atomic>


atomic<int> sum = 0; // atomic 

void Add()
{
	for (int i = 0; i < 1000000; i++)
	{
		sum++; 

		//  sum++ 한줄의 코드로 실행되는것이 아니라 컴파일 후 어셈블리어로 변환 되서 실행이된다.
		//  00007FF7BB8824F5  mov         eax, dword ptr[sum(07FF7BB88F480h)]     / eax = sum
		//	00007FF7BB8824FB  inc         eax								      / eax = eax + 1
		//	00007FF7BB8824FD  mov         dword ptr[sum(07FF7BB88F480h)], eax     / sum = eax
		/*  왜 한줄에 실행시키는것이 아니라 3줄에 걸쳐서 실행을 시키는 것인가?
		cpu 가 메모리에 있는 값을 가져오고 거기다가 연산을 하는 행동이 동시에 처리가 불가능하다

		멀티쓰레드에서 발생하는 문제 
		
		Add 함수가 호출이 되서 
		eax = sum 
		eax = eax + 1 << eax에 더하는 와중에 Sub 함수가 호출이 된다면?
		sum = eax

		Sub 함수는 현재 sum 이 0이다 즉 Add함수에서 아직 값을 sum에 할당하지 않았음 따라서
		eax = sum
		eax = eax - 1
		sum = eax

		Add함수가 먼저 끝나서 sum 이 1 이 됬지만
		Sub 함수는 sum이 0값에서 가져와서 
		즉 sum = -1 값이 들어간다

		따라서 멀티스레드 환경에서는 동기화 작업이 필요하다 
		atomic<int> sum = 0; // atomic 객체로 만들어주면 동기화가 가능하다
		atomic 는 위에 문제를 어떻게 해결 했을까?
		00007FF641FB23CE  call        std::_Atomic_integral<int,4>::operator++ (07FF641FB153Ch)

		디버깅에서 디스어셈블리를 확인해본 결과 1줄의 코드로 실행이되고 atomic 내부에서 처리가 되는것 같다.
		Add 함수에서 sum++; 실행 중에는 Sub 함수가 sum--; 코드가 실행되지 않도록 대기상태에 들어간다.
		따라서 정상적인 결과가 나옴 0

		하지만 atomic는 일반 정수에 비해 연산이 많이 느려서 사용할때 주의해야함 
		*/ 
	}
}

void Sub()
{
	for (int i = 0; i < 1000000; i++)
	{
		sum--;
	}
}

int main()
{
	Add();
	Sub();

	cout << sum << endl; // 싱글 스레드 환경에서 공유자원은 문제가 발생하지 않음
	
	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl; // 멀티 스레드 환경에서는 문제가 발생 ex) sum : -210758

	return 0;
}