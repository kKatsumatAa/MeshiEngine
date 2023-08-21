#include "Character.h"

using namespace DirectX;


const float Character::IS_WALL_ROT_ = 30.0f;


void Character::Damaged(int32_t damage, std::function<void()> deadFunc)
{
	//ダメージ有効でなければ抜ける
	if (!isValidDamage_) { return; }

	hp_ -= damage;

	//死んだときの処理
	if (hp_ <= 0)
	{
		deadFunc();
	}
}

void Character::Update()
{
	//武器の有効フラグも連動
	if (GetWeapon())
	{
		GetWeapon()->SetIsValid(GetIsValid());
	}
	//死んだら武器落とす
	else
	{
		FallWeapon({ 0,0,0 });
	}

	Object::Update();
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

			ImGui::Text("fallVec : %.2f", fallVec_.y_);

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

	Object::DrawImGui(f);
}

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	SetWeapon(weapon);

	//シルエット解除
	weapon->SetIsSilhouette(false);

	if (localPos)
	{
		weapon_->SetLocalPos(*localPos);
	}

	weapon_->ChangeOwner(this);
	//所有者の判定属性を入れる
	if (GetCollider())
	{
		weapon_->SetAttribute(GetCollider()->GetAttribute());
	}
}

void Character::FallWeapon(const Vec3& directionVec, Vec3* localPos)
{
	//武器持ってたら
	if (weapon_)
	{
		if (localPos)
		{
			GetWeapon()->SetLocalPos(*localPos);
		}

		GetWeapon()->SetFallVec(directionVec);
		GetWeapon()->ChangeOwner(nullptr);
		//仮で手から離れたらアイテムの属性にする
		weapon_->SetAttribute(COLLISION_ATTR_ITEMS);
		weapon_->SetIsThrowing(true);
		weapon_->Update();
		SetWeapon(nullptr);
	}
}

void Character::OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump, std::function<void()>f)
{
	//落下処理
	if (!isOnGround_)
	{
		//加速
		fallVec_.y_ = max((fallVec_.y_ + FALL_ACC_ * velocityYPow), FALL_V_Y_MIN_);
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
	Object::WorldMatColliderUpdate();

	//壁との当たり判定
	QueryCallBackUpdate();

	//上端から下端までのレイキャスト用レイを準備(当たり判定は親子関係も考慮)
	Ray ray;
	ray.start = { GetWorldTrans().x_,GetWorldTrans().y_, GetWorldTrans().z_ };
	//上端
	ray.start.m128_f32[1] += LengthY;
	ray.dir = { 0,-1.0f,0 };
	RaycastHit info;

	//接地状態
	if (isOnGround_)
	{
		//スムーズに坂を下るための吸着処理(少し長めにすることで)
		const float adsDistance = LengthY * 0.2f;
		//接地を維持
		if (CollisionManager::GetInstance()->Raycast(
			ray, COLLISION_ATTR_LANDSHAPE, &info, LengthY * 2.0f + adsDistance))
		{
			isOnGround_ = true;
			//めり込み分上に
			SetTrans(GetTrans() - Vec3(0, info.distance - LengthY * 2.0f, 0));
			//行列更新
			Object::WorldMatColliderUpdate();
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
	else if (fallVec_.y_ <= 0.0f)
	{
		if (CollisionManager::GetInstance()->Raycast(
			ray, COLLISION_ATTR_LANDSHAPE, &info, LengthY * 2.0f))
		{
			//着地
			isOnGround_ = true;
			SetTrans(GetTrans() - Vec3(0, info.distance - LengthY * 2.0f, 0));
			Object::WorldMatColliderUpdate();
		}
	}

}

void Character::QueryCallBackUpdate()
{
	//球コライダー取得
	SphereCollider* sphereColl = dynamic_cast<SphereCollider*>(GetCollider());

	//クエリーコールバッククラス
	class CharacterQueryCallBack :public QueryCallback
	{
	public:
		CharacterQueryCallBack(Sphere* sphere) : sphere(sphere) {};

		// 衝突時コールバック関数
		bool OnQueryHit(const QueryHit& info)
		{
			//上方向
			const XMVECTOR up = { 0,1.0f,0,0 };
			//排斥方向
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//上方向と排斥方向の角度差のコサイン値
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

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
	CharacterQueryCallBack callback(sphereColl);

	// 球と地形の交差を全検索
	CollisionManager::GetInstance()->QuerySphere(*sphereColl, &callback, COLLISION_ATTR_LANDSHAPE);
	// 交差による排斥分動かす
	SetTrans(GetTrans()
		+ Vec3(callback.move.m128_f32[0], callback.move.m128_f32[1], callback.move.m128_f32[2]));
	// ワールド行列更新
	Object::WorldMatColliderUpdate();
}

void Character::SetIsValid(bool isValid)
{
	Object::SetIsValid(isValid);
	//武器の有効フラグも連動
	if (GetWeapon())
	{
		GetWeapon()->SetIsValid(GetIsValid());
	}
}
