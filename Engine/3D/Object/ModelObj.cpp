#include "ModelObj.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>
#include <algorithm>

using namespace DirectX;


//参照先のパス一部（objかfbxか）
const std::string ModelObj::S_TYPE_DIRECTORY_ = ".obj/";


std::unique_ptr<ModelObj> ModelObj::LoadFromOBJ(const std::string& folderName, bool smoothing)
{
	//新たなModel型のインスタンスのメモリを確保
	std::unique_ptr<ModelObj> model = std::make_unique<ModelObj>();
	model->InitializeInternal(folderName, smoothing);

	//所有権渡す
	return std::move(model);
}

std::unique_ptr<ModelObj> ModelObj::CreateModelFromModelMesh(IModel& model, const std::string& meshName)
{
	std::unique_ptr<ModelObj>lModel = nullptr;

	//メッシュを元のモデルから譲渡（元のモデルからは削除）
	auto lMesh = model.MoveUniqueMesh(meshName);

	if (lMesh)
	{
		//生成
		lModel = std::make_unique<ModelObj>();

		//マテリアルも譲渡
		lModel->AddMaterial(model.MoveUniqueMaterial(lMesh.get()));
		//メッシュ追加
		lModel->AddMesh(std::move(lMesh));

		//その他のパラメータもコピー
		lModel->CopyUnUniqueParams(model);
	}
	return std::move(lModel);
}

void ModelObj::InitializeInternal(const std::string& folderName, bool smoothing)
{
	//モデル読み込み
	LoadFromOBJInternal(folderName, smoothing);

	//継承した初期化関数
	IModel::Initialize();
}

void ModelObj::LoadTextures()
{
	std::string directoryPathL = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + name_ + "/";

	IModel::LoadTexturesInternal(directoryPathL);
}

void ModelObj::LoadFromOBJInternal(const std::string& folderName, bool smoothing)
{
	const std::string FILE_NAME = folderName + ".obj";
	const std::string DIRECTORY_PATH = IModel::S_BASE_DIRECTORY_ + S_TYPE_DIRECTORY_ + folderName + "/";

	// ファイルストリーム
	std::ifstream file;
	// .objファイルを開く
	file.open(DIRECTORY_PATH + FILE_NAME);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(false);
	}

	name_ = folderName;

	// メッシュ生成
	meshes_.emplace_back(std::move(std::make_unique <Mesh>()));
	//所有権をいったん与える
	std::unique_ptr<Mesh> mesh = std::move(meshes_.back());
	int32_t indexCount = 0;

	std::vector<Vec3> positions;	// 頂点座標
	std::vector<Vec3> normals;	// 法線ベクトル
	std::vector<Vec2> texcoords;	// テクスチャUV
	// 1行ずつ読み込む
	std::string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		//マテリアル
		if (key == "mtllib")
		{
			// マテリアルのファイル名読み込み
			std::string filename;
			line_stream >> filename;
			// マテリアル読み込み
			LoadMaterial(DIRECTORY_PATH, filename);
		}
		// 先頭文字列がgならグループの開始
		if (key == "g") {

			// カレントメッシュの情報が揃っているなら
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// 頂点法線の平均によるエッジの平滑化
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				//メッシュの接線
				mesh->CalculateTangent();

				//所有権戻す
				meshes_[meshes_.size() - 1] = std::move(mesh);

				// 次のメッシュ生成
				meshes_.emplace_back(std::move(std::make_unique<Mesh>()));
				mesh = std::move(meshes_.back());
				indexCount = 0;
			}

			// グループ名読み込み
			std::string groupName;
			line_stream >> groupName;

			// メッシュに名前をセット
			mesh->SetName(groupName);
		}
		// 先頭文字列がvなら頂点座標
		if (key == "v") {
			// X,Y,Z座標読み込み
			Vec3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//逆向きに読み込まれちゃうので
			position.z *= VERTEX_Z_CHANGE_;
			positions.emplace_back(position);
		}
		// 先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			// U,V成分読み込み
			Vec2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V方向反転
			texcoord.y = TEXCOORD_Y_MAX_ - texcoord.y;
			// テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		// 先頭文字列がvnなら法線ベクトル
		if (key == "vn") {
			// X,Y,Z成分読み込み
			Vec3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//逆向きに読み込まれちゃうので
			normal.z *= VERTEX_Z_CHANGE_;
			// 法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		// 先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl")
		{
			if (mesh->GetMaterial() == nullptr) {
				// マテリアルの名読み込み
				std::string materialName;
				line_stream >> materialName;

				// マテリアル名で検索し、マテリアルを割り当てる
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second.get());
				}
			}
		}
		// 先頭文字列がfならポリゴン（三角形）
		if (key == "f")
		{
			int indexNum = 0;
			std::vector<uint16_t> indices;

			//半角スペース区切りで行を読み込む
			std::string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				std::istringstream index_stream(index_string);
				unsigned short indexPos, indexNormal, indexUV;
				//座標などを変数に読み込む
				index_stream >> indexPos;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexUV;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexNormal;

				Mesh::VertexPosNormalUvSkin vertex{};
				//座標などをセット
				vertex.pos = positions[indexPos - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexUV - 1];
				vertex.boneWeight[0] = DEFAULT_BONE_WEIGHT_;
				//メッシュを登録
				mesh->AddVertex(vertex);

				//スムージング
				if (smoothing) {
					mesh->AddSmoothData(indexPos, (unsigned short)mesh->GetVertexCount() - 1);
				}
				//四角メッシュだったら[2,3,0]の順で入れる
				if (indexNum >= S_MESH_VERTEX_NORMAL_NUM_) {
					indices.emplace_back((unsigned short)(indexCount + S_INDEX_OFFSET_VERTEX_4_OF_2_));
					indices.emplace_back((unsigned short)indexCount);
					indices.emplace_back((unsigned short)(indexCount + S_INDEX_OFFSET_VERTEX_4_OF_4_));
				}
				else
				{
					indices.emplace_back((unsigned short)indexCount);
				}
				indexNum++;
				indexCount++;
			}
			// インデックスデータの追加
			//三角メッシュ
			if (indices.size() == S_MESH_VERTEX_NORMAL_NUM_)
			{
				//DirectXの座標系用に並び替え
				mesh->AddIndex(indices[0]);
				mesh->AddIndex(indices[2]);
				mesh->AddIndex(indices[1]);
			}
			//四角メッシュだったら
			else {
				mesh->AddIndex(indices[0]);
				mesh->AddIndex(indices[2]);
				mesh->AddIndex(indices[1]);
				mesh->AddIndex(indices[3]);
				mesh->AddIndex(indices[5]);
				mesh->AddIndex(indices[4]);
			}
		}
	}
	file.close();

	// 頂点法線の平均によるエッジの平滑化
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}

	//メッシュの接線
	mesh->CalculateTangent();

	//所有権戻す
	meshes_[meshes_.size() - 1] = std::move(mesh);
}

void ModelObj::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	// ファイルストリーム
	std::ifstream file;
	// マテリアルファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(false);
	}

	std::unique_ptr<Material> material;

	// 1行ずつ読み込む
	std::string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin()); // 先頭の文字を削除
		}

		// 先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {

			// 既にマテリアルがあれば
			if (material.get()) {
				// マテリアルをコンテナに登録
				AddMaterial(std::move(material));
			}

			// 新しいマテリアルを生成
			material = Material::Create();
			// マテリアル名読み込み
			line_stream >> material->name_;
		}
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient_.x;
			line_stream >> material->ambient_.y;
			line_stream >> material->ambient_.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse_.x;
			line_stream >> material->diffuse_.y;
			line_stream >> material->diffuse_.z;
		}
		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular_.x;
			line_stream >> material->specular_.y;
			line_stream >> material->specular_.z;
		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			//テクスチャファイル名読み込み
			line_stream >> material->textureFilename_;

			// フルパスからファイル名を取り出す
			//'\\'以外を取得
			material->textureFilename_ = MySubstr(material->textureFilename_, '\\');

			//'/'以外を取得
			material->textureFilename_ = MySubstr(material->textureFilename_, '/');
		}
	}
	// ファイルを閉じる
	file.close();

	if (material.get()) {
		// マテリアルを登録（所有権を渡す）
		AddMaterial(std::move(material));
	}
}