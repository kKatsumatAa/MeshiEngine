#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "Object.h"

/// <summary>
/// �R���C�_�[���N���X�i�I�u�W�F�N�g�����R���C�_�[�j
/// </summary>
class BaseCollider
{
public:
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	inline void SetObject(Object* object) {
		this->object = object;
	}

	inline Object* GetObject3d() {
		return object;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	inline CollisionShapeType GetShapeType() { return shapeType; }

	//�����L���ɂ��邩�ǂ����̃t���O
	inline void SetIsValid(bool isValid) { this->isValid = isValid; }
	inline bool GetIsValid() { return isValid; }

	//2d���ǂ����i�X�v���C�g�p�j
	inline void SetIs2D(bool is2D) { this->is2D = is2D; }
	inline bool GetIs2D() { return is2D; }

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	/// <param name="info">�Փˏ��</param>
	inline void OnCollision(const CollisionInfo& info) {
		object->OnCollision(info);
	}

protected:
	//�������Object�̃|�C���^������
	Object* object = nullptr;
	// �`��^�C�v
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//�����邩���Ȃ���
	bool isValid = true;
	//�X�v���C�g�p�̓񎟌����ǂ����t���O
	bool is2D = false;
};

