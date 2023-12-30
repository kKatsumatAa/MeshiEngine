#include "PostEffectManager.h"
#include "ImGuiManager.h"

PostEffectManager& PostEffectManager::GetInstance()
{
	static PostEffectManager inst;
	return inst;
}

void PostEffectManager::Initialize(const char* normalTexHandle)
{
	//ポストエフェクト
	for (int8_t i = 0; i < S_POST_PERA_NUM_; i++)
	{
		postPera_[i] = std::make_unique<PostPera>();
		postPera_[i]->Initialize(normalTexHandle);
	}
}

void PostEffectManager::Update()
{
	for (int8_t i = 0; i < S_POST_PERA_NUM_; i++)
	{
		postPera_[i]->Update();
	}
}

void PostEffectManager::ImGuiUpdate()
{
	ImGui::Begin("PostEffect");

	//(imgui)
	for (int8_t i = 0; i < S_POST_PERA_NUM_; i++)
	{
		postPera_[i]->ImGuiUpdate(i);
	}

	ImGui::End();
}

void PostEffectManager::PreBackGroundDraw()
{
	//0枚目に描画（海とか背景用）
	postPera_[PERA_0]->DrawToPostpera(NULL);
	//ぼかしテクスチャに描画
	postPera_[PERA_0]->DrawToBlurAll();
	//ぼかし
	postPera_[PERA_0]->DrawShrinkTextureForBlur();

	backGroundF = [=]() {postPera_[PERA_0]->Draw2(); };
}

void PostEffectManager::BeforeDraw(std::function<void()> drawSceneF, const std::function<void()>& drawSpriteF)
{
	auto f1 = [=]() {backGroundF(); drawSceneF(); };

	//1枚目に描画
	postPera_[PERA_1]->DrawToPostpera(f1, true);

	//深度バッファ
	DirectXWrapper::GetInstance().ResourceBarrier(
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		DirectXWrapper::GetInstance().GetDepthBuff());

	//ぼかしテクスチャに描画
	postPera_[PERA_1]->DrawToBlurAll();
	//ぼかし
	postPera_[PERA_1]->DrawShrinkTextureForBlur();

	//------------------------------------------------------------

	//一枚目に描画結果、二枚目も描画する
	std::function<void()>f2 = [=]() { postPera_[PERA_1]->Draw2();  };

	postPera_[PERA_2]->DrawToPostpera(f2, false, drawSpriteF);
	//ブラー用に書き込み
	postPera_[PERA_2]->DrawToBlurAll();

	//ブルーム用(ブラー)
	postPera_[PERA_2]->DrawShrinkTextureForBlur();


	//------------------------------------------------------------

	//二枚目に描画結果、三枚目も描画する
	std::function<void()>f3 = [=]() { postPera_[PERA_2]->Draw2();  };

	postPera_[PERA_3]->DrawToPostpera(f3);
	//ブラー用に書き込み
	postPera_[PERA_3]->DrawToBlurAll();

	//ブルーム用(ブラー)
	postPera_[PERA_3]->DrawShrinkTextureForBlur();
}

void PostEffectManager::DrawDisplay()
{
	postPera_[PERA_3]->Draw2();

	//深度バッファ
	DirectXWrapper::GetInstance().ResourceBarrier(
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		DirectXWrapper::GetInstance().GetDepthBuff());
}
