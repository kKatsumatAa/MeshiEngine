/*
* @file ImGuiManager.h
* @brief ImGuiの処理を管理
*/

#pragma once
#include<imgui.h>
#include"TextureManager.h"

//ImGuiの管理
class ImGuiManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// imGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// imgui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};

