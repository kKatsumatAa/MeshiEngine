#include "FBXNodeColliders.h"
#include "SphereCollider.h"

using namespace DirectX;


//----------------------------------------------
void FBXNodeColliders::ColliderObject::OnCollision(const CollisionInfo& info)
{
	if (onCollFunc_)
	{
		onCollFunc_(this, info);
	}
}

IObject3D* FBXNodeColliders::GetColliderObj(const std::string& nodeName)
{
	for (uint16_t i = 0; i < (uint16_t)colliderObjs_.size(); i++)
	{
		if (colliderObjs_[i])
		{
			//一致する文字列があればその当たり判定用オブジェクトを返す
			if (colliderObjs_[i]->GetObjName().find(nodeName) != std::string::npos)
			{
				return colliderObjs_[i].get();
			}
		}
	}

	return nullptr;
}

//---------------------------------------------
void FBXNodeColliders::CreateNodeColliders(uint16_t attribute)
{
	if (nodes_ == nullptr)
	{
		return;
	}

	//ノードのコライダーをセットしていく
	for (auto node : *nodes_)
	{
		//コライダー用オブジェクトクラス
		std::unique_ptr<ColliderObject>obj = nullptr;

		//ボーンだったら部位ごとにコライダー追加
		if (node.attribute == FbxNodeAttribute::eSkeleton)
		{
			obj = std::make_unique<ColliderObject>();
			obj->SetObjName(node.name);
			std::unique_ptr<SphereCollider>collider = std::make_unique<SphereCollider>();
			collider->SetAttribute(attribute);
			obj->SetCollider(std::move(collider));
		}

		//要素追加
		colliderObjs_.push_back(std::move(obj));
	}
}

//---------------------------------
void FBXNodeColliders::Update(WorldMat* worldMat)
{
	if (nodes_ == nullptr || worldMat == nullptr)
	{
		return;
	}

	//行列計算
	worldMat->CalcAllTreeMat();

	//親のスケールを無効
	Vec3 pScale = worldMat->scale_;

	WorldMat worldMatL = *worldMat;
	worldMatL.scale_ = Vec3(parentObj_->GetModel()->GetScaleExtend() * worldMat->scale_.x,
		parentObj_->GetModel()->GetScaleExtend() * worldMat->scale_.y,
		parentObj_->GetModel()->GetScaleExtend() * worldMat->scale_.z);
	worldMatL.CalcWorldMat();

	//ノードのコライダーのパラメータを更新していく
	for (uint16_t i = 0; i < (uint16_t)colliderObjs_.size(); i++)
	{
		if (colliderObjs_[i])
		{
			Node node = (*nodes_)[i];

			//親の行列をxmmatrixに入れる
			XMMATRIX xMatP;
			worldMatL.matWorld_.MatIntoXMMATRIX(xMatP);

			//ボーンの行列を入れる
			XMMATRIX xMatNodeG = node.globalTransform;

			M4 mat;
			//matにボーンのグローバルトランスフォーム*モデルの所有者のワールド行列入れる
			mat.PutInXMMATRIX(xMatNodeG * xMatP);
			//ローカルの一つ上の親行列としてセット
			colliderObjs_[i]->SetLocalParentMat(mat);

			//コライダーのパラメータ更新
			//コライダーは小さく,モデルの縮小率反映するため
			colliderObjs_[i]->SetScale(Vec3(scale_, scale_, scale_));
			colliderObjs_[i]->ColliderUpdate();

			//ボーンを反映した位置をセット(parent_は設定してないのでできる)
			colliderObjs_[i]->SetLocalTrans(colliderObjs_[i]->GetWorldTrans());
		}
	}
}

void FBXNodeColliders::Draw()
{
	//分かりやすいように描画
	for (auto collObj = colliderObjs_.begin(); collObj != colliderObjs_.end();
		collObj++)
	{
		if (collObj->get())
		{
			collObj->get()->DrawSphere();
		}
	}
}

//--------------------------------------------------------------------------
void FBXNodeColliders::SetParentObj(IObject3D* parentObj)
{
	parentObj_ = parentObj;

	if (colliderObjs_.size())
	{
		for (auto collObj = colliderObjs_.begin(); collObj != colliderObjs_.end();
			collObj++)
		{
			//親オブジェクトをセット
			if (collObj->get())
			{
				collObj->get()->SetParentObj(parentObj);
			}
		}
	}
}

void FBXNodeColliders::SetOnCollisionFunc(std::function<void(IObject3D* obj, const CollisionInfo& info)> onCollisionF)
{
	if (colliderObjs_.size())
	{
		for (auto collObj = colliderObjs_.begin(); collObj != colliderObjs_.end();
			collObj++)
		{
			//当たり判定処理をセット
			if (collObj->get())
			{
				collObj->get()->SetOnCollFunc(onCollisionF);
			}
		}
	}
}
