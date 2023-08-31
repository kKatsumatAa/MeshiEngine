#include "MeshCollider.h"
#include "Collision.h"

using namespace DirectX;

void MeshCollider::ConstructTriangles(IModel* model)
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
				vertices[idx0].pos.x_,
				vertices[idx0].pos.y_,
				vertices[idx0].pos.z_,
				1 };
			tri.p1 = {
				vertices[idx1].pos.x_,
				vertices[idx1].pos.y_,
				vertices[idx1].pos.z_,
				1 };
			tri.p2 = {
				vertices[idx2].pos.x_,
				vertices[idx2].pos.y_,
				vertices[idx2].pos.z_,
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
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, DirectX::XMVECTOR* inter,
	DirectX::XMVECTOR* reject)
{

	//ワールド座標
	XMMATRIX matWorld;
	GetObject3d()->GetMatWorld().MatIntoXMMATRIX(matWorld);

	//ワールド座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		//ワールドに変換
		Triangle worldTriangle = triangle;

		worldTriangle.p0 = XMVector3Transform({ triangle.p0 }, matWorld);
		worldTriangle.p1 = XMVector3Transform({ triangle.p1 }, matWorld);
		worldTriangle.p2 = XMVector3Transform({ triangle.p2 }, matWorld);
		worldTriangle.ComputeNormal();

		//球と三角形の当たり判定
		if (Collision::CheckSphere2Triangle(sphere, worldTriangle, inter, reject)) {
			return true;
		}
	}

	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, DirectX::XMVECTOR* inter)
{
	//最短距離を保存する
	float minDistance = 999999;
	bool isReturn = false;

	//ワールド座標
	XMMATRIX matWorld;
	GetObject3d()->GetMatWorld().MatIntoXMMATRIX(matWorld);

	//ワールド座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		//ワールドに変換
		Triangle worldTriangle = triangle;

		worldTriangle.p0 = XMVector3Transform({ triangle.p0 }, matWorld);
		worldTriangle.p1 = XMVector3Transform({ triangle.p1 }, matWorld);
		worldTriangle.p2 = XMVector3Transform({ triangle.p2 }, matWorld);
		worldTriangle.ComputeNormal();


		XMVECTOR tempInter;
		//レイと三角形の当たり判定
		if (Collision::CheckRay2Triangle(ray, worldTriangle, nullptr, &tempInter)) {

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
