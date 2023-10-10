#include "MeshCollider.h"
#include "Collision.h"

using namespace DirectX;

void MeshCollider::ConstructTriangles(IModel* model)
{
	//荳芽ｧ貞ｽ｢繝ｪ繧ｹ繝医ｒ繧ｯ繝ｪ繧｢
	triangles_.clear();
	//繝｢繝・Ν縺ｮ謖√▽繝｡繝・す繝･繝ｪ繧ｹ繝医ｒ蜿門ｾ・
	const std::vector<std::unique_ptr<Mesh>>& meshes = model->GetMeshes();
	//迴ｾ蝨ｨ縺ｮ繝｡繝・す繝･縺ｮ髢句ｧ倶ｸ芽ｧ貞ｽ｢逡ｪ蜿ｷ繧貞・繧後※縺翫￥螟画焚
	int32_t start = 0;

	//蜈ｨ繝｡繝・す繝･縺ｫ縺､縺・※鬆・ｼ医Γ繝・す繝･豈趣ｼ峨↓蜃ｦ逅・☆繧・
	std::vector<std::unique_ptr<Mesh>>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = it->get();
		const std::vector<Mesh::VertexPosNormalUvSkin>& vertices = mesh->GetVertices();
		const std::vector<unsigned short>& indices = mesh->GetIndices();

		//繧､繝ｳ繝・ャ繧ｯ繧ｹ縺ｯ荳芽ｧ貞ｽ｢縺ｮ謨ｰ*3縺ゅｋ縺ｮ縺ｧ縲√◎縺薙°繧峨Γ繝・す繝･蜀・・荳芽ｧ貞ｽ｢縺ｮ謨ｰ繧帝・ｮ・
		size_t triangleNum = indices.size() / 3;
		//迴ｾ蝨ｨ縺ｮ繝｡繝・す繝･縺ｮ荳芽ｧ貞ｽ｢縺ｮ謨ｰ縺縺台ｸ芽ｧ貞ｽ｢繝ｪ繧ｹ繝医↓繧ｹ繝壹・繧ｹ繧定ｿｽ蜉
		triangles_.resize(triangles_.size() + triangleNum);
		//蜈ｨ荳芽ｧ貞ｽ｢縺ｫ縺､縺・※鬆・↓蜃ｦ逅・
		for (int i = 0; i < triangleNum; i++) {
			//莉翫°繧芽ｨ育ｮ励☆繧倶ｸ芽ｧ貞ｽ｢縺ｮ蜿ら・
			Triangle& tri = triangles_[start + i];
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];

			//荳芽ｧ貞ｽ｢縺ｮ荳蛾らせ縺ｮ蠎ｧ讓吶ｒ莉｣蜈･
			tri.iP0 = {
				vertices[idx0].pos.x,
				vertices[idx0].pos.y,
				vertices[idx0].pos.z,
				1 };
			tri.iP1 = {
				vertices[idx1].pos.x,
				vertices[idx1].pos.y,
				vertices[idx1].pos.z,
				1 };
			tri.iP2 = {
				vertices[idx2].pos.x,
				vertices[idx2].pos.y,
				vertices[idx2].pos.z,
				1 };
			//荳蛾らせ縺九ｉ豕慕ｷ壹ｒ險育ｮ・
			tri.ComputeNormal();
		}
		//谺｡縺ｮ繝｡繝・す繝･縺ｯ縲∽ｻ翫∪縺ｧ縺ｮ荳芽ｧ貞ｽ｢逡ｪ蜿ｷ縺ｮ谺｡縺九ｉ菴ｿ縺・
		start += (int32_t)triangleNum;
	}
}

void MeshCollider::Update()
{
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, DirectX::XMVECTOR* inter,
	DirectX::XMVECTOR* reject)
{

	//繝ｯ繝ｼ繝ｫ繝牙ｺｧ讓・
	XMMATRIX matWorld;
	GetObject3d()->GetMatWorld().MatIntoXMMATRIX(matWorld);

	//繝ｯ繝ｼ繝ｫ繝牙ｺｧ讓咏ｳｻ縺ｧ莠､蟾ｮ繧偵メ繧ｧ繝・け
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		//繝ｯ繝ｼ繝ｫ繝峨↓螟画鋤
		Triangle worldTriangle = triangle;

		worldTriangle.iP0 = XMVector3Transform({ triangle.iP0 }, matWorld);
		worldTriangle.iP1 = XMVector3Transform({ triangle.iP1 }, matWorld);
		worldTriangle.iP2 = XMVector3Transform({ triangle.iP2 }, matWorld);
		worldTriangle.ComputeNormal();

		//逅・→荳芽ｧ貞ｽ｢縺ｮ蠖薙◆繧雁愛螳・
		if (Collision::CheckSphere2Triangle(sphere, worldTriangle, inter, reject)) {
			return true;
		}
	}

	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, DirectX::XMVECTOR* inter)
{
	//譛遏ｭ霍晞屬繧剃ｿ晏ｭ倥☆繧・
	float minDistance = 999999;
	bool isReturn = false;

	//繝ｯ繝ｼ繝ｫ繝牙ｺｧ讓・
	XMMATRIX matWorld;
	GetObject3d()->GetMatWorld().MatIntoXMMATRIX(matWorld);

	//繝ｯ繝ｼ繝ｫ繝牙ｺｧ讓咏ｳｻ縺ｧ莠､蟾ｮ繧偵メ繧ｧ繝・け
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();
	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		//繝ｯ繝ｼ繝ｫ繝峨↓螟画鋤
		Triangle worldTriangle = triangle;

		worldTriangle.iP0 = XMVector3Transform({ triangle.iP0 }, matWorld);
		worldTriangle.iP1 = XMVector3Transform({ triangle.iP1 }, matWorld);
		worldTriangle.iP2 = XMVector3Transform({ triangle.iP2 }, matWorld);
		worldTriangle.ComputeNormal();


		XMVECTOR tempInter;
		//繝ｬ繧､縺ｨ荳芽ｧ貞ｽ｢縺ｮ蠖薙◆繧雁愛螳・
		if (Collision::CheckRay2Triangle(ray, worldTriangle, nullptr, &tempInter)) {

			//繝ｯ繝ｼ繝ｫ繝牙ｺｧ讓咏ｳｻ縺ｧ縺ｮ莠､轤ｹ縺ｨ繝ｬ繧､縺ｮ霍晞屬繧定ｨ育ｮ・
			XMVECTOR length = tempInter - ray.start;
			Vec3 lengthV = { length.m128_f32[0],length.m128_f32[1], length.m128_f32[2] };

			//莉翫∪縺ｧ縺ｧ霍晞屬縺御ｸ逡ｪ遏ｭ縺代ｌ縺ｰ險倬鹸
			if (lengthV.GetLength() < minDistance)
			{
				minDistance = lengthV.GetLength();
				if (distance) {
					//莠､轤ｹ縺ｨ繝ｬ繧､縺ｮ霍晞屬繧定ｨ育ｮ・
					*distance = XMVector3Dot(length, ray.dir).m128_f32[0];
				}
				if (inter) {
					//陦晉ｪ∫せ
					*inter = tempInter;
				}
			}
			//陦晉ｪ√＠縺溘ヵ繝ｩ繧ｰ繧偵が繝ｳ
			isReturn = true;
		}
	}

	return isReturn;
}
