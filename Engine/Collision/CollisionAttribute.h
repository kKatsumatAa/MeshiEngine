#pragma once

/// <summary>
/// �����蔻�葮��
/// </summary>
const uint16_t COLLISION_ATTR_LANDSHAPE = 0b1 << 0; // �n�`
const uint16_t COLLISION_ATTR_ALLIES = 0b1 << 1; // �����w�c
const uint16_t COLLISION_ATTR_ENEMYS = 0b1 << 2; // �G�w�c
const uint16_t COLLISION_ATTR_ITEMS = 0b1 << 3; // �A�C�e��
const uint16_t COLLISION_ATTR_ALL = 0xffff; // �S��