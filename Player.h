#pragma once
#include"PlayerBullet.h"

enum
{
	NORMAL,
	TARGET
};

class Player
{
private:
	//ワールド変換データ
	WorldMat worldMat;
	//モデル
	Draw draw;
	std::list< std::unique_ptr<PlayerBullet>> bullets_;
	static const int shotCool = 10;
	int shotTime = 0;
	int status = NORMAL;

public:
	void Attack();
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);
	int  GetPlayerStatus() { return status; }
};

