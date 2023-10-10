#include "FBXNodeColliders.h"
#include "SphereCollider.h"

using namespace DirectX;

//---------------------------------------------
void FBXNodeColliders::CreateNodeColliders()
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
			//obj->SetCollider(std::make_unique<SphereCollider>());
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
	worldMat->scale_ = Vec3(nodeParamMagnif_ * worldMat->scale_.x,
		nodeParamMagnif_ * worldMat->scale_.y,
		nodeParamMagnif_ * worldMat->scale_.z);
	worldMat->CalcWorldMat();

	//ノードのコライダーのパラメータを更新していく
	for (uint16_t i = 0; i < (uint16_t)colliderObjs_.size(); i++)
	{
		if (colliderObjs_[i])
		{
			Node node = (*nodes_)[i];

			//親の行列をxmmatrixに入れる
			XMMATRIX xMatP;
			worldMat->matWorld_.MatIntoXMMATRIX(xMatP);

			//モデルが大きすぎたりするので
			XMMATRIX xMatNodeG = node.globalTransform;
			/*		xMatNodeG.r[3].m128_f32[0] *= nodeParamMagnif_;
					xMatNodeG.r[3].m128_f32[1] *= nodeParamMagnif_;
					xMatNodeG.r[3].m128_f32[2] *= nodeParamMagnif_;*/

			M4 mat;
			//matにボーンのグローバルトランスフォーム*モデルの所有者のワールド行列入れる
			mat.PutInXMMATRIX(xMatNodeG * xMatP);

			//スケール反映した行列とボーンや親の行列をかける
			colliderObjs_[i]->SetScale({ scale_ ,scale_ ,scale_ });
			colliderObjs_[i]->CalcWorldMat();
			colliderObjs_[i]->SetMatWorld(colliderObjs_[i]->GetMatWorld() * mat);
		}
	}

	//スケール戻す
	worldMat->scale_ = pScale;
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