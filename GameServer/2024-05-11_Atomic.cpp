#include "pch.h"
#include <thread>
#include <atomic>


atomic<int> sum = 0; // atomic 

void Add()
{
	for (int i = 0; i < 1000000; i++)
	{
		sum++; 

		//  sum++ ������ �ڵ�� ����Ǵ°��� �ƴ϶� ������ �� �������� ��ȯ �Ǽ� �����̵ȴ�.
		//  00007FF7BB8824F5  mov         eax, dword ptr[sum(07FF7BB88F480h)]     / eax = sum
		//	00007FF7BB8824FB  inc         eax								      / eax = eax + 1
		//	00007FF7BB8824FD  mov         dword ptr[sum(07FF7BB88F480h)], eax     / sum = eax
		/*  �� ���ٿ� �����Ű�°��� �ƴ϶� 3�ٿ� ���ļ� ������ ��Ű�� ���ΰ�?
		cpu �� �޸𸮿� �ִ� ���� �������� �ű�ٰ� ������ �ϴ� �ൿ�� ���ÿ� ó���� �Ұ����ϴ�

		��Ƽ�����忡�� �߻��ϴ� ���� 
		
		Add �Լ��� ȣ���� �Ǽ� 
		eax = sum 
		eax = eax + 1 << eax�� ���ϴ� ���߿� Sub �Լ��� ȣ���� �ȴٸ�?
		sum = eax

		Sub �Լ��� ���� sum �� 0�̴� �� Add�Լ����� ���� ���� sum�� �Ҵ����� �ʾ��� ����
		eax = sum
		eax = eax - 1
		sum = eax

		Add�Լ��� ���� ������ sum �� 1 �� ������
		Sub �Լ��� sum�� 0������ �����ͼ� 
		�� sum = -1 ���� ����

		���� ��Ƽ������ ȯ�濡���� ����ȭ �۾��� �ʿ��ϴ� 
		atomic<int> sum = 0; // atomic ��ü�� ������ָ� ����ȭ�� �����ϴ�
		atomic �� ���� ������ ��� �ذ� ������?
		00007FF641FB23CE  call        std::_Atomic_integral<int,4>::operator++ (07FF641FB153Ch)

		����뿡�� �𽺾������ Ȯ���غ� ��� 1���� �ڵ�� �����̵ǰ� atomic ���ο��� ó���� �Ǵ°� ����.
		Add �Լ����� sum++; ���� �߿��� Sub �Լ��� sum--; �ڵ尡 ������� �ʵ��� �����¿� ����.
		���� �������� ����� ���� 0

		������ atomic�� �Ϲ� ������ ���� ������ ���� ������ ����Ҷ� �����ؾ��� 
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

	cout << sum << endl; // �̱� ������ ȯ�濡�� �����ڿ��� ������ �߻����� ����
	
	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl; // ��Ƽ ������ ȯ�濡���� ������ �߻� ex) sum : -210758

	return 0;
}