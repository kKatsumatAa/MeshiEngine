#include "MeshCollider.h"
#include "Collision.h"

using namespace DirectX;

void MeshCollider::ConstructTriangles(Model* model)
{
	//三角形リストをクリア
	triangles_.clear();
	//モデルの持つメッシュリストを取得
	const std::vector<std::unique_ptr<Mesh>>& meshes = model->GetMeshes();
	//現在のメッシュの開始三角形番号を入れておく変数
	int32_t start = 0;

	//全メッシュについて順（メッシュ毎）に処理する
	std::vector<std::unique_ptr<Mesh>>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = it->get();
		const std::vector<Mesh::VertexPosNormalUvSkin>& vertices = mesh->GetVertices();
		const std::vector<unsigned short>& indices = mesh->GetIndices();

		//インデックスは三角形の数*3あるので、そこからメッシュ内の三角形の数を逆算
		size_t triangleNum = indices.size() / 3;
		//現在のメッシュの三角形の数だけ三角形リストにスペースを追加
		triangles_.resize(triangles_.size() + triangleNum);
		//全三角形について順に処理
		for (int i = 0; i < triangleNum; i++) {
			//今から計算する三角形の参照
			Triangle& tri = triangles_[start + i];
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];

			//三角形の三頂点の座標を代入
			tri.p0 = {
				vertices[idx0].pos.x,
				vertices[idx0].pos.y,
				vertices[idx0].pos.z,
				1 };
			tri.p1 = {
				vertices[idx1].pos.x,
				vertices[idx1].pos.y,
				vertices[idx1].pos.z,
				1 };
			tri.p2 = {
				vertices[idx2].pos.x,
				vertices[idx2].pos.y,
				vertices[idx2].pos.z,
				1 };
			//三頂点から法線を計算
			tri.ComputeNormal();
		}
		//次のメッシュは、今までの三角形番号の次から使う
		start += (int32_t)triangleNum;
	}
}

void MeshCollider::Update()
{
	//逆行列を計算
	M4 m4 = GetObject3d()->GetMatWorld();
	XMMATRIX m;
	m4.MatIntoXMMATRIX(m);

	invMatWorld_ = XMMatrixInverse(nullptr, m);
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, DirectX::XMVECTOR* inter)
{
	// オブジェクトのローカル座標系（このオブジェクトを中心とした座標系？）
	//での球を得る（半径はXスケールを参照)
	Sphere localSphere;
	localSphere.center = XMVector3Transform(sphere.center, invMatWorld_);
	//localSphere.radius *= XMVector3Length(invMatWorld_.r[0]).m128_f32[0];
	localSphere.radius = sphere.radius * XMVector3Length(invMatWorld_.r[0]).m128_f32[0];

	//ローカル座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		//球と三角形の当たり判定
		if (Collision::CheckSphere2Triangle(localSphere, triangle, inter)) {
			if (inter) {
				XMMATRIX matWorld;
				GetObject3d()->GetMatWorld().MatIntoXMMATRIX(matWorld);
				//ワールド座標で(逆行列でローカルにしたのを戻す?)
				*inter = XMVector3Transform(*inter, matWorld);
			}
			return true;
		}
	}

	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, DirectX::XMVECTOR* inter)
{
	// オブジェクトのローカル座標系でのレイを得る
	Ray localRay;
	localRay.start = XMVector3Transform(ray.start, invMatWorld_);
	localRay.dir = XMVector3TransformNormal(ray.dir, invMatWorld_);

	//最短距離を保存する
	float minDistance = 999999;
	bool isReturn = false;

	//ローカル座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();

	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		XMVECTOR tempInter;
		//レイと三角形の当たり判定
		if (Collision::CheckRay2Triangle(localRay, triangle, nullptr, &tempInter)) {

			//ワールド座標系での交点を得る
			XMMATRIX matWorld;
			GetObject3d()->GetMatWorld().MatIntoXMMATRIX(matWorld);
			tempInter = XMVector3Transform(tempInter, matWorld);

			//ワールド座標系での交点とレイの距離を計算
			XMVECTOR length = tempInter - ray.start;
			Vec3 lengthV = { length.m128_f32[0],length.m128_f32[1], length.m128_f32[2] };

			//今までで距離が一番短ければ記録
			if (lengthV.GetLength() < minDistance)
			{
				minDistance = lengthV.GetLength();
				if (distance) {
					//交点とレイの距離を計算
					*distance = XMVector3Dot(length, ray.dir).m128_f32[0];
				}
				if (inter) {
					//衝突点
					*inter = tempInter;
				}
			}
			//衝突したフラグをオン
			isReturn = true;
		}
	}

	return isReturn;
}
