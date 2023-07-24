#include "PostEffectManager.h"

PostEffectManager& PostEffectManager::GetInstance()
{
	static PostEffectManager inst;
	return inst;
}

void PostEffectManager::Initialize(const wchar_t* normalTexHandle)
{
	//ポストエフェクト
	postPera_[0] = std::make_unique<PostPera>();
	postPera_[1] = std::make_unique<PostPera>();
	postPera_[0]->Initialize(normalTexHandle);
	postPera_[1]->Initialize(normalTexHandle);
}

void PostEffectManager::Update()
{
	//ポストエフェクト(imgui)
	postPera_[0]->Update();
	postPera_[1]->Update();
}

void PostEffectManager::ImGuiUpdate()
{
	//ポストエフェクト(imgui)
	postPera_[0]->ImGuiUpdate();
	postPera_[1]->ImGuiUpdate();
}

void PostEffectManager::BeforeDraw(std::function<void()> drawSceneF)
{
	//1枚目に描画
	postPera_[0]->DrawToPostpera(drawSceneF);
	postPera_[0]->DrawShrinkTextureForBlur();
	//一枚目に描画結果、二枚目も描画する
	std::function<void()>f2 = [=]() { postPera_[0]->Draw2();  };
	postPera_[1]->DrawToPostpera(f2);

	//ブルーム用(ブラー)
	postPera_[1]->DrawShrinkTextureForBlur();
}

void PostEffectManager::DrawDisplay()
{
	postPera_[1]->Draw2();
}
