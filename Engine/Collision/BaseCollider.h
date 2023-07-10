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
	friend class CollisionManager;

public:
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	inline void SetObject(Object* object) {
		object_ = object;
	}

	inline Object* GetObject3d() {
		return object_;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	inline const CollisionShapeType& GetShapeType() { return shapeType_; }

	//�����L���ɂ��邩�ǂ����̃t���O
	inline void SetIsValid(bool isValid) { isValid_ = isValid; }
	inline bool GetIsValid() { return isValid_; }

	//2d���ǂ����i�X�v���C�g�p�j
	inline void SetIs2D(bool is2D) { is2D_ = is2D; }
	inline bool GetIs2D() { return is2D_; }

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	/// <param name="info">�Փˏ��</param>
	inline void OnCollision(const CollisionInfo& info) {
		object_->OnCollision(info);
	}

	//���葮���Z�b�g
	inline void SetAttribute(uint16_t attribute) { attribute_ = attribute; }
	//�����蔻��̑����𑝂₷
	inline void AttAttribute(uint16_t attribute) { attribute_ |= attribute; }
	//
	inline void RemoveAttribute(uint16_t attribute) { attribute_ &= attribute; }
	//
	inline uint16_t GetAttribute() { return attribute_; }


protected:
	//�������Object�̃|�C���^������
	Object* object_ = nullptr;
	// �`��^�C�v
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	//�����邩���Ȃ���
	bool isValid_ = true;
	//�X�v���C�g�p�̓񎟌����ǂ����t���O
	bool is2D_ = false;
	//�����蔻�葮��
	uint16_t attribute_ = 0b111111111111111;
};

