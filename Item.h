#pragma once
#include "Collider.h"

//アイテムの種類
enum ITEM_TYPE
{
	INORMAL,
	ILASER,
	ISHOTGUN
};

class Item :
    public Collider
{
private:

	Model* model = nullptr;
	Object draw;

	const Vec3 scaleTmp = { 3.0f,3.0f,3.0f };

	int itemType;

public:


public:
	void Initialize(const Vec3& position, const int itemType, Model* model);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);

	int GetItemType() { return itemType; }

	//衝突を検出したら呼び出す（コールバック関数）
	void OnCollision(Collider& collider) override;
	void OnCollision2(Collider& collider) override;
};

