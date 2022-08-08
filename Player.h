#pragma once
#include"PlayerBullet.h"

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

public:
	void Attack();
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle, const UINT64& bulletTexHundle);
};

