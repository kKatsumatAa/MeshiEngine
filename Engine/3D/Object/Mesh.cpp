#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>
#include "Util.h"
#include "Object.h"
#include <set>

using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Mesh::sDevice_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!Mesh::sDevice_);

	Mesh::sDevice_ = device;

	// マテリアルの静的初期化
	Material::StaticInitialize(device);
}

Mesh::Mesh()
{
	globalTransform_ = XMMatrixIdentity();

	cbt_.Initialize();
}

void Mesh::MappingVertices(std::vector<VertexPosNormalUvSkin>* vertices)
{
	auto verticesL = vertices_;

	if (vertices)
	{
		verticesL = *vertices;
	}

	// 頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	HRESULT result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(verticesL.begin(), verticesL.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}
}

void Mesh::SetName(const std::string& name)
{
	name_ = name;
}

void Mesh::AddVertex(const VertexPosNormalUvSkin& vertex)
{
	vertices_.emplace_back(vertex);
}

void Mesh::AddIndex(uint16_t index)
{
	indices_.emplace_back(index);
}

void Mesh::PopIndex()
{
	indices_.pop_back();
}

void Mesh::AddSmoothData(uint16_t indexPosition, uint16_t indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr) {
		// 各面用の共通頂点コレクション
		std::vector<uint16_t>& v = itr->second;
		// 全頂点の法線を平均する
		XMVECTOR normal = {};
		for (uint16_t index : v) {
			normal += XMVectorSet(vertices_[index].normal.x_, vertices_[index].normal.y_, vertices_[index].normal.z_, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		for (uint16_t index : v) {
			vertices_[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
		}
	}
}

void Mesh::CalculateTangent()
{
	for (int32_t i = 0; i < indices_.size() / 3; i++)
	{//三角形一つごとに計算
		//三角形のインデックスを取り出して、一時的な変数に入れる
		uint16_t index0 = indices_[i * 3 + 0];
		uint16_t index1 = indices_[i * 3 + 1];
		uint16_t index2 = indices_[i * 3 + 2];

		// Shortcuts for vertices
		Vec3& v0 = vertices_[index0].pos;
		Vec3& v1 = vertices_[index1].pos;
		Vec3& v2 = vertices_[index2].pos;

		// Shortcuts for UVs
		Vec2& uv0 = vertices_[index0].uv;
		Vec2& uv1 = vertices_[index1].uv;
		Vec2& uv2 = vertices_[index2].uv;

		// Edges of the triangle : postion delta
		Vec3 deltaPos1 = v1 - v0;
		Vec3 deltaPos2 = v2 - v0;

		// UV delta
		Vec2 deltaUV1 = uv1 - uv0;
		Vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x_ * deltaUV2.y_ - deltaUV1.y_ * deltaUV2.x_);
		Vec3 tangent = (deltaPos1 * deltaUV2.y_ - deltaPos2 * deltaUV1.y_) * r;
		Vec3 biTangent = (deltaPos2 * deltaUV1.x_ - deltaPos1 * deltaUV2.x_) * r;

		Vec3 n = { vertices_[index0].normal.x_,vertices_[index0].normal.y_,vertices_[index0].normal.z_ };
		Vec3 t = { tangent.x_,tangent.y_,tangent.z_ };
		Vec3 b = { biTangent.x_,biTangent.y_,biTangent.z_ };

		// Gram-Schmidt orthogonalize
		t = (t - n * n.Dot(t)).GetNormalized();

		// Calculate handedness
		if (n.Cross(t).Dot(b) < 0.0f) {
			t = t * -1.0f;
		}

		vertices_[index0].tangent = { t.x_,t.y_,t.z_,0 };
		vertices_[index1].tangent = { t.x_,t.y_,t.z_,0 };
		vertices_[index2].tangent = { t.x_,t.y_,t.z_,0 };
	}
}

void Mesh::SetMaterial(Material* material)
{
	material_ = material;
}

void Mesh::CreateBuffers()
{
	HRESULT result = {};

	uint32_t sizeVB = static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * indices_.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));


	MappingVertices();

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(VertexPosNormalUvSkin);

	if (indices_.size())
	{
		// リソース設定
		resourceDesc.Width = sizeIB;

		// インデックスバッファ生成
		BuffProperties(heapProps, resourceDesc, &indexBuff_);

		// インデックスバッファへのデータ転送
		uint16_t* indexMap = nullptr;
		result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
		if (SUCCEEDED(result)) {

			std::copy(indices_.begin(), indices_.end(), indexMap);

			indexBuff_->Unmap(0, nullptr);
		}

		// インデックスバッファビューの作成
		ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
		ibView_.Format = DXGI_FORMAT_R16_UINT;
		ibView_.SizeInBytes = sizeIB;
	}
}

void Mesh::SendingMat(Vec3 materialExtend, const ConstBuffTransform& cbt)
{
	timer_++;

	cbt_.SetWorldMat(globalTransform_);
	cbt_.SetViewProjMat(cbt.GetViewProjMat());
	cbt_.SetCameraPos(cbt.GetCameraPos());

	//マテリアル
	XMFLOAT3 amb = material_->ambient_;
	material_->ambient_ = material_->ambient_ * materialExtend.x_;
	XMFLOAT3 diff = material_->diffuse_;
	material_->diffuse_ = material_->diffuse_ * materialExtend.y_;
	XMFLOAT3 spe = material_->specular_;
	material_->specular_ = material_->specular_ * materialExtend.z_;
	//更新
	material_->Update();

	//戻す
	material_->ambient_ = amb;
	material_->diffuse_ = diff;
	material_->specular_ = spe;

	//頂点情報
	SendingVetices();
}

void Mesh::SendingVetices()
{
	bool isVailid = false;

	//間隔が来たら
	if (timer_ % max((int32_t)polygonOffsetData_.interval, 1) == 0)
	{
		if (polygonOffsetData_.length != 0.0f)
		{
			//コピー
			offsetVertices_.clear();
			offsetVertices_.resize(vertices_.size());
			offsetVertices_ = vertices_;

			isVailid = true;
		}
	}

	if (isVailid)
	{
		for (int r = 0; r < (int32_t)((float)GetPolygonCount() * polygonOffsetData_.ratio); r++)
		{
			//ランダムにポリゴンのインデックス決める
			int32_t pIndex = (int32_t)GetRand(0, (float)max((GetPolygonCount() - 1), 0));

			//ポリゴンを法線方向にランダムで動かす
			float extend = GetRand(0.1f, 1.0f);
			Vec3 offset = GetPolygonNormal(pIndex) * (polygonOffsetData_.length * extend);
			offsetVertices_[pIndex * 3 + 0].pos += offset;
			offsetVertices_[pIndex * 3 + 1].pos += offset;
			offsetVertices_[pIndex * 3 + 2].pos += offset;
		}
	}

	if (polygonOffsetData_.ratio != 0.0f)
	{
		MappingVertices(&offsetVertices_);
	}
	else
	{
		//マッピング
		MappingVertices();
	}
}

void Mesh::Draw(Vec3 materialExtend, const ConstBuffTransform& cbt,
	const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	SendingMat(materialExtend, cbt);

	// シェーダリソースビューをセット
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = material_->textureHandle_;
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(Object::ROOTPARAM_NUM::TEX, srvGpuHandle);

	// マテリアルの定数バッファをセット
	ID3D12Resource* constBuff = material_->GetConstantBuffer();
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(Object::ROOTPARAM_NUM::MATERIAL, constBuff->GetGPUVirtualAddress());

	//メッシュごとの行列
	cbt_.DrawCommand(Object::ROOTPARAM_NUM::MESH_MAT);

	// 頂点バッファをセット
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファをセット
	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView_);


	// 描画コマンド
	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indices_.size(), 1, 0, 0, 0);
}


//-------------------------------------------------------------------------------------------
Vec3 Mesh::GetPolygonNormal(int32_t index)
{
	uint16_t index0 = indices_[index * 3 + 0];
	uint16_t index1 = indices_[index * 3 + 1];
	uint16_t index2 = indices_[index * 3 + 2];

	//座標を元に戻す
	Vec3 normal0 = vertices_[index0].normal;
	Vec3 normal1 = vertices_[index1].normal;
	Vec3 normal2 = vertices_[index2].normal;

	return (normal0 + normal1 + normal2).GetNormalized();
}

void Mesh::SetPolygonOffsetData(const Mesh::PolygonOffset& polygonOffsetData)
{
	polygonOffsetData_ = polygonOffsetData;
}
