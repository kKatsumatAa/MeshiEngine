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
	postPera_[0] = std::make_unique<PostPera>();
	postPera_[1] = std::make_unique<PostPera>();
	postPera_[2] = std::make_unique<PostPera>();
	postPera_[0]->Initialize(normalTexHandle);
	postPera_[1]->Initialize(normalTexHandle);
	postPera_[2]->Initialize(normalTexHandle);
}

void PostEffectManager::Update()
{
	postPera_[0]->Update();
	postPera_[1]->Update();
	postPera_[2]->Update();
}

void PostEffectManager::ImGuiUpdate()
{
	ImGui::Begin("PostEffect");

	//(imgui)
	postPera_[0]->ImGuiUpdate(0);
	postPera_[1]->ImGuiUpdate(1);
	postPera_[2]->ImGuiUpdate(2);

	ImGui::End();
}

void PostEffectManager::BeforeDraw(std::function<void()> drawSceneF, const std::function<void()>& drawSpriteF)
{
	//1枚目に描画
	postPera_[0]->DrawToPostpera(drawSceneF, true);

	//深度バッファ
	DirectXWrapper::GetInstance().ResourceBarrier(
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		DirectXWrapper::GetInstance().GetDepthBuff());

	//ぼかしテクスチャに描画
	postPera_[0]->DrawToBlurAll();
	//ぼかし
	postPera_[0]->DrawShrinkTextureForBlur();

	//------------------------------------------------------------

	//一枚目に描画結果、二枚目も描画する
	std::function<void()>f2 = [=]() { postPera_[0]->Draw2();  };

	postPera_[1]->DrawToPostpera(f2, false, drawSpriteF);
	//ブラー用に書き込み
	postPera_[1]->DrawToBlurAll();

	//ブルーム用(ブラー)
	postPera_[1]->DrawShrinkTextureForBlur();


	//------------------------------------------------------------

	//二枚目に描画結果、三枚目も描画する
	std::function<void()>f3 = [=]() { postPera_[1]->Draw2();  };

	postPera_[2]->DrawToPostpera(f3);
	//ブラー用に書き込み
	postPera_[2]->DrawToBlurAll();

	//ブルーム用(ブラー)
	postPera_[2]->DrawShrinkTextureForBlur();
}

void PostEffectManager::DrawDisplay()
{
	postPera_[2]->Draw2();

	//深度バッファ
	DirectXWrapper::GetInstance().ResourceBarrier(
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		DirectXWrapper::GetInstance().GetDepthBuff());
}
