#pragma once
#include"IObject3D.h"

//ステージに波紋を出すオブジェクト
class IMakeWaveObj
{
protected:
	//ステージに波紋
	void BeginWaveStage(const Vec3& pos, const Vec2& thickExtend, float lengthExtend, float time);
};