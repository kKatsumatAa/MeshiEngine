#pragma once

/// <summary>
/// �Փ˔���p�֘A�̌^��`
/// </summary>

enum CollisionShapeType
{
	SHAPE_UNKNOWN = -1, // ���ݒ�

	//3D
	COLLISIONSHAPE_SPHERE, // ��

	COLLISIONSHAPE_PLANE, // ����

	COLLISIONSHAPE_TRIANGLE, // �O�p�`

	COLLISIONSHAPE_MESH,//���b�V��

	//2D
	COLLISIONSHAPE_CIRCLE,//�~

	COLLISIONSHAPE_RECTANGLE,//�~
};