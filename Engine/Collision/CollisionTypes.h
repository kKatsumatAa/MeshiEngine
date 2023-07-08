#pragma once

/// <summary>
/// 衝突判定用関連の型定義
/// </summary>

enum CollisionShapeType
{
	SHAPE_UNKNOWN = -1, // 未設定

	COLLISIONSHAPE_SPHERE, // 球

	COLLISIONSHAPE_PLANE, // 平面

	COLLISIONSHAPE_TRIANGLE, // 三角形

	COLLISIONSHAPE_MESH,//メッシュ
};