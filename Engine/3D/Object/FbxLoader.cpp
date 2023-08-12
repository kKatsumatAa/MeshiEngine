﻿#include "FbxLoader.h"
#include"TextureManager.h"
#include"Util.h"

using namespace DirectX;



/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxLoader::S_BASE_DIRECTORY_ = "Resources/model/fbx/";

FbxLoader::~FbxLoader()
{

}

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader sInstance;
	return &sInstance;
}

void FbxLoader::Initialize()
{
	//再初期化チェック
	assert(fbxManager_ == nullptr);
	//fbxマネージャーの生成
	fbxManager_ = FbxManager::Create();
	//fbxマネージャーの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager_, IOSROOT);
	fbxManager_->SetIOSettings(ios);
	//fbxインポータの生成
	fbxImporter_ = FbxImporter::Create(fbxManager_, "");
}

void FbxLoader::Finalize()
{
	//各種fbxインスタンスの破棄
	fbxImporter_->Destroy();
	fbxManager_->Destroy();
}

std::unique_ptr<ModelFBX> FbxLoader::LoadModelFromFile(const string& modelName, bool smoothing)
{
	//スムージングするかどうか
	isSmoothing_ = smoothing;

	//モデルと同じ名前のフォルダから読み込む
	const string DIRECTORY_PATH = S_BASE_DIRECTORY_ + modelName + "/";
	//拡張子.fbxを付加
	const string FILE_NAME = modelName + ".fbx";
	//フルパス
	const string FULL_PATH = DIRECTORY_PATH + FILE_NAME;

	//ファイル名を指定してfbxファイルを読み込む
	if (!fbxImporter_->Initialize(FULL_PATH.c_str(), -1, fbxManager_->GetIOSettings()))
	{
		assert(0);
	}

	//シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");

	//ファイルからロードしたfbxの情報をシーンにインポート
	fbxImporter_->Import(fbxScene);

	//モデル生成
	std::unique_ptr<ModelFBX> model = std::make_unique<ModelFBX>();
	model->name_ = modelName;
	//fbxノードの数を取得
	int32_t nodeCount = fbxScene->GetNodeCount();
	//あらかじめ変数分のメモリを確保することで、アドレスがずれるのを予防
	model->nodes_.reserve(nodeCount);
	//ルートノートから順に解析してモデルに流し込む
	ParseNodeRecursive(model.get(), fbxScene->GetRootNode());
	//fbxシーンをモデルに持たせて、モデルごとに解放
	model->fbxScene_ = fbxScene;

	//初期化
	model->Initialize();

	//所有権渡す
	return std::move(model);
}

void FbxLoader::ParseNodeRecursive(ModelFBX* model, FbxNode* fbxNode, Node* parent)
{
	//モデルにノードを追加
	model->nodes_.emplace_back();
	Node& node = model->nodes_.back();
	//ノード名を取得
	node.name = fbxNode->GetName();

	//fbxノードのローカル移動情報
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	//形式変換して代入
	node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
	node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

	//回転角をDegree（度）からラジアンに変換
	node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
	node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
	node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);

	//スケール、回転、平行移動行列の計算
	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScalingFromVector(node.scaling);
	matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTranslation = XMMatrixTranslationFromVector(node.translation);

	//ローカル変形行列の計算
	node.transform = XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotation;
	node.transform *= matTranslation;

	//グローバル変換行列の計算
	node.globalTransform = node.transform;
	if (parent)
	{
		node.parent = parent;
		//親の変形を乗算
		node.globalTransform *= parent->globalTransform;
	}

	//fbxノードのメッシュ情報を解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		//種類がメッシュの場合
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//ノードからメッシュ情報を読み取る
			model->meshNode_ = &node;
			ParseMesh(model, fbxNode);
		}
	}

	//子ノードに対して再帰呼び出し
	for (int32_t i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
	}
}

void FbxLoader::ParseMesh(ModelFBX* model, FbxNode* fbxNode)
{
	//ノードのメッシュを取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//メッシュ配列に追加
	model->meshes_.emplace_back(std::move(std::make_unique <Mesh>()));
	//所有権をいったん与える
	std::unique_ptr<Mesh> mesh = std::move(model->meshes_.back());


	//頂点座標読み取り
	ParseMeshVertices(model, fbxMesh, mesh.get());
	//面を構成するデータの読み取り
	ParseMeshFaces(model, fbxMesh, mesh.get());
	//マテリアルの読み取り
	ParseMaterial(model, mesh.get(), fbxNode);
	//スキニング情報の読み取り
	PerseSkin(model, fbxMesh, mesh.get());

	// 頂点法線の平均によるエッジの平滑化
	if (isSmoothing_) {
		mesh->CalculateSmoothedVertexNormals();
	}

	//メッシュの接線
	CalcMeshTangent(model, fbxMesh, mesh.get());

	//所有権戻す
	model->meshes_[model->meshes_.size() - 1] = std::move(mesh);
}

void FbxLoader::ParseMeshVertices(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh)
{
	std::vector<Mesh::VertexPosNormalUvSkin>& vertices = mesh->vertices_;

	//頂点座標データの数(コントロールポイントとは、1つ分の頂点データのこと)
	const int32_t CONTROL_POINTS_COUNT =
		fbxMesh->GetControlPointsCount();

	//必要数だけ頂点データ配列を確保
	Mesh::VertexPosNormalUvSkin vert{};
	vertices.resize(CONTROL_POINTS_COUNT, vert);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4* coord = fbxMesh->GetControlPoints();

	//fbxメッシュの全頂点座標をモデル内の配列にコピーする
	for (int32_t i = 0; i < CONTROL_POINTS_COUNT; i++)
	{
		Mesh::VertexPosNormalUvSkin& vertex = vertices[i];
		//座標のコピー
		vertex.pos.x = (float)coord[i][0];
		vertex.pos.y = (float)coord[i][1];
		vertex.pos.z = (float)coord[i][2];
	}
}

void FbxLoader::ParseMeshFaces(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh)
{
	//参照渡し
	std::vector<Mesh::VertexPosNormalUvSkin>& vertices = mesh->vertices_;
	std::vector<uint16_t>& indices = mesh->indices_;

	//面の数
	const int32_t POLYGON_COUNT = fbxMesh->GetPolygonCount();
	//uvデータの数
	const int32_t TEXTURE_UV_COUNT = fbxMesh->GetTextureUVCount();
	//UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//面ごとの情報読み取り
	for (int32_t i = 0; i < POLYGON_COUNT; i++)
	{
		//面を構成する頂点の数を取得（3なら三角形ポリゴン,4なら四角形）
		const int32_t POLYGON_SIZE = fbxMesh->GetPolygonSize(i);
		assert(POLYGON_SIZE <= 4);

		//1頂点ずつ処理
		for (int32_t j = 0; j < POLYGON_SIZE; j++)
		{
			//面のj番目の頂点
			int32_t index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			//頂点法線読み込み
			Mesh::VertexPosNormalUvSkin& vertex = vertices[index];
			FbxVector4 normal;
			//取得できれば
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			//テクスチャUV読み込み
			if (TEXTURE_UV_COUNT > 0)
			{
				//情報を読み込むときに入れる変数
				FbxVector2 uvs;
				bool lUnmappedUV;
				//0番決め打ちで読み込み（現在は1モデルに1テクスチャのみなので）
				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			//インデックス配列に頂点インデックス追加
			//3頂点までなら
			if (j < 3)
			{
				//1点追加し、ほかの2点と三角形を構築する
				indices.push_back(index);
			}
			//4頂点目
			else
			{
				//3点追加し、四角形の 0,1,2,3 のうち 2,3,0 で三角形を構築する
				int32_t index2 = indices[indices.size() - 1];
				int32_t index3 = index;
				int32_t index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}
		}
	}
}

void FbxLoader::CalcMeshTangent(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh)
{
	//面の数
	const int32_t POLYGON_COUNT = fbxMesh->GetPolygonCount();
	//uvデータの数
	const int32_t TEXTURE_UV_COUNT = fbxMesh->GetTextureUVCount();

	//面ごとの情報読み取り
	for (int32_t i = 0; i < POLYGON_COUNT; i++)
	{
		//面を構成する頂点の数を取得（3なら三角形ポリゴン,4なら四角形）
		const int32_t POLYGON_SIZE = fbxMesh->GetPolygonSize(i);
		assert(POLYGON_SIZE <= 4);

		if (POLYGON_SIZE == 3)
		{
			//面のj番目の頂点
			int32_t index0 = fbxMesh->GetPolygonVertex(i, 0);
			int32_t index1 = fbxMesh->GetPolygonVertex(i, 1);
			int32_t index2 = fbxMesh->GetPolygonVertex(i, 2);


			// Shortcuts for vertices
			XMFLOAT3& v0 = mesh->vertices_[index0].pos;
			XMFLOAT3& v1 = mesh->vertices_[index1].pos;
			XMFLOAT3& v2 = mesh->vertices_[index2].pos;

			// Shortcuts for UVs
			XMFLOAT2& uv0 = mesh->vertices_[index0].uv;
			XMFLOAT2& uv1 = mesh->vertices_[index1].uv;
			XMFLOAT2& uv2 = mesh->vertices_[index2].uv;

			// Edges of the triangle : postion delta
			XMFLOAT3 deltaPos1 = v1 - v0;
			XMFLOAT3 deltaPos2 = v2 - v0;

			// UV delta
			XMFLOAT2 deltaUV1 = uv1 - uv0;
			XMFLOAT2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			XMFLOAT3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

			Vec3 tanN = { tangent.x,tangent.y,tangent.z };
			tanN.Normalized();

			Vec3 tan0 = { mesh->vertices_[index0].tangent.x,mesh->vertices_[index0].tangent.y,mesh->vertices_[index0].tangent.z };
			Vec3 tan1 = { mesh->vertices_[index1].tangent.x,mesh->vertices_[index1].tangent.y,mesh->vertices_[index1].tangent.z };
			Vec3 tan2 = { mesh->vertices_[index2].tangent.x,mesh->vertices_[index2].tangent.y,mesh->vertices_[index2].tangent.z };

			tan0.Normalized();
			tan1.Normalized();
			tan2.Normalized();

			mesh->vertices_[index0].tangent = XMFLOAT4(tan0.x_, tan0.y_, tan0.z_, 0) + XMFLOAT4(tanN.x_, tanN.y_, tanN.z_, 0);
			mesh->vertices_[index1].tangent = XMFLOAT4(tan1.x_, tan1.y_, tan1.z_, 0) + XMFLOAT4(tanN.x_, tanN.y_, tanN.z_, 0);
			mesh->vertices_[index2].tangent = XMFLOAT4(tan2.x_, tan2.y_, tan2.z_, 0) + XMFLOAT4(tanN.x_, tanN.y_, tanN.z_, 0);
		}
	}
}

void FbxLoader::ParseMaterial(ModelFBX* model, Mesh* mesh, FbxNode* fbxNode)
{
	const int32_t MATERIAL_COUNT = fbxNode->GetMaterialCount();
	if (MATERIAL_COUNT > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);

		//モデル用マテリアル作成
		std::unique_ptr<Material> materialM = Material::Create();

		std::string mName;

		if (material)
		{
			materialM->name_ = material->GetName();
			mName = materialM->name_;

			//FbxSurfacePhongクラスかどうかを調べる
			if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				FbxSurfacePhong* phong = static_cast<FbxSurfacePhong*>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = phong->Ambient;
				materialM->ambient_.x = (float)ambient.Get()[0];
				materialM->ambient_.y = (float)ambient.Get()[1];
				materialM->ambient_.z = (float)ambient.Get()[2];

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = phong->Diffuse;
				materialM->diffuse_.x = (float)diffuse.Get()[0];
				materialM->diffuse_.y = (float)diffuse.Get()[1];
				materialM->diffuse_.z = (float)diffuse.Get()[2];

				//鏡面反射光係数
				FbxPropertyT<FbxDouble3> specular = phong->Specular;
				materialM->specular_.x = (float)specular.Get()[0];
				materialM->specular_.y = (float)specular.Get()[1];
				materialM->specular_.z = (float)specular.Get()[2];

				//alpha
				materialM->alpha_ = 1.0f;
			}
			//lambert
			else if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				materialM->ambient_.x = (float)ambient.Get()[0];
				materialM->ambient_.y = (float)ambient.Get()[1];
				materialM->ambient_.z = (float)ambient.Get()[2];

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				materialM->diffuse_.x = (float)diffuse.Get()[0];
				materialM->diffuse_.y = (float)diffuse.Get()[1];
				materialM->diffuse_.z = (float)diffuse.Get()[2];

				//alpha
				materialM->alpha_ = 1.0f;

				//specular
				materialM->specular_ = XMFLOAT3{ 0.3f,0.3f,0.3f };
			}

			//ディフューズテクスチャを取り出す
			const FbxProperty DIFFUSE_PROPERTY =
				material->FindProperty(FbxSurfaceMaterial::sDiffuse);

			if (DIFFUSE_PROPERTY.IsValid())
			{
				const FbxFileTexture* TEX = DIFFUSE_PROPERTY.GetSrcObject<FbxFileTexture>();
				if (TEX)
				{
					const char* FILE_PATH = TEX->GetFileName();
					//ファイルパスからファイル名抽出
					string path_str(FILE_PATH);
					string name = ExtractFileName(path_str);

					//画像名保存(モデルの方でフルパスにしてくれる)
					materialM->textureFilename_ = name;
				}
				//テクスチャがない場合はそのままで白い画像が適用
			}
		}

		//メッシュにマテリアルセット
		model->AddMaterial(std::move(materialM));

		// マテリアル名で検索し、マテリアルを割り当てる
		auto itr = model->materials_.find(mName);
		if (itr != model->materials_.end()) {
			mesh->SetMaterial(itr->second.get());
		}
	}
}

void FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
{
	//行
	for (int32_t i = 0; i < 4; i++)
	{
		//列
		for (int32_t j = 0; j < 4; j++)
		{
			//1要素コピー
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}

void FbxLoader::PerseSkin(ModelFBX* model, FbxMesh* fbxMesh, Mesh* mesh)
{
	//(今は１スキンのみの前提で実装)

	//スキニング情報
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	//スキニング情報がなければ終了
	if (fbxSkin == nullptr)
	{
		//各頂点について処理
		for (int32_t i = 0; i < mesh->vertices_.size(); i++)
		{
			//最初のボーン(ないが、単位行列が入ってる)のみ影響100%にする
			mesh->vertices_[i].boneIndex[0] = 0;
			mesh->vertices_[i].boneWeight[0] = 1.0f;
		}

		return;
	}

	//ボーン配列の参照
	std::vector<ModelFBX::Bone>& bones = model->bones_;

	//ボーンの数
	int32_t clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	//全てのボーンについて
	for (int32_t i = 0; i < clusterCount; i++)
	{
		//fbxボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーン自体のノードの名前を取得
		const char* BONE_NAME = fbxCluster->GetLink()->GetName();

		//新しくボーンを追加し、追加したボーンの参照を得る
		bones.emplace_back(ModelFBX::Bone(BONE_NAME));
		ModelFBX::Bone& bone = bones.back();
		//自作ボーンとfbxのボーンを紐づける
		bone.fbxCluster = fbxCluster;

		//fbxから初期姿勢行列を取得
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//XMMatrix型に変換
		XMMATRIX initialPose;
		ConvertMatrixFromFbx(&initialPose, fbxMat);

		//初期姿勢行列の逆行列を得る(スキニング処理に使うのは逆行列)
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
	}

	//ボーン番号とスキンウェイとのペア
	struct WeightSet
	{
		uint32_t index;
		float weight;
	};

	//二次元配列（ジャグ配列）
	//list  :頂点が影響を受けるボーンの全リスト
	//vector:それを全頂点分
	std::vector<std::list<WeightSet>> weightLists(mesh->vertices_.size());

	//全てのボーンについて
	for (int32_t i = 0; i < clusterCount; i++)
	{
		//fbxボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//このボーンに影響を受ける頂点の数
		int32_t controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
		//このボーンに影響を受ける頂点の配列
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		//影響を受ける全頂点について
		for (int32_t j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点番号
			int32_t vertIndex = controlPointIndices[j];
			//スキンウェイト
			float weight = (float)controlPointWeights[j];
			//その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加
			weightLists[vertIndex].emplace_back(WeightSet{ (uint32_t)i,weight });
		}
	}

	//頂点配列書き換え用の参照
	auto& vertices = mesh->vertices_;
	//各頂点について処理
	for (int32_t i = 0; i < vertices.size(); i++)
	{
		//頂点のウェイトから最も大きい4つを選択
		auto& weightList = weightLists[i];
		//大小比較用のラムダ式を指定して降順にソート
		weightList.sort(
			[](auto const& lhs, auto const& rhs)
			{
				//左の要素の方が大きければtrue,そうでなければfalse
				return lhs.weight > rhs.weight;
			}
		);

		int32_t weightArrayIndex = 0;
		//降順ソート済みのウェイトリストから
		for (auto& weightSet : weightList)
		{
			//頂点データに書き込み
			vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;
			//4つに達したら終了
			if (++weightArrayIndex >= Mesh::S_MAX_BONE_INDICES_)
			{
				float weight = 0.0f;
				//2番目以降のウェイトを合計
				for (int32_t j = 1; j < Mesh::S_MAX_BONE_INDICES_; j++)
				{
					weight += vertices[i].boneWeight[j];
				}
				//合計で1.0f(100%)になるように調整
				vertices[i].boneWeight[0] = 1.0f - weight;
				break;
			}
		}
	}
}
