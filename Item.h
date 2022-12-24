#pragma once
#include "Collider.h"

//�A�C�e���̎��
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

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision(Collider& collider) override;
	void OnCollision2(Collider& collider) override;
};

