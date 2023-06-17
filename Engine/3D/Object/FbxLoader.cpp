#include "FbxLoader.h"
#include"TextureManager.h"

using namespace DirectX;

const std::string FbxLoader::S_DEFAULT_TEX_FILE_NAME_ = "image/white.png";



/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxLoader::S_BASE_DIRECTORY_ = "Resources/";

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

std::unique_ptr<ModelFBX> FbxLoader::LoadModelFromFile(const string& modelName)
{
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

	//バッファ生成
	model->CreateBuffers();

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

	//頂点座標読み取り
	ParseMeshVertices(model, fbxMesh);
	//面を構成するデータの読み取り
	ParseMeshFaces(model, fbxMesh);
	//マテリアルの読み取り
	ParseMaterial(model, fbxNode);
	//スキニング情報の読み取り
	PerseSkin(model, fbxMesh);
}

void FbxLoader::ParseMeshVertices(ModelFBX* model, FbxMesh* fbxMesh)
{
	std::vector<ModelFBX::VertexPosNormalUvSkin>& vertices = model->vertices_;

	//頂点座標データの数(コントロールポイントとは、1つ分の頂点データのこと)
	const int32_t CONTROL_POINTS_COUNT =
		fbxMesh->GetControlPointsCount();
	//必要数だけ頂点データ配列を確保
	ModelFBX::VertexPosNormalUvSkin vert{};
	model->vertices_.resize(CONTROL_POINTS_COUNT, vert);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4* coord = fbxMesh->GetControlPoints();

	//fbxメッシュの全頂点座標をモデル内の配列にコピーする
	for (int32_t i = 0; i < CONTROL_POINTS_COUNT; i++)
	{
		ModelFBX::VertexPosNormalUvSkin& vertex = vertices[i];
		//座標のコピー
		vertex.pos.x = (float)coord[i][0];
		vertex.pos.y = (float)coord[i][1];
		vertex.pos.z = (float)coord[i][2];
	}

}

void FbxLoader::ParseMeshFaces(ModelFBX* model, FbxMesh* fbxMesh)
{
	std::vector<ModelFBX::VertexPosNormalUvSkin>& vertices = model->vertices_;
	std::vector<uint16_t>& indices = model->indices_;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);
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
			ModelFBX::VertexPosNormalUvSkin& vertex = vertices[index];
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

void FbxLoader::ParseMaterial(ModelFBX* model, FbxNode* fbxNode)
{
	const int32_t MATERIAL_COUNT = fbxNode->GetMaterialCount();
	if (MATERIAL_COUNT > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		//テクスチャを読み込んたかどうかを表すフラグ
		bool textureLoaded = false;

		if (material)
		{
			//fbxSurfaceLambertクラスかどうかを調べる
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

				if (model->material_ == nullptr)
				{
					model->material_ = Material::Create();
				}
				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				model->material_->ambient_.x = 0.06f;
				model->material_->ambient_.y = 0.06f;
				model->material_->ambient_.z = 0.06f;

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				model->material_->diffuse_.x = (float)diffuse.Get()[0];
				model->material_->diffuse_.y = (float)diffuse.Get()[1];
				model->material_->diffuse_.z = (float)diffuse.Get()[2];

				//alpha
				model->material_->alpha_ = 1.0f;

				//specular
				model->material_->specular_ = XMFLOAT3{ 0.1f,0.1f,0.1f };
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
					//texture読み込み
					LoadTexture(model, S_BASE_DIRECTORY_ + model->name_ + "/" + name);
					textureLoaded = true;
				}
			}
		}
		//テクスチャがない場合は白テクスチャを貼る
		if (!textureLoaded)
		{
			LoadTexture(model, S_BASE_DIRECTORY_ + S_DEFAULT_TEX_FILE_NAME_);
		}
	}
}

void FbxLoader::LoadTexture(ModelFBX* model, const std::string& fullpath)
{
	//ユニコード文字列に変換
	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));

	TextureManager::GetInstance().LoadGraph(wfilepath, model->material_->textureHandle_);

}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
	//区切り文字が出てくる一番最後の一つ後ろからの文字列を返す（Resources'/'[***.png]みたいな）

	size_t pos1;
	//区切り文字 '\\'が出てくる一番最後の部分を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	//区切り文字 '/'が出てくる一番最後の部分を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
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

void FbxLoader::PerseSkin(ModelFBX* model, FbxMesh* fbxMesh)
{
	//(今は１スキンのみの前提で実装)

	//スキニング情報
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	
	//スキニング情報がなければ終了
	if (fbxSkin == nullptr)
	{
		//各頂点について処理
		for (int32_t i = 0; i < model->vertices_.size(); i++)
		{
			//最初のボーン(ないが、単位行列が入ってる)のみ影響100%にする
			model->vertices_[i].boneIndex[0] = 0;
			model->vertices_[i].boneWeight[0] = 1.0f;
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
	std::vector<std::list<WeightSet>> weightLists(model->vertices_.size());
	
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
	auto& vertices = model->vertices_;
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
			if (++weightArrayIndex >= ModelFBX::S_MAX_BONE_INDICES_)
			{
				float weight = 0.0f;
				//2番目以降のウェイトを合計
				for (int32_t j = 1; j < ModelFBX::S_MAX_BONE_INDICES_; j++)
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
