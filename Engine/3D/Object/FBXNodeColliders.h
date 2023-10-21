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
		std::function<void(IObject3D* obj, const CollisionInfo& info)>onCollFunc_ = NULL;
		//親オブジェクトのポインタ
		IObject3D* parentObj_ = nullptr;

	public:
		void SetOnCollFunc(const std::function<void(IObject3D* obj, const CollisionInfo& info)>f) { onCollFunc_ = f; }
		void SetParentObj(IObject3D* parentObj) { parentObj_ = parentObj; }

	public:
		void OnCollision(const CollisionInfo& info)override;
	};

private:
	//外部のノード配列のポインタ
	std::vector<Node>* nodes_ = nullptr;
	//ノードのコライダー用オブジェクトの配列(これとセットで関数ポインタとかやれば部位ごとに処理できるかも)
	std::vector<std::unique_ptr<ColliderObject>> colliderObjs_;
	//オブジェクトのスケール
	float scale_ = 1.0f;
	//親オブジェクト
	IObject3D* parentObj_ = nullptr;

public://関数------------
	void Update(WorldMat* worldMat);
	void Draw();

public:
	//ノードの配列のポインタを保存
	void SetNodesPointer(std::vector<Node>* nodes) { nodes_ = nodes; }
	//スケールセット
	void SetScaleCollider(float scale) { scale_ = scale; }
	//親オブジェクトセット
	void SetParentObj(IObject3D* parentObj);
	//判定処理のセット
	void SetOnCollisionFunc(std::function<void(IObject3D* obj, const CollisionInfo& info)>onCollisionF);
	//判定の有効フラグセット
	void SetIsValidNodeColliders(bool isValidColliders);

public:
	//コライダーオブジェクトの配列を取得(ボーンのみ抽出)
	const std::vector<std::unique_ptr<ColliderObject>>& GetColliders() { return colliderObjs_; }
	//コライダーオブジェクトを名前で取得
	IObject3D* GetColliderObj(const std::string& nodeName);

public:
	//ノードの配列の一つ一つのコライダーをセットする
	void CreateNodeColliders(uint16_t attribute);
};