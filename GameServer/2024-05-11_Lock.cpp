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

		//std::unique_lock<std::mutex> unique_lock(m, std::defer_lock); // unique_lock �� �����ڿ��� lock �ϴ� ����� �ƴ�

		//unique_lock.lock(); // lock �� ���� ���ϴ� �������� ȣ�� �� ������

		// �������� lock, unlock�� �����ʰ� Ŭ������ �����ڿ� �Ҹ��ڸ� �̿��ؼ� 
		// �ڵ����� lock �� unlock �� ���� �� �� ����
		//m.lock(); // �����尡 v.push_back �����ϰ��ְ� unlock�Ҷ����� �ٸ� ������� ������

		v.push_back(i);

		//m.unlock(); // unlock ������ ���� ���
	}
}
/*
�������带 Add �Լ��� �������ְ� ������ �Ѵٸ� ������ ����.
vector ���� push_back �Ҷ� Capacity �� �����ϸ� �޸𸮸� ���� �Ҵ��ϴµ�
t1 �����忡�� �޸𸮸� �Ҵ��ؼ� 
[1][2][3] << ������ �޸� << ����
[1][2][3][][][] << ���ο� �޸�
���� ������ �޸𸮸� �����ϰ� ���ο� �޸𸮸� �Ҵ��ϴ� ��������
t2 �����嵵 Add�Լ��� ȣ���̵Ǽ� 
[1][2][3] << ������ �޸𸮸� �ѹ��� ���� �� �̹� ������ ���¿��� ������ �ϱ⶧����
������ ���ٴ°��� �˼��ִ�. 
�׷��� ó������ Capacity �� �����Ӱ� �����Ѵٸ�?
v.reserve(20000) �������ָ� ������ ���� ������ 
���ϴ� size�� 20000�� ������ �ʰ� �̻��� ���� ���´�

���� �ѽ����常 v�� ������ �� �ֵ��� ������ ��������Ѵ�. (int�� atomic�� �ذ������� vector�� atomic�� ����� �� ����)
mutex �� ����ؼ� lock �ɼ�����
*/

int main()
{
	// v.reserve(20000); // Capacity 20000 ����

	std::thread t1(Add);
	std::thread t2(Add);

	t1.join();
	t2.join();
	
	std::cout << v.size() << endl;


	return 0;
}