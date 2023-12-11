#include "Replay.h"
#include"MouseInput.h"
#include"KeyboardInput.h"


void Replay::Initialize()
{
	if (rePlayDatas_.size())
	{
		rePlayDatas_.clear();
	}
	replayDataCount_ = 0;

	NextFrame();
}

void Replay::NextFrame()
{
	rePlayDatas_.push_back(std::make_unique<ReplayData>());
}

const ReplayData& Replay::GetNormalTimeTotalData()
{
	return *(rePlayDatas_[replayDataCount_].get());
}

bool Replay::AddReplayDataCount()
{
	replayDataCount_++;

	if (replayDataCount_ >= rePlayDatas_.size())
	{
		return false;
	}

	return true;
}
