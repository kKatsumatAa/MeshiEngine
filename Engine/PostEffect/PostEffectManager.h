/*
* @file PostEffectManager.h
* @brief 複数のポストエフェクトを管理
*/

#pragma once
#include"PostPera.h"


class PostEffectManager final
{
public:
	enum PostPeraNum
	{
		PERA_0,
		PERA_1,
		PERA_2,
		PERA_3,
	};

public:
	//演出用の画面の最低サイズ
	const float DISPLAY_SIZE_MIN_ = 0.9f;
	static const int32_t S_POST_PERA_NUM_ = 4;

private:
	//ポストペラ
	std::unique_ptr<PostPera> postPera_[S_POST_PERA_NUM_];

private:
	//背景用描画
	std::function<void()>backGroundF = NULL;

private:
	//コンストラクタ
	PostEffectManager() { ; }
	//デストラクタ
	~PostEffectManager() { ; }

public:
	//コピーコンストラクタ禁止
	PostEffectManager& operator=(const PostEffectManager& inst) = delete;
	//コピーコンストラクタ禁止
	PostEffectManager(const PostEffectManager& inst) = delete;

	//インスタンス取得
	static PostEffectManager& GetInstance();

public:
	//初期化
	void Initialize(const char* normalTexHandle);
	//更新
	void Update();
	//ImGuiの描画
	void ImGuiUpdate();
	//背景用の事前描画
	void PreBackGroundDraw();
	//シーンのものをポストペラに描画する
	void BeforeDraw(std::function<void()>drawSceneF, const std::function<void()>& drawSpriteF = nullptr);
	//実際に画面に描画
	void DrawDisplay();

public:
	//1つ目のポストペラを取得
	PostPera* GetPostEffect1() { return postPera_[0].get(); }
	//2つ目のポストペラを取得
	PostPera* GetPostEffect2() { return postPera_[1].get(); }
	//3つ目のポストペラを取得
	PostPera* GetPostEffect3() { return postPera_[2].get(); }
	//4つ目のポストペラを取得
	PostPera* GetPostEffect4() { return postPera_[3].get(); }
};
