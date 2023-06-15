#include "Async.h"

void Async::SetLockFlag(bool islock)
{
    std::lock_guard<std::mutex>  lock(isLoadedMutex_);
    isLoaded_ = islock;
}

bool Async::GetLockFlag()
{
    std::lock_guard<std::mutex>  lock(isLoadedMutex_);
    return isLoaded_;
}

void Async::AsyncLoad(std::function<void()> p)
{
    //関数
    p();

    SetLockFlag(true);
}

void Async::StartAsyncFunction(const std::function<void()>& p)
{
    //非同期処理スタート
    th1_ = std::thread([=] { AsyncLoad(p); });
}

void Async::EndThread()
{
    //終わったら
    th1_.join();
}
