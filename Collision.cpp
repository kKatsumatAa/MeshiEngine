#include "Collision.h"
#include "Vec3.h"
using namespace DirectX;

bool Collision::CheckSphere2Sphere(const Sphere& sphere, const Sphere& sphere2)
{
    XMVECTOR length = sphere.centor - sphere2.centor;
    Vec3 vec = { length.m128_f32[0],length.m128_f32[1],length.m128_f32[2] };

    if (vec.GetLength() <= sphere.radius + sphere2.radius)
        return true;

    return false;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter)
{
    //"座標系の原点"から球の中心座標への距離
    XMVECTOR distV = XMVector3Dot(sphere.centor, plane.normal);
    //平面の原点距離を減算することで、平面と球の中心との距離が出る
    float dist = distV.m128_f32[0] - plane.distance;
    //距離の絶対値が半径より大きければ当たっていない
    if (fabsf(dist) > sphere.radius)return false;

    //疑似交点を計算
    if (inter)
    {
        //平面上の最近接点を、疑似交点とする
        *inter = -dist * plane.normal + sphere.centor;
    }

    return true;
}
