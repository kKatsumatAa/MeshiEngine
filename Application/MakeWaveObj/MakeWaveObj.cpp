#include "MakeWaveObj.h"
#include "LevelManager.h"
#include "ObjectManager.h"


void IMakeWaveObj::BeginWaveStage(const Vec3& pos, const Vec2& thickExtend, float lengthExtend, float time)
{
	//メッシュ分割のウェーブ
	auto landShapes = ObjectManager::GetInstance().GetObjs(LevelManager::GetInstance().S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);
	for (auto landShape : landShapes)
	{
		landShape->BeginWave(pos, Vec2(thickExtend.x, thickExtend.y), lengthExtend, time);
	}
}
