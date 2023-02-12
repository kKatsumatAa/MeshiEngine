#pragma once
#include "Object.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"
#include "PlaneCollider.h"

class Player :
    public Object
{
public:
    /// <summary>
    /// �I�u�W�F�N�g����
    /// </summary>
    /// <param name="model"></param>
    /// <returns></returns>
    static Player* Create();

public:

    bool Initialize() override;

    void Update() override;

    void OnCollision(const CollisionInfo& info) override;
};

