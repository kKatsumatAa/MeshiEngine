/*
* @file ClearEffect.h
* @brief ステージのクリア後の演出
*/

#pragma once
#include"Sprite.h"

//ステートの前方宣言
class ClearEffectState;

//クリア演出
class ClearEffect final
{
private:
	//ステート
	std::unique_ptr<ClearEffectState> state_ = nullptr;
	//エフェクト中か
	bool isEffect_ = false;

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ClearEffect() { ; }

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ClearEffect() { ; }

public:
	/// <summary>
	/// エフェクト中かをセット
	/// </summary>
	/// <param name="isEffect">エフェクト中かのフラグ</param>
	void SetIsEffect(bool isEffect) { isEffect_ = isEffect; }

	/// <summary>
	/// 演出中かをゲット
	/// </summary>
	/// <returns>演出中かのフラグ</returns>
	bool GetIsEffect() { return isEffect_; }


public:
	//コピーコンストラクタ禁止
	ClearEffect& operator=(const ClearEffect& obj) = delete;
	//コピーコンストラクタ禁止
	ClearEffect(const ClearEffect& obj) = delete;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>スタティックのインスタンス</returns>
	static ClearEffect& GetInstance();

public:
	/// <summary>
	/// ステートを変更
	/// </summary>
	/// <param name="state">変更後のステート</param>
	void ChangeState(std::unique_ptr<ClearEffectState> state);
	
	//クリア演出を始める
	void BeginClearEffect();

	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画
	void Draw();

};