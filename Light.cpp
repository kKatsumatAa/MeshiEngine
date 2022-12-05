#include "Light.h"

ID3D12Device* Light::device_ = nullptr;


void Light::StaticInitialize()
{
	//�ď��������`�F�b�N
	assert(!Light::device_);
	//nullptr�`�F�b�N
	assert(Directx::GetInstance().GetDevice());
	//�Z�b�g
	Light::device_ = Directx::GetInstance().GetDevice();
}

void Light::Initialize()
{

}
