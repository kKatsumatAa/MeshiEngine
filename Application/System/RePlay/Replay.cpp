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
}

void Replay::NextFrame()
{
	rePlayDatas_.push_back(std::make_unique<ReplayData>());
}

void Replay::UpdateWhile1Frame(std::function<void(const ReplayData& replay)>updateF)
{
	float totalSpeed = 0;

	while (true)
	{
		//フレームデータ
		auto replayData = rePlayDatas_[replayDataCount_].get();

		//超過分
		float overVel = 0;
		//合計が1.0fを超えていたら
		if (totalSpeed + replayData->gameVel > 1.0f)
		{
			//残りを今回使う量に
			float useVel = 1.0f - totalSpeed;
			//超過分
			overVel = replayData->gameVel - useVel;
			//使う量だけ
			replayData->gameVel = useVel;
		}

		//更新処理
		updateF(*replayData);

		//合計スピード
		totalSpeed += replayData->gameVel;

		//また次にこの関数来た時に超過分から始めるか
		//フレームデータの終端
		if (overVel > 0.0f)
		{
			replayData->gameVel = overVel;
			break;
		}
		else if (replayDataCount_ >= rePlayDatas_.size() - 1)
		{
			break;
		}
		//次のフレームデータに行く
		else
		{
			replayDataCount_++;
		}
	}
}

const ReplayData& Replay::GetNormalTimeTotalData()
{
	return *(rePlayDatas_[replayDataCount_].get());
}

bool Replay::AddReplayDataCount()
{
	replayDataCount_++;

	if (GetReplayEnd())
	{
		return false;
	}

	return true;
}
