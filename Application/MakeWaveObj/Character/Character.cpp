#include "Character.h"
#include "Gun.h"

using namespace DirectX;


const float Character::IS_WALL_ROT_ = 30.0f;


void Character::Damaged(int8_t damage, std::function<void()> deadFunc, std::function<void()> notDeadFunc)
{
	//ダメージ有効でなければ抜ける
	if (!isValidDamage_) { return; }

	//減る前を保存しておく
	oldHP_ = hp_;
	hp_ -= damage;

	//死んだときの処理
	if (hp_ <= 0 && deadFunc)
	{
		deadFunc();
	}
	else if (notDeadFunc)
	{
		notDeadFunc();
	}
}

Character::~Character()
{
	//死んだら武器落とす
	//（unique_ptrでobjectManagerで管理してるので、worldMat_のparent_がおかしなことになるため）
	FallHaveWeapon({ 0,0,0 }, nullptr, false);
}

void Character::Update()
{
	//カウント増やす
	counter_.AddCount();

	//武器の有効フラグも連動
	if (GetWeapon())
	{
		GetWeapon()->SetIsValid(GetIsValidUpdate());

		//死んだら武器落とす
		if (!GetIsAlive())
		{
			GetWeapon()->SetIsAlive(false);
			FallHaveWeapon(WEAPON_FALL_VEC_);
		}
	}

	ObjectFBX::Update();
}

void Character::DrawImGui(std::function<void()> imguiF)
{
	std::function<void()> f = [=]() {

		//トランスなど
		if (ImGui::TreeNode("Character")) {

			ImGui::Checkbox("isVailidDamage", &isValidDamage_);

			ImGui::Text("HP : %d", hp_);

			std::string weaponSTmp = "weapon : ";
			std::string weaponS;
			if (weapon_ == nullptr)
			{
				weaponS = weaponSTmp + "NULL";
			}
			else
			{
				weaponS = weaponSTmp + weapon_->GetObjName();
			}
			ImGui::Text(weaponS.c_str());

			ImGui::Text("isOnGround : %d", isOnGround_);

			ImGui::Text("fallVec : %.2f", fallVec_.y);

			ImGui::TreePop();
		}

		//持ってる武器のimguiも
		if (weapon_)
		{
			if (ImGui::TreeNode(weapon_->GetObjName().c_str()))
			{
				weapon_->DrawImGui();

				ImGui::TreePop();
			}
		}
		};

	//親クラスのを呼び出す
	ObjectFBX::DrawImGui(f);
}

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	//銃なら弾を撃つ位置の倍率
	if (weapon->GetObjName() == "gun")
	{
		auto gun = dynamic_cast<Gun*>(weapon);
		gun->SetShotPosExtend(shotPosExtend_);
	}

	SetWeapon(weapon);

	//シルエット解除
	weapon->SetIsSilhouette(false);

	if (localPos)
	{
		weapon_->SetLocalPos(*localPos);
	}

	weapon_->ChangeOwner(this);
}

void Character::FallHaveWeapon(const Vec3& directionVec, Vec3* localPos, bool isUpdatePos)
{
	//武器持ってたら
	if (weapon_)
	{
		FallWeapon(weapon_, directionVec, localPos, isUpdatePos);

		//武器なくす
		SetWeapon(nullptr);
	}
}

void Character::FallWeapon(Weapon* weapon, const Vec3& directionVec, Vec3* localPos, bool isUpdatePos)
{
	if (weapon)
	{
		if (localPos)
		{
			weapon->SetLocalPos(*localPos);
		}

		//移動
		if (isUpdatePos)
		{
			weapon->SetTrans(weapon->GetTrans() + directionVec);
		}
		//親ノードをなくす
		weapon->ResetParentFbxNode();
		//親などをnull
		weapon->ChangeOwner(nullptr);
		//仮で手から離れたらアイテムの属性にする
		weapon->SetAttribute(COLLISION_ATTR_ITEMS);
		weapon->SetIsThrowing(true);

		//更新
		weapon->SetFallVec(directionVec);
		if (isUpdatePos)
		{
			weapon->OldPosUpdate();
			weapon->Update();
		}
	}
}

void Character::OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump, std::function<void()>f)
{
	//落下処理
	if (!isOnGround_)
	{
		//加速
		fallVec_.y = max((fallVec_.y + FALL_ACC_ * velocityYPow), FALL_V_Y_MIN_);
		//移動
		SetTrans(GetTrans() + fallVec_ * velocityYPow);
		//ジャンプの条件トリガー満たしてなかったら
		if (f && !isJump)
		{
			f();
		}
	}
	//ジャンプ操作
	else if (isJump)
	{
		isOnGround_ = false;
		fallVec_ = { 0,JUMP_V_Y_FIST_,0 };
		//関数の処理実行
		if (f)
		{
			f();
		}
	}
	//行列更新
	IObject3D::WorldMatColliderUpdate();

	//壁との当たり判定
	QueryCallBackUpdate();

	//上端から下端までのレイキャスト用レイを準備(当たり判定は親子関係も考慮)
	Ray ray;
	ray.start = { GetWorldTrans().x,GetWorldTrans().y, GetWorldTrans().z };
	//上端
	ray.start.m128_f32[1] += LengthY;
	ray.dir = { ON_GROUND_RAY_VEC_.x,ON_GROUND_RAY_VEC_.y ,ON_GROUND_RAY_VEC_.z };
	RaycastHit info;

	//接地状態
	if (isOnGround_)
	{
		//スムーズに坂を下るための吸着処理(少し長めにすることで)
		const float adsDistance = LengthY * ADS_DISTANCE_RATE_;
		//接地を維持
		if (CollisionManager::GetInstance()->Raycast(
			ray, COLLISION_ATTR_LANDSHAPE, &info, LengthY * LENGTH_RATE_FROM_GROUND_ + adsDistance))
		{
			isOnGround_ = true;
			//めり込み分上に
			SetTrans(GetTrans() - Vec3(0, info.distance - LengthY * LENGTH_RATE_FROM_GROUND_, 0));
			//行列更新
			ObjectFBX::WorldMatColliderUpdate();
		}
		//地面がないので落下
		else
		{
			isOnGround_ = false;
			//初速を0
			fallVec_ = {};
		}
	}
	//落下状態
	else if (fallVec_.y <= 0.0f)
	{
		if (CollisionManager::GetInstance()->Raycast(
			ray, COLLISION_ATTR_LANDSHAPE, &info, LengthY * LENGTH_RATE_FROM_GROUND_))
		{
			//着地
			isOnGround_ = true;
			SetTrans(GetTrans() - Vec3(0, info.distance - LengthY * LENGTH_RATE_FROM_GROUND_, 0));
			ObjectFBX::WorldMatColliderUpdate();
		}
	}

}

void Character::QueryCallBackUpdate()
{
	//キャラの使用しているコライダーが球以外もあり得るので
	std::unique_ptr<SphereCollider> sphereColl = std::make_unique<SphereCollider>();
	sphereColl->SetObject(this);
	sphereColl->Update();

	//クエリーコールバッククラス
	class CharacterQueryCallBack :public QueryCallback
	{
	public:
		//上方向
		const Vec4 UP_VEC_ = { 0,1.0f,0,0 };

	public:
		CharacterQueryCallBack(Sphere* sphere) : sphere(sphere) {};

		// 衝突時コールバック関数
		bool OnQueryHit(const QueryHit& info)
		{
			//上方向
			const XMVECTOR UP_VEC = { UP_VEC_.x,UP_VEC_.y,UP_VEC_.z,UP_VEC_.z };
			//排斥方向
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//上方向と排斥方向の角度差のコサイン値
			float cos = XMVector3Dot(rejectDir, UP_VEC).m128_f32[0];

			// 地面判定しきい値角度
			const float threshold = cosf(XMConvertToRadians(IS_WALL_ROT_));
			//角度差によって天井または地面と判定される場合を除いて
			if (-threshold < cos && cos < threshold) {
				sphere->center += info.reject;
				move += info.reject;
			}

			return true;
		}

		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
	};

	//クエリーコールバックの関数オブジェクト
	CharacterQueryCallBack callback(sphereColl.get());

	// 球と地形の交差を全検索
	CollisionManager::GetInstance()->QuerySphere(*sphereColl.get(), &callback, COLLISION_ATTR_LANDSHAPE);
	// 交差による排斥分動かす
	SetTrans(GetTrans()
		+ Vec3(callback.move.m128_f32[0], callback.move.m128_f32[1], callback.move.m128_f32[2]));
	// ワールド行列更新
	ObjectFBX::WorldMatColliderUpdate();
}

void Character::SetIsValid(bool isValid)
{
	ObjectFBX::SetIsValid(isValid);
	//武器の有効フラグも連動
	if (GetWeapon())
	{
		GetWeapon()->SetIsValid(GetIsValidUpdate());
	}
}

bool Character::CheckRayOfEyeHit(const Vec3& dir, float length, uint16_t attr, RaycastHit* info)
{
	//攻撃できるかどうか
//レイにプレイヤーの正面ベクトル入れる
	Ray ray;
	ray.dir = { dir.x,dir.y,dir.z };
	ray.start = { GetWorldTrans().x,GetWorldTrans().y ,GetWorldTrans().z };

	//正面ベクトルに何かあるか
	bool isRayHit = CollisionManager::GetInstance()->Raycast(ray, attr, info, length);

	return isRayHit;
}


//----------------------------------------------------------------------
std::string Character::GetPartName(const std::string& boneName)
{
	std::string partName = "";

	//右半身
	if (boneName.find("Right") != std::string::npos)
	{
		if (boneName.find(BonePartName::SHOULDER) != std::string::npos ||
			boneName.find(BonePartName::ARM) != std::string::npos ||
			boneName.find(BonePartName::HAND) != std::string::npos)
		{
			partName = PartName::RIGHT_ARM;
		}
		else
		{
			partName = PartName::RIGHT_LEG;
		}
	}
	//左半身
	else if (boneName.find("Left") != std::string::npos)
	{
		if (boneName.find(BonePartName::SHOULDER) != std::string::npos ||
			boneName.find(BonePartName::ARM) != std::string::npos ||
			boneName.find(BonePartName::HAND) != std::string::npos)
		{
			partName = PartName::LEFT_ARM;
		}
		else
		{
			partName = PartName::LEFT_LEG;
		}
	}
	//頭
	else if (boneName.find(BonePartName::HEAD) != std::string::npos)
	{
		partName = PartName::HEAD;
	}
	//胴体
	else
	{
		partName = PartName::BODY;
	}

	return partName;
}

bool Character::GetIsPartStillAttached(const std::string& partName)
{
	for (auto& mesh : model_->GetMeshes())
	{
		if (mesh->GetName() == partName)
		{
			return true;
		}
	}

	return false;
}

bool Character::GetIsHavingBottomBody()
{
	//両足あれば
	if (GetIsPartStillAttached(PartName::LEFT_LEG) ||
		GetIsPartStillAttached(PartName::RIGHT_LEG))
	{
		return true;
	}

	return false;
}

bool Character::GetIsHavingHead()
{
	if (GetIsPartStillAttached(PartName::HEAD))
	{
		return true;
	}

	return false;
}

void Character::InvalidPartNodeColliders(const std::string& partName)
{
	std::string leftRight = "";

	//部位によってボーンごとの判定無効に
	if (partName == PartName::HEAD)
	{
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::HEAD);
	}
	else if (partName == PartName::LEFT_ARM)
	{
		leftRight = "Left";
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::HAND);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::ARM);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::SHOULDER);
	}
	else if (partName == PartName::RIGHT_ARM)
	{
		leftRight = "Right";
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::HAND);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::ARM);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::SHOULDER);
	}
	else if (partName == PartName::LEFT_LEG)
	{
		leftRight = "Left";
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::TOE);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::FOOT);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::LEG);
	}
	else if (partName == PartName::RIGHT_LEG)
	{
		leftRight = "Right";
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::TOE);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::FOOT);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::LEG);
	}
	//胴体
	else
	{
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::SPINE);
		nodeColliders_.InvalidContainingStrColliders(leftRight, BonePartName::NECK);
	}
}
