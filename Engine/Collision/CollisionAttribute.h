#pragma once

/// <summary>
/// 当たり判定属性
/// </summary>
const uint16_t COLLISION_ATTR_LANDSHAPE = 0b1 << 0; // 地形
const uint16_t COLLISION_ATTR_ALLIES = 0b1 << 1; // 味方陣営
const uint16_t COLLISION_ATTR_ENEMYS = 0b1 << 2; // 敵陣営
const uint16_t COLLISION_ATTR_ITEMS = 0b1 << 3; // アイテム
const uint16_t COLLISION_ATTR_ALL = 0xffff; // 全て