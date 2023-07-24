#include "PostEffectManager.h"

PostEffectManager& PostEffectManager::GetInstance()
{
	static PostEffectManager inst;
	return inst;
}

void PostEffectManager::Initialize(const wchar_t* normalTexHandle)
{
	//�|�X�g�G�t�F�N�g
	postPera_[0] = std::make_unique<PostPera>();
	postPera_[1] = std::make_unique<PostPera>();
	postPera_[0]->Initialize(normalTexHandle);
	postPera_[1]->Initialize(normalTexHandle);
}

void PostEffectManager::Update()
{
	//�|�X�g�G�t�F�N�g(imgui)
	postPera_[0]->Update();
	postPera_[1]->Update();
}

void PostEffectManager::ImGuiUpdate()
{
	//�|�X�g�G�t�F�N�g(imgui)
	postPera_[0]->ImGuiUpdate();
	postPera_[1]->ImGuiUpdate();
}

void PostEffectManager::BeforeDraw(std::function<void()> drawSceneF)
{
	//1���ڂɕ`��
	postPera_[0]->DrawToPostpera(drawSceneF);
	postPera_[0]->DrawShrinkTextureForBlur();
	//�ꖇ�ڂɕ`�挋�ʁA�񖇖ڂ��`�悷��
	std::function<void()>f2 = [=]() { postPera_[0]->Draw2();  };
	postPera_[1]->DrawToPostpera(f2);

	//�u���[���p(�u���[)
	postPera_[1]->DrawShrinkTextureForBlur();
}

void PostEffectManager::DrawDisplay()
{
	postPera_[1]->Draw2();
}
