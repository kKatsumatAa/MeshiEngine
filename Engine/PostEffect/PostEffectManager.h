#pragma once
#include"PostPera.h"


class PostEffectManager final
{
public:
	//���o�p�̉�ʂ̍Œ�T�C�Y
	const float DISPLAY_SIZE_MIN_ = 0.9f;
	static const int32_t POST_PERA_NUM_ = 3;

private:
	//�|�X�g�y��
	std::unique_ptr<PostPera> postPera_[POST_PERA_NUM_];

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
	//�V�[���̂��̂��|�X�g�y���ɕ`�悷��
	void BeforeDraw(std::function<void()>drawSceneF, const std::function<void()>& drawSpriteF = nullptr);
	//���ۂɉ�ʂɕ`��
	void DrawDisplay();

public:
	PostPera* GetPostEffect1() { return postPera_[0].get(); }
	PostPera* GetPostEffect2() { return postPera_[1].get(); }
	PostPera* GetPostEffect3() { return postPera_[2].get(); }
};
