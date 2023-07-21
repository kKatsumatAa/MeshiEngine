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
	void Initialize();
	void Update();
	//�V�[���̂��̂��|�X�g�y���ɕ`�悷��
	void BeforeDraw(std::function<void()>drawSceneF);
	//���ۂɉ�ʂɕ`��
	void DrawDisplay();

public:
	PostPera* GetPostPera1() { return postPera_[0].get(); }
	PostPera* GetPostPera2() { return postPera_[1].get(); }
};
