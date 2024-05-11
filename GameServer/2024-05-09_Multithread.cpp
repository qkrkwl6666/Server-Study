#include "pch.h"

#include <thread>

void HelloThread()
{
    cout << "Hello Thread" << endl;
}


void HelloThread2(int num)
{
    cout << num << endl;
}

int main()
{
    std::thread t;
    std::thread t2;

    auto tId1 = t.get_id(); // 0

    // 현재 t 쓰레드 작동 X

    t = std::thread(HelloWorld);
    t2 = std::thread(HelloThread2, 100);

    auto tId2 = t.get_id(); // 

    // 자주 쓰는 쓰레드 메서드
    int32 core = t.hardware_concurrency(); //cpu 코어 개수?
    auto id = t.get_id();// 쓰레드의 id
    cout << core << endl;
    cout << id << endl;
    // t.detach(); thread 객체에서 실제 쓰레드 분리
    // t.joinable(); // 쓰레드가 현재 실행중인지 리턴 Id 가 0이면 작동 X

    if (t.joinable())
        t.join();

    if (t2.joinable())
        t2.join();

    std::thread t3;

    std::vector<std::thread> v;

    v.reserve(10);

    for (int i = 0; i < 10; i++)
    {
        v.push_back(std::thread(HelloThread2 , i));
    }


    for (int i = 0; i < 10; i++)
    {
        if (v[i].joinable())
            v[i].join();
    }

        
}
