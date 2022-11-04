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
    //"���W�n�̌��_"���狅�̒��S���W�ւ̋���
    XMVECTOR distV = XMVector3Dot(sphere.centor, plane.normal);
    //���ʂ̌��_���������Z���邱�ƂŁA���ʂƋ��̒��S�Ƃ̋������o��
    float dist = distV.m128_f32[0] - plane.distance;
    //�����̐�Βl�����a���傫����Γ������Ă��Ȃ�
    if (fabsf(dist) > sphere.radius)return false;

    //�^����_���v�Z
    if (inter)
    {
        //���ʏ�̍ŋߐړ_���A�^����_�Ƃ���
        *inter = -dist * plane.normal + sphere.centor;
    }

    return true;
}
