#pragma once
#include"Item.h"

class ItemManager
{
private:
	Model* model = nullptr;
	//sound
	Sound* audio = nullptr;

public:
	//
	std::list< std::unique_ptr<Item>> items_;


	void Initialize(Model* model);


	void GenerateItem(const Vec3& position, const int& itemType);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
	void DrawSprite();

	//�e���X�g���擾(const�Q��)
	const std::list<std::unique_ptr<Item>>& GetItems()
	{
		return items_;
	}
};

