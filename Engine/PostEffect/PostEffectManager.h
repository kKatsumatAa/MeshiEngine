#pragma once
#include"PostPera.h"


class PostEffectManager final
{
private:
	std::unique_ptr<PostPera> postPera_[2];

public:
	//���o�p�̉�ʂ̍Œ�T�C�Y
	const float DISPLAY_SIZE_MIN_ = 0.9f;


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
	void ImGuiUpdate();
	//�V�[���̂��̂��|�X�g�y���ɕ`�悷��
	void BeforeDraw(std::function<void()>drawSceneF);
	//���ۂɉ�ʂɕ`��
	void DrawDisplay();

public:
	PostPera* GetPostEffect1() { return postPera_[0].get(); }
	PostPera* GetPostEffect2() { return postPera_[1].get(); }
};
