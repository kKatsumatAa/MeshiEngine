#include "Light.h"

ID3D12Device* Light::device_ = nullptr;


void Light::StaticInitialize()
{
	//再初期化をチェック
	assert(!Light::device_);
	//nullptrチェック
	assert(Directx::GetInstance().GetDevice());
	//セット
	Light::device_ = Directx::GetInstance().GetDevice();
}

void Light::Initialize()
{

}
