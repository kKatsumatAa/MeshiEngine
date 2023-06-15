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
    //�֐�
    p();

    SetLockFlag(true);
}

void Async::StartAsyncFunction(const std::function<void()>& p)
{
    //�񓯊������X�^�[�g
    th1_ = std::thread([=] { AsyncLoad(p); });
}

void Async::EndThread()
{
    //�I�������
    th1_.join();
}
