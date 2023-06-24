#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	sceneM_->draw_[5].PlayReverseAnimation(sceneM_->modelFBX_, true);
}

void SceneGame::Update(PostPera* postPera)
{
	//‰Û‘è—p
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		if (postPera[0].effectFlags_.isMultiTex)
		{
			postPera[0].effectFlags_.isMultiTex = false;
			postPera[0].effectFlags_.isGlassFilter = true;
		}
		else if (postPera[0].effectFlags_.isGlassFilter)
		{
			postPera[0].effectFlags_.isGlassFilter = false;
			postPera[0].effectFlags_.isBloom = true;
		}
		else if (postPera[0].effectFlags_.isBloom)
		{
			postPera[0].effectFlags_.isBloom = false;
			postPera[0].effectFlags_.isCrossFilter = true;
		}
		else if (postPera[0].effectFlags_.isGaussian)
		{
			postPera[0].effectFlags_.isGaussian = false;
			postPera[0].effectFlags_.isCrossFilter = true;
		}
		else if (postPera[0].effectFlags_.isCrossFilter)
		{
			postPera[0].effectFlags_.isCrossFilter = false;
			//postPera[0].effectFlags_.isMultiTex = true;
		}
		else if (!postPera[0].effectFlags_.isCrossFilter && !postPera[0].effectFlags_.isMultiTex)
		{
			postPera[0].effectFlags_.isMultiTex = true;
		}
		/*else if (postPera[0].effectFlags_.isGaussian2)
		{
			postPera[0].effectFlags_.isGaussian2 = false;
			postPera[0].effectFlags_.isEmboss = true;
		}*/
		else if (postPera[0].effectFlags_.isEmboss)
		{
			postPera[0].effectFlags_.isEmboss = false;
			postPera[0].effectFlags_.isSharpness = true;
		}
		else if (postPera[0].effectFlags_.isSharpness)
		{
			postPera[0].effectFlags_.isSharpness = false;
			postPera[0].effectFlags_.isGradation = true;
		}
		else if (postPera[0].effectFlags_.isGradation)
		{
			postPera[0].effectFlags_.isGradation = false;
			postPera[0].effectFlags_.isOutLine = true;
		}
		else if (postPera[0].effectFlags_.isOutLine)
		{
			postPera[0].effectFlags_.isOutLine = false;
			postPera[0].effectFlags_.isVignette = true;
		}
		else if (postPera[0].effectFlags_.isVignette)
		{
			postPera[0].effectFlags_.isVignette = false;
			postPera[0].effectFlags_.isBarrelCurve = true;
		}
		else if (postPera[0].effectFlags_.isBarrelCurve)
		{
			postPera[0].effectFlags_.isBarrelCurve = false;
			postPera[0].effectFlags_.isScanningLine = true;
		}
		else if (postPera[0].effectFlags_.isScanningLine)
		{
			postPera[0].effectFlags_.isScanningLine = false;
			postPera[0].effectFlags_.isGrayScale = true;
		}
		else if (postPera[0].effectFlags_.isGrayScale)
		{
			postPera[0].effectFlags_.isGrayScale = false;
			postPera[0].effectFlags_.isMosaic = true;
		}

		else if (postPera[0].effectFlags_.isMosaic)
		{
			postPera[0].effectFlags_.isMosaic = false;
			postPera[0].effectFlags_.isNega = true;
		}
		else if (postPera[0].effectFlags_.isNega)
		{
			postPera[0].effectFlags_.isNega = false;
			postPera[0].effectFlags_.isRGBShift = true;
		}
		else if (postPera[0].effectFlags_.isRGBShift)
		{
			postPera[0].effectFlags_.isRGBShift = false;
			postPera[0].effectFlags_.isMultiTex = true;
		}
	}


	LevelManager::GetInstance().Update();

	//ƒV[ƒ“‘JˆÚ
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	LevelManager::GetInstance().Draw();

	//sceneM_->draw_[6].DrawModel(sceneM_->model_[6]);

	//ÅŒã‚É•`‰æ‚µ‚È‚¢‚Æ‰f‚ç‚È‚¢
	//ParticleManager::GetInstance()->Draw(sceneM->texhandle[1]);

	sceneM_->draw_[5].DrawFBX(sceneM_->modelFBX_, nullptr, { 10.5f,10.5f,10.5f,10.0f });
}

void SceneGame::DrawSprite()
{
	sceneM_->debugText_.Print("[1]", 10, 10);
}

void SceneGame::DrawImgui()
{
}

void SceneGame::DrawPostEffect()
{
	//postPera.Draw();
}

void SceneGame::DrawPostEffect2()
{
	//postPera.Draw2All();
}
