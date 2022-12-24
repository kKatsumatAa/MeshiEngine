#include "ItemManager.h"

void ItemManager::Initialize(Model* model)
{
	assert(model);

	this->model = model;
	items_.clear();
}

void ItemManager::GenerateItem(const Vec3& position, const int& itemType)
{
	//アイテムを生成、初期化
	std::unique_ptr<Item> item = std::make_unique<Item>();
	item->Initialize(position, itemType, model);
	//アイテムを登録
	items_.push_back(std::move(item));
}

void ItemManager::Update()
{
	for (std::unique_ptr<Item>& item : items_)
	{
		item->Update();
	}

	items_.remove_if([](std::unique_ptr<Item>& item)
		{
			return item->GetIsDead();
		}
	);
}

void ItemManager::Draw(ViewMat& view, ProjectionMat& projection)
{
	for (std::unique_ptr<Item>& item : items_)
	{
		item->Draw(view, projection);
	}
}

void ItemManager::DrawSprite()
{
}
