#include "FbxLoader.h"
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

	//フルパス
	const std::string FULL_PATH = GetFullPath(modelName);

	//シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");
	FbxSceneImport(FULL_PATH, fbxScene);

	//モデル生成
	std::unique_ptr<ModelFBX> model = std::make_unique<ModelFBX>();
	model->name_ = modelName;
	//fbxノードの数を取得
	int32_t nodeCount = fbxScene->GetNodeCount();
	//あらかじめ変数分のメモリを確保することで、アドレスがずれるのを予防
	model->nodes_.reserve(nodeCount);
	//ルートノートから順に解析してモデルに流し込む
	ParseNodeRecursive(model.get(), fbxScene->GetRootNode());
	//アニメーション
	LoadAnimation(model.get(), fbxScene);

	//fbxシーンをモデルに持たせて、モデルごとに解放
	model->fbxScene_ = fbxScene;

	//初期化
	model->Initialize();

	//所有権渡す
	return std::move(model);
}

std::string FbxLoader::GetFullPath(const std::string& name)
{
	//モデルと同じ名前のフォルダから読み込む
	const string DIRECTORY_PATH = S_BASE_DIRECTORY_ + name + "/";
	//拡張子.fbxを付加
	const string FILE_NAME = name + ".fbx";
	//フルパス
	const string FULL_PATH = DIRECTORY_PATH + FILE_NAME;

	return std::string(FULL_PATH);
}

void FbxLoader::FbxSceneImport(const std::string& fbxFullPath, FbxScene* fbxScene)
{
	//ファイル名を指定してfbxファイルを読み込む
	if (!fbxImporter_->Initialize(fbxFullPath.c_str(), -1, fbxManager_->GetIOSettings()))
	{
		assert(0);
	}

	//ファイルからロードしたfbxの情報をシーンにインポート
	fbxImporter_->Import(fbxScene);
}

void FbxLoader::CalcGlobalTransform(const FbxNode& fbxNode, Node& node, Node* parent, float scaleExtend)
{
	//fbxノードのローカル移動情報
	FbxDouble3 rotation = fbxNode.LclRotation.Get();
	FbxDouble3 scaling = fbxNode.LclScaling.Get();
	FbxDouble3 translation = fbxNode.LclTranslation.Get();

	//形式変換して代入
	node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	node.scaling = { ((float)scaling[0]) * scaleExtend,((float)scaling[1]) * scaleExtend,((float)scaling[2]) * scaleExtend,0.0f };
	node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

	//回転角をDegree（度）からラジアンに変換
	node.rotation.x = XMConvertToRadians(node.rotation.x);
	node.rotation.y = XMConvertToRadians(node.rotation.y);
	node.rotation.z = XMConvertToRadians(node.rotation.z);

	//スケール、回転、平行移動行列の計算
	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
	matRotation = XMMatrixRotationRollPitchYaw(node.rotation.x, node.rotation.y, node.rotation.z);
	matTranslation = XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);

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
}

void FbxLoader::ParseNodeRecursive(ModelFBX* model, FbxNode* fbxNode, Node* parent)
{
	//モデルにノードを追加
	model->nodes_.emplace_back();
	Node& node = model->nodes_.back();
	//ノード名を取得
	node.name = fbxNode->GetName();
	//idも
	node.id = fbxNode->GetUniqueID();
	//属性も
	node.attribute = fbxNode->GetNodeAttribute() ?
		fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;

	//グローバルトランスフォーム
	CalcGlobalTransform(*fbxNode, node, parent);

	//fbxノードのメッシュ情報を解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		//種類がメッシュの場合
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//メッシュ配列に追加
			model->meshes_.emplace_back(std::move(std::make_unique <Mesh>()));
			//所有権をいったん与える
			std::unique_ptr<Mesh> mesh = std::move(model->meshes_.back());
			//グローバルトランスフォーム行列
			mesh->globalTransform_ = node.globalTransform;

			//ノードからメッシュ情報を読み取る
			mesh->SetMeshNode(&node);
			ParseMesh(model, fbxNode, mesh.get());

			//所有権戻す
			model->meshes_[model->meshes_.size() - 1] = std::move(mesh);
		}
	}

	//子ノードに対して再帰呼び出し
	for (int32_t i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
	}
}

void FbxLoader::ParseMesh(ModelFBX* model, FbxNode* fbxNode, Mesh* mesh)
{
	//ノードのメッシュを取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//コントロールポイント用配列
	myControlPoints_.clear();


	//頂点座標(コントロールポイント)読み取り
	ParseMeshControlPointsPos(fbxMesh);
	//スキニング情報(コントロールポイントへ)の読み取り
	PerseSkin(model, fbxMesh);
	//面を構成するデータの読み取り
	ParseMeshFaces(fbxMesh, mesh);
	//マテリアルの読み取り
	ParseMaterial(model, mesh, fbxNode);

	// 頂点法線の平均によるエッジの平滑化
	if (isSmoothing_) {
		mesh->CalculateSmoothedVertexNormals();
	}

	//メッシュの接線
	mesh->CalculateTangent();
}

void FbxLoader::ParseMeshControlPointsPos(FbxMesh* fbxMesh)
{
	//頂点座標データの数(コントロールポイントとは、1つ分の頂点データのこと)
	const int32_t CONTROL_POINTS_COUNT = fbxMesh->GetControlPointsCount();

	//コントロールポイント配列も
	MyControlPoint controlP;
	myControlPoints_.resize(CONTROL_POINTS_COUNT, controlP);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4* coord = fbxMesh->GetControlPoints();

	//fbxメッシュの全頂点座標をモデル内の配列にコピーする
	for (int32_t i = 0; i < CONTROL_POINTS_COUNT; i++)
	{
		MyControlPoint& point = myControlPoints_[i];
		//座標のコピー
		point.pos.x = (float)coord[i][0];
		point.pos.y = (float)coord[i][1];
		point.pos.z = (float)coord[i][2];
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
		return;
	}

	//ボーンの数
	int32_t clusterCount = fbxSkin->GetClusterCount();

	//二次元配列（ジャグ配列）
	//list  :頂点が影響を受けるボーンの全リスト
	//vector:それを全頂点分
	std::vector<std::list<WeightSet>> weightLists(myControlPoints_.size());

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
	auto& controlPoint = myControlPoints_;
	//各頂点について処理
	for (int32_t i = 0; i < myControlPoints_.size(); i++)
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
			controlPoint[i].boneIndex[weightArrayIndex] = weightSet.index;
			controlPoint[i].boneWeight[weightArrayIndex] = weightSet.weight;
			//4つに達したら終了
			if (++weightArrayIndex >= Mesh::S_MAX_BONE_INDICES_)
			{
				float weight = 0.0f;
				//2番目以降のウェイトを合計
				for (int32_t j = 1; j < Mesh::S_MAX_BONE_INDICES_; j++)
				{
					weight += controlPoint[i].boneWeight[j];
				}
				//合計で1.0f(100%)になるように調整
				controlPoint[i].boneWeight[0] = 1.0f - weight;
				break;
			}
		}
	}


	//全てのボーンについて行列を追加
	model->boneNodeIndices_.resize(clusterCount);
	model->offsetTransforms_.resize(clusterCount);
	for (int32_t i = 0; i < clusterCount; i++)
	{
		//fbxボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーンのノードのインデックス
		uint64_t nodeId = static_cast<uint64_t>(fbxCluster->GetLink()->GetUniqueID());
		int nodeCount = static_cast<int>(model->nodes_.size());
		//idが同じのものを探してインデックスを保存
		for (int j = 0; j < nodeCount; j++)
		{
			if (model->nodes_[j].id == nodeId)
			{
				model->boneNodeIndices_[i] = j;
				break;
			}
		}


		//fbxから初期姿勢行列を取得
		FbxAMatrix initMat;
		fbxCluster->GetTransformLinkMatrix(initMat);
		//グローバルトランスフォーム
		FbxAMatrix globalInitPosMat;
		fbxCluster->GetTransformMatrix(globalInitPosMat);

		//初期姿勢の逆行列とトランスフォーム行列掛ける
		DirectX::XMMATRIX lMat = DirectX::XMMatrixIdentity();
		ConvertMatrixFromFbx(&lMat, initMat.Inverse() * globalInitPosMat);
		model->offsetTransforms_[i] = lMat;
	}
}


//-------------------------------------------------------------------------------------------
void FbxLoader::LoadAnimation(ModelFBX* model, FbxScene* fbxScene)
{
	//アニメーション名を保存する配列
	FbxArray<FbxString*> animationNames;

	//ノード情報
	size_t nodeCount = model->GetNodes()->size();
	std::vector<FbxNode*> fbxNodes;
	fbxNodes.resize(nodeCount);
	//ノード一つ一つのポインタを保存(モデルのノードとsceneのノードが一致してる前提)
	for (size_t i = 0; i < nodeCount; i++)
	{
		fbxNodes[i] = fbxScene->FindNodeByName(model->nodes_[i].name.c_str());
	}

	//シーンからアニメーション一覧を取得
	fbxScene->FillAnimStackNameArray(animationNames);
	//アニメーションの数
	const int32_t ANIMATION_COUNT = animationNames.GetCount();

	for (int32_t i = 0; i < ANIMATION_COUNT; i++)
	{
		//要素追加
		model->animationDatas_.emplace_back();
		//参照
		ModelFBX::Animation& animationData = model->animationDatas_.back();
		//アニメーション名をモデルの変数に保存
		animationData.name = animationNames[i]->Buffer();

		//名前でアニメーションを取得
		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationData.name.c_str());
		//シーンのアニメーションをこれに設定
		fbxScene->SetCurrentAnimationStack(animationStack);

		//1フレームで進む時間の設定
		FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
		if (timeMode == FbxTime::EMode::eFrames30Drop)
		{
			timeMode = FbxTime::EMode::eFrames30;
		}
		//一秒で進む
		FbxTime oneSecond;
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		//fpsをdoubleに変換
		animationData.frameRate = static_cast<double>(oneSecond.GetFrameRate(timeMode));

		//1フレームに変換
		const FbxTime SAMPLING_INTERVAL =
			static_cast<FbxLongLong>(oneSecond.Get() / animationData.frameRate);
		//アニメーション情報を取得
		const FbxTakeInfo* TAKE_INFO = fbxScene->GetTakeInfo(animationData.name.c_str());
		//開始時間
		const FbxTime START_TIME = TAKE_INFO->mLocalTimeSpan.GetStart();
		//終了時間
		const FbxTime END_TIME = TAKE_INFO->mLocalTimeSpan.GetStop();

		//fpsで割る（秒単位にする）
		animationData.startTime = START_TIME.GetFrameCount() / animationData.frameRate;

		//キーフレームごとに情報を保存
		for (FbxTime time = START_TIME; time < END_TIME; time += SAMPLING_INTERVAL)
		{
			//要素追加
			animationData.keyframes.emplace_back();
			//参照
			ModelFBX::Keyframe& keyframe = animationData.keyframes.back();
			//配列のサイズ変更
			keyframe.nodeKeys.resize(nodeCount);

			//キーフレームの時間をfps(フレームレート)で割って秒単位にする
			double fTime = time.GetFrameCount() / animationData.frameRate;
			//開始から何秒か
			keyframe.seconds = fTime - animationData.startTime;

			//キーフレームごとにノード全ての行列取得
			for (int32_t j = 0; j < nodeCount; ++j)
			{
				//ノードがあれば
				if (fbxNodes[j])
				{
					//参照
					ModelFBX::NodeKeyData& node = keyframe.nodeKeys[j];
					//キーフレームに対応した行列
					const FbxAMatrix& local_transform = fbxNodes[j]->EvaluateLocalTransform(time);
					FbxVector4 scale = local_transform.GetS();
					FbxQuaternion rot = local_transform.GetQ();
					FbxVector4 trans = local_transform.GetT();
					//パラメータ
					node.scale = { (float)scale[0],(float)scale[1],(float)scale[2] };
					node.rotate = { (float)rot[0],(float)rot[1],(float)rot[2],(float)rot[3] };
					node.trans = { (float)trans[0],(float)trans[1],(float)trans[2] };
				}
			}
		}

		animationData.endTime = animationData.keyframes.back().seconds;
		//アニメーションの長さ保存
		animationData.secondsLength = animationData.endTime - animationData.startTime;
		//一フレームごとに加算する時間
		animationData.addTime = animationData.endTime / END_TIME.GetFrameCount(FbxTime::EMode::eFrames60);
	}

	animationNames.Clear();
}

//----------------------------------------------------------------------------------------
void FbxLoader::ParseMeshFaces(FbxMesh* fbxMesh, Mesh* mesh)
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

	int16_t vertexCount = 0;

	//面ごとの情報読み取り
	for (int32_t i = 0; i < POLYGON_COUNT; i++)
	{
		//面を構成する頂点の数を取得（3なら三角形ポリゴン,4なら四角形）
		const int32_t POLYGON_SIZE = fbxMesh->GetPolygonSize(i);
		assert(POLYGON_SIZE <= 4);

		//1頂点ずつ処理
		for (int32_t j = 0; j < POLYGON_SIZE; j++)
		{
			//面のj番目の頂点(GetPolygonVertex()はコントロールポイントへのインデックスを取得)
			int32_t index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			vertices.emplace_back();
			Mesh::VertexPosNormalUvSkin& vertex = vertices.back();

			//コントロールポイント
			{
				//座標
				MyControlPoint contrP = myControlPoints_[index];
				vertex.pos.x = contrP.pos.x;
				vertex.pos.y = contrP.pos.y;
				vertex.pos.z = contrP.pos.z;

				//ボーン
				for (int k = 0; k < Mesh::S_MAX_BONE_INDICES_; k++)
				{
					vertex.boneIndex[k] = contrP.boneIndex[k];
					vertex.boneWeight[k] = contrP.boneWeight[k];
				}
			}

			//頂点法線読み込み
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
				indices.push_back(vertexCount);
			}
			//4頂点目
			else
			{
				//3点追加し、四角形の 0,1,2,3 のうち 2,3,0 で三角形を構築する
				int16_t index2 = indices[indices.size() - 1];
				int16_t index3 = vertexCount;
				int16_t index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}

			vertexCount++;
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

void FbxLoader::AddAnimationModel(ModelFBX* addToModel, const std::string& animeFbxName)
{
	//フルパス
	const std::string FULL_PATH = GetFullPath(animeFbxName);

	//シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");
	FbxSceneImport(FULL_PATH, fbxScene);

	LoadAnimation(addToModel, fbxScene);
}
