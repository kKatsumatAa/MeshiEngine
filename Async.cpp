#include "Async.h"

void Async::SetLockFlag(bool islock)
{
    std::lock_guard<std::mutex>  lock(isLoadedMutex);
    isLoaded = islock;
}

bool Async::GetLockFlag()
{
    std::lock_guard<std::mutex>  lock(isLoadedMutex);
    return isLoaded;
}

void Async::AsyncLoad(std::function<void()> p)
{
    //関数
    p();

    SetLockFlag(true);
}

void Async::StartAsyncFunction(std::function<void()> p)
{
    //非同期処理スタート
    th1 = std::thread([=] { AsyncLoad(p); });
}

void Async::EndThread()
{
    //終わったら
    th1.join();
}
