#pragma once
#include <cstdint>
#include "ViewMat.h"
#include "WorldMat.h"
#include "WorldMat.h"
#include "ProjectionMat.h"

//“–‚½‚è”»’è—pƒtƒ‰ƒO
const uint32_t kCollisionAttributePlayer = 0b1;//~0001(1)
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;//~0010(2)