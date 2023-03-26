#include "FbxLoader.h"

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxLoader::baseDirectory = "Resources/";

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::Initialize()
{
    //再初期化チェック
    assert(fbxManager == nullptr);
    //fbxマネージャーの生成
    fbxManager = FbxManager::Create();
    //fbxマネージャーの入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(ios);
    //fbxインポータの生成
    fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
    //各種fbxインスタンスの破棄
    fbxImporter->Destroy();
    fbxManager->Destroy();
}

void FbxLoader::LoadModelFromFile(const string& modelName)
{
    //モデルと同じ名前のフォルダから読み込む
    const string directoryPath = baseDirectory + modelName + "/";
    //拡張子.fbxを付加
    const string fileName = modelName + ".fbx";
    //フルパス
    const string fullPath = directoryPath + fileName;

    //ファイル名を指定してfbxファイルを読み込む
    if (!fbxImporter->Initialize(fullPath.c_str(), -1, fbxManager->GetIOSettings()))
    {
        assert(0);
    }

    //シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

    //ファイルからロードしたfbxの情報をシーンにインポート
    fbxImporter->Import(fbxScene);
}
