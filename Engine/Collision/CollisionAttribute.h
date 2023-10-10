#pragma once
#include<memory>

/// <summary>
/// 蠖薙◆繧雁愛螳壼ｱ樊ｧ
/// </summary>
const uint16_t COLLISION_ATTR_LANDSHAPE = (uint16_t)(0b1 << 0); // 蝨ｰ蠖｢
const uint16_t COLLISION_ATTR_ALLIES = (uint16_t)(0b1 << 1); // 蜻ｳ譁ｹ髯｣蝟ｶ
const uint16_t COLLISION_ATTR_ENEMYS = (uint16_t)(0b1 << 2); // 謨ｵ髯｣蝟ｶ
const uint16_t COLLISION_ATTR_ITEMS = (uint16_t)(0b1 << 3); // 繧｢繧､繝・Β
const uint16_t COLLISION_ATTR_ALL = (uint16_t)(0xffff);// 蜈ｨ縺ｦ
