#include "LandShape.h"
#include "GameVelocityManager.h"


std::unique_ptr<LandShape> LandShape::Create(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	// �I�u�W�F�N�g�̃C���X�^���X�𐶐�
	std::unique_ptr<LandShape> instance = std::make_unique<LandShape>();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize(std::move(worldMat), model)) {
		instance.reset();
		assert(0);
	}

	return instance;
}

bool LandShape::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	if (!TouchableObject::Initialize(std::move(worldMat), model))
	{
		return false;
	}

	//�f�B�]���u�̉摜
	SetisDissolve(true);
	SetDissolveT(0);
	SetDissolveTexHandle(TextureManager::GetInstance().LoadGraph("dissolveMask.png"));

	//�m�[�}���}�b�v
	SetIsNormalMap(true);
	SetNormalMapTexHandle(TextureManager::GetInstance().LoadGraph("landShapeNormal.jpg"));

	return true;
}

void LandShape::Update()
{
	Object::Update();
}

void LandShape::Draw()
{
	Object::DrawModel(nullptr);
}
