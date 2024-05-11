#include "pch.h"
#include <mutex>

std::vector<int> v;
std::mutex m;


template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard()
	{
		_mutex->unlock();
	}
private:
	T* _mutex;
};


void Add()
{

	std::lock_guard<std::mutex> lockguard(m); 

	for (int i = 0; i < 10000; i++)
	{
		//LockGuard<std::mutex> lockguard(m); 

		//std::unique_lock<std::mutex> unique_lock(m, std::defer_lock); // unique_lock 은 생성자에서 lock 하는 방식이 아닌

		//unique_lock.lock(); // lock 을 내가 원하는 시점에서 호출 할 수있음

		// 수동으로 lock, unlock를 하지않고 클래스의 생성자와 소멸자를 이용해서 
		// 자동으로 lock 및 unlock 을 구현 할 수 있음
		//m.lock(); // 스레드가 v.push_back 접근하고있고 unlock할때까지 다른 스레드는 대기상태

		v.push_back(i);

		//m.unlock(); // unlock 스레드 접근 허용
	}
}
/*
각스레드를 Add 함수로 설정해주고 실행을 한다면 에러가 난다.
vector 에서 push_back 할때 Capacity 가 부족하면 메모리를 새로 할당하는데
t1 스레드에서 메모리를 할당해서 
[1][2][3] << 기존의 메모리 << 삭제
[1][2][3][][][] << 새로운 메모리
위에 기존의 메모리를 삭제하고 새로운 메모리를 할당하는 과정에서
t2 스레드도 Add함수가 호출이되서 
[1][2][3] << 기존의 메모리를 한번더 삭제 즉 이미 삭제된 상태에서 삭제를 하기때문에
에러가 난다는것을 알수있다. 
그러면 처음부터 Capacity 를 여유롭게 설정한다면?
v.reserve(20000) 설정해주면 에러는 나지 않지만 
원하는 size가 20000이 나오지 않고 이상한 값이 나온다

따라서 한스레드만 v에 접근할 수 있도록 순서를 정해줘야한다. (int는 atomic로 해결했지만 vector는 atomic를 사용할 수 없음)
mutex 를 사용해서 lock 걸수있음
*/

int main()
{
	// v.reserve(20000); // Capacity 20000 설정

	std::thread t1(Add);
	std::thread t2(Add);

	t1.join();
	t2.join();
	
	std::cout << v.size() << endl;


	return 0;
}