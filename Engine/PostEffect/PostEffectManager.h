#pragma once
#include"PostPera.h"


class PostEffectManager final
{
private:
	std::unique_ptr<PostPera> postPera_[2];


private:
	PostEffectManager() { ; }
	~PostEffectManager() { ; }

public:
	PostEffectManager& operator=(const PostEffectManager& inst) = delete;
	PostEffectManager(const PostEffectManager& inst) = delete;

	static PostEffectManager& GetInstance();

public:
	void Initialize(const wchar_t* normalTexHandle);
	void Update();
	//シーンのものをポストペラに描画する
	void BeforeDraw(std::function<void()>drawSceneF);
	//実際に画面に描画
	void DrawDisplay();

public:
	PostPera* GetPostEffect1() { return postPera_[0].get(); }
	PostPera* GetPostEffect2() { return postPera_[1].get(); }
};
