#pragma once
#include"Item.h"


class ItemManager
{
private:
	SoundData* soundData;
	Player* player;

	float gTimer = 300;

	//待機
	bool isWait = false;
	int  waitTimer = 0;

	ParticleManager* particleManager;

	ViewMat* view;
	ProjectionMat* projection;


public:
	std::list<std::unique_ptr<Item>> items;


	void ItemGenerate(const Vec3& pos, bool& isRightMove);
	void Initialize(Player* player, SoundData* soundData, ParticleManager* pManager, ViewMat* view, ProjectionMat* projection);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<Item>>& GetItems()
	{
		return items;
	}
};

