#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>
#include <algorithm>

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string Model::baseDirectory = "Resources/";
UINT Model::descriptorHandleIncrementSize = 0;


void Model::LoadFromOBJInternal(const std::string& folderName, const bool smoothing, const bool& modelType)
{
	const std::string filename = folderName + ".obj";
	const std::string directoryPath = baseDirectory + folderName + "/";

	// ファイルストリーム
	std::ifstream file;
	// .objファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	name = folderName;

	// メッシュ生成
	meshes.emplace_back(new Mesh);
	Mesh* mesh = meshes.back();
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	std::vector<XMFLOAT3> positions;	// 頂点座標
	std::vector<XMFLOAT3> normals;	// 法線ベクトル
	std::vector<XMFLOAT2> texcoords;	// テクスチャUV
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
			LoadMaterial(directoryPath, filename);
		}
		// 先頭文字列がgならグループの開始
		if (key == "g") {

			// カレントメッシュの情報が揃っているなら
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// 頂点法線の平均によるエッジの平滑化
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				// 次のメッシュ生成
				meshes.emplace_back(new Mesh);
				mesh = meshes.back();
				indexCountTex = 0;
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
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		// 先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			// U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V方向反転
			texcoord.y = 1.0f - texcoord.y;
			// テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		// 先頭文字列がvnなら法線ベクトル
		if (key == "vn") {
			// X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
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
				auto itr = materials.find(materialName);
				if (itr != materials.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		// 先頭文字列がfならポリゴン（三角形）
		if (key == "f")
		{
			int faceIndexCount = 0;
			// 半角スペース区切りで行の続きを読み込む
			std::string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// 頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// 頂点番号
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
				// マテリアル、テクスチャがある場合
				if (material && material->textureFilename.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
					index_stream >> indexNormal;
					// 頂点データの追加
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					// エッジ平滑化用のデータを追加
					if (smoothing) {
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else {
					char c;
					index_stream >> c;
					// スラッシュ2連続の場合、頂点番号のみ
					if (c == '/') {
						// 頂点データの追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
					}
					else {
						index_stream.seekg(-1, std::ios_base::cur); // 1文字戻る
						index_stream >> indexTexcoord;
						index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
						index_stream >> indexNormal;
						// 頂点データの追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
						// エッジ平滑化用のデータを追加
						if (smoothing) {
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}
				// インデックスデータの追加
				if (faceIndexCount >= 3) {

					if (modelType == false)
					{
						// 四角形ポリゴンの4点目なので、
						// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
						mesh->AddIndex(indexCountTex - 1);
						mesh->AddIndex(indexCountTex);
						mesh->AddIndex(indexCountTex - 3);
					}
					else
					{
						mesh->PopIndex();
						mesh->PopIndex();
						mesh->PopIndex();

						mesh->AddIndex(indexCountTex - 1);
						mesh->AddIndex(indexCountTex - 2);
						mesh->AddIndex(indexCountTex - 3);

						mesh->AddIndex(indexCountTex - 3);
						mesh->AddIndex(indexCountTex);
						mesh->AddIndex(indexCountTex - 1);
					}
				}
				else
				{
					mesh->AddIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	file.close();

	// 頂点法線の平均によるエッジの平滑化
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	// ファイルストリーム
	std::ifstream file;
	// マテリアルファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

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
			if (material) {
				// マテリアルをコンテナに登録
				AddMaterial(material);
			}

			// 新しいマテリアルを生成
			material = Material::Create();
			// マテリアル名読み込み
			line_stream >> material->name;
		}
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}
		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			//テクスチャファイル名読み込み
			line_stream >> material->textureFilename;

			// フルパスからファイル名を取り出す
			size_t pos1;
			pos1 = material->textureFilename.rfind('\\');
			if (pos1 != std::string::npos) {
				material->textureFilename = material->textureFilename.substr(pos1 + 1, material->textureFilename.size() - pos1 - 1);
			}

			pos1 = material->textureFilename.rfind('/');
			if (pos1 != std::string::npos) {
				material->textureFilename = material->textureFilename.substr(pos1 + 1, material->textureFilename.size() - pos1 - 1);
			}

		}
	}
	// ファイルを閉じる
	file.close();

	if (material) {
		// マテリアルを登録
		AddMaterial(material);
	}
}

void Model::AddMaterial(Material* material)
{
	// コンテナに登録
	materials.emplace(material->name, material);
}

void Model::LoadTextures()
{
	std::string directoryPath = baseDirectory + name + "/";

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	for (auto& m : materials) {
		Material* material = m.second;
		srvGpuHandle.ptr = material->textureHandle;

		// テクスチャあり
		if (material->textureFilename.size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; cpuDescHandleSRV.ptr = 0;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; gpuDescHandleSRV.ptr = 0;
			// マテリアルにテクスチャ読み込み
			material->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
		}
		// テクスチャなし
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; cpuDescHandleSRV.ptr = 0;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; gpuDescHandleSRV.ptr = 0;
			// マテリアルにテクスチャ読み込み
			material->LoadTexture(baseDirectory, cpuDescHandleSRV, gpuDescHandleSRV);
		}
	}
}


Model* Model::LoadFromOBJ(const std::string& folderName, const bool smoothing, const bool& modelType)
{
	//新たなModel型のインスタンスのメモリを確保
	Model* model = new Model;
	model->Initialize(folderName, smoothing);

	return model;
}


Model::~Model()
{
	for (auto m : meshes) {
		delete m;
	}
	meshes.clear();

	for (auto m : materials) {
		delete m.second;
	}
	materials.clear();
}

void Model::StaticInitialize()
{
	Mesh::StaticInitialize(Directx::GetInstance().GetDevice());
}

void Model::Initialize(const std::string& foldername, bool smoothing)
{
	// モデル読み込み
	LoadFromOBJInternal(foldername.c_str(), smoothing);

	// メッシュのマテリアルチェック
	for (Mesh* m : meshes) {
		// マテリアルの割り当てがない
		if (m && m->GetMaterial() == nullptr) {
			if (defaultMaterial == nullptr) {
				// デフォルトマテリアルを生成
				defaultMaterial = Material::Create();
				defaultMaterial->name = "no material";
				materials.emplace(defaultMaterial->name, defaultMaterial);
			}
			// デフォルトマテリアルをセット
			m->SetMaterial(defaultMaterial);
		}
	}

	// メッシュのバッファ生成
	for (auto& m : meshes) {
		if (m) {
			m->CreateBuffers();
		}
	}
	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials) {
		m.second->Update();
	}

	// テクスチャの読み込み
	LoadTextures();
}

void Model::Draw(const int indexNum)
{
	// デスクリプタヒープの配列
	if (TextureManager::GetInstance().srvHeap) {
		ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetInstance().srvHeap.Get() };
		Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(indexNum, ppHeaps);
	}

	// 全メッシュを描画
	for (auto& mesh : meshes) {
		mesh->Draw(Directx::GetInstance().GetCommandList());
	}
}