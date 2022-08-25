#include "ItemManager.h"


void ItemManager::Initialize(Player* player, SoundData* soundData, ParticleManager* pManager, ViewMat* view, ProjectionMat* projection)
{
	this->soundData = soundData;
	this->player = player;
	this->view = view;
	this->projection = projection;
	particleManager = pManager;
}

void ItemManager::ItemGenerate(const Vec3& pos, bool& isRightMove)
{
	//‹…‚ğ¶¬A‰Šú‰»
	std::unique_ptr<Item> item = std::make_unique<Item>();
	item->Initialize(player, { pos.x,pos.y,pos.z }, soundData, particleManager);
	item->rightMove = isRightMove;

	//‹…‚ğ“o˜^
	items.push_back(std::move(item));
}

void ItemManager::Update()
{
	//“G
	for (std::unique_ptr<Item>& item : items)
	{
		item->Update(*view, *projection);
		if (item->isGet)
		{
			SoundPlayWave(Directx::GetInstance().xAudio2.Get(), soundData[7], 2.0f);
			/*particleManager->GenerateRandomParticle(item->worldMat.trans, 120, item->worldMat.scale.x * 3.5f,
				{ item->GetColor().x,item->GetColor().y,item->GetColor().z,1.0f }, 10);*/
		}
	}

	//“GÁ‚·
	items.remove_if([](std::unique_ptr<Item>& item)
		{
			return ((item->IsDead() && item->isGet) || item->IsAnnihilation());
		}
	);
}

void ItemManager::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (std::unique_ptr<Item>& Item : items)
	{
		Item->Draw(view, projection, texHundle);
	}
}
