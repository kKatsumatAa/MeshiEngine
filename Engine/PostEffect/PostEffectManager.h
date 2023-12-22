/*
* @file PostEffectManager.h
* @brief 複数のポストエフェクトを管理
*/

#pragma once
#include"PostPera.h"


class PostEffectManager final
{
public:
	//演出用の画面の最低サイズ
	const float DISPLAY_SIZE_MIN_ = 0.9f;
	static const int32_t POST_PERA_NUM_ = 4;

private:
	//ポストペラ
	std::unique_ptr<PostPera> postPera_[POST_PERA_NUM_];

private:
	//背景用描画
	std::function<void()>backGroundF = NULL;

private:
	PostEffectManager() { ; }
	~PostEffectManager() { ; }

public:
	PostEffectManager& operator=(const PostEffectManager& inst) = delete;
	PostEffectManager(const PostEffectManager& inst) = delete;

	static PostEffectManager& GetInstance();

public:
	void Initialize(const char* normalTexHandle);
	void Update();
	void ImGuiUpdate();
	//背景用の事前描画
	void PreBackGroundDraw();
	//シーンのものをポストペラに描画する
	void BeforeDraw(std::function<void()>drawSceneF, const std::function<void()>& drawSpriteF = nullptr);
	//実際に画面に描画
	void DrawDisplay();

public:
	PostPera* GetPostEffect1() { return postPera_[0].get(); }
	PostPera* GetPostEffect2() { return postPera_[1].get(); }
	PostPera* GetPostEffect3() { return postPera_[2].get(); }
	PostPera* GetPostEffect4() { return postPera_[3].get(); }
};
