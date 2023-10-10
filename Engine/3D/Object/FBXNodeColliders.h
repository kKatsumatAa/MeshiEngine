#pragma once
#include"Object.h"


//fbxのノードとコライダー
class FBXNodeColliders
{
public:
	//コライダー用オブジェクト
	class ColliderObject : public Object
	{
	private:
		//onCollisionが呼ばれたときに呼び出す関数のポインタ
		std::function<void(const CollisionInfo& info)>onCollFunc_ = NULL;

	public:
		void SetOnCollFunc(const std::function<void(const CollisionInfo& info)>f) { onCollFunc_ = f; }

	public:
		void OnCollision(const CollisionInfo& info)override { if (onCollFunc_) { onCollFunc_(info); } }
	};

private:
	//外部のノード配列のポインタ
	std::vector<Node>* nodes_ = nullptr;
	//ノードのコライダー用オブジェクトの配列(これとセットで関数ポインタとかやれば部位ごとに処理できるかも)
	std::vector<std::unique_ptr<ColliderObject>> colliderObjs_;
	//ノードのパラメータ倍率（モデルが大きすぎたりするので）
	float nodeParamMagnif_ = 1.0f;
	//オブジェクトのスケール
	float scale_ = 1.0f;

public://関数------------
	void Update(WorldMat* worldMat);
	void Draw();

public:
	//ノードの配列のポインタを保存
	void SetNodesPointer(std::vector<Node>* nodes) { nodes_ = nodes; }
	//パラメータ倍率をセット
	void SetNodeParamMagnif(float magnif) { nodeParamMagnif_ = magnif; }
	//スケールセット
	void SetScaleCollider(float scale) { scale_ = scale; }

public:
	//コライダーオブジェクトの配列を取得(ノードと同じ順番)
	const std::vector<std::unique_ptr<ColliderObject>>& GetColliders() { return colliderObjs_; }

public:
	//ノードの配列の一つ一つのコライダーをセットする
	void CreateNodeColliders();
};