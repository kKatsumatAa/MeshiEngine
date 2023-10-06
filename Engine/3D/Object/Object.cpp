#include "Object.h"
#include"BaseCollider.h"

using namespace DirectX;

//------------------------------------------
//パイプラインなどの設定
RootPipe Object::pipelineSetM_[2];
//図形用
Primitive Object::primitive_;
RootPipe Object::primitivePipeLineSet_[3];

//インプットレイアウト
D3D12_INPUT_ELEMENT_DESC Object::sInputLayoutPrimitive_[5] = {
	{//xyz座標
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//法線ベクトル
	 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//uv座標
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//接線
	 "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//従法線
	 "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

};


//---------------------------------------
void Object::CommonInitialize()
{
	HRESULT result = {};

	//図形クラスの
	primitive_.Initialize();

	// パイプランステートの生成
	//.obj(objはfbxと違い頂点シェーダーでボーンの処理をしないので)
	{
		//パイプラインなどの設定
		PipeLineSetting(D3D12_FILL_MODE_SOLID, pipelineSetM_[0],
			"Resources/shaders/OBJVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
			sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
		//ワイヤーフレーム
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, pipelineSetM_[1],
			"Resources/shaders/OBJVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
			sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
	}
	//primitive用
	{
		PipeLineSetting(D3D12_FILL_MODE_SOLID, primitivePipeLineSet_[0],
			"Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
		//ワイヤーフレーム用
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, primitivePipeLineSet_[1],
			"Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false);
		//線
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, primitivePipeLineSet_[2],
			"Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, false);
	}
}

//-------------------------------------------------------------------
Object::~Object()
{
}

Object::Object()
{
	//インスタンスの種類
	objInsType_ = ObjectInstanceType::OBJ;
}

//------------------------------------------------------------------
void Object::Update()
{
	IObject3D::Update();
}

//-------------------------------------------------------------------------------------
void Object::Draw()
{
	//とりあえず球を描画する
	DrawSphere();
}

void Object::DrawModelInternal(int32_t pipelineNum)
{
	//テクスチャを設定していなかったら
	uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
	TextureManager::CheckTexHandle(dissolveTextureHandleL);
	uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
	TextureManager::CheckTexHandle(specularMapTextureHandleL);
	uint64_t normalMapTextureHandleL = normalMapTextureHandle_;
	TextureManager::CheckTexHandle(normalMapTextureHandleL);

	//モデル用
	//ラムダ式でコマンド関数
	std::function<void()>SetRootPipeRM = [=]() {IObject::SetRootPipe(pipelineSetM_, pipelineNum, pipelineSetM_[0].rootSignature.Get()); };
	std::function<void()>SetMaterialTexM = [=]() {IObject3D::SetMaterialLightMTex(
		NULL, dissolveTextureHandleL, specularMapTextureHandleL, normalMapTextureHandleL, false); };

	//メッシュのオフセットデータセット
	IObject3D::GetModel()->SetPolygonOffsetData(meshOffsetData_);

	model_->Draw(SetRootPipeRM, SetMaterialTexM, cbt_);
}

void Object::DrawUpdate(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
	Camera* camera, IModel* model, bool isWireFrame)
{
	Camera* lCamera = camera;
	//カメラがセットされてなければ使用されてるカメラを使う
	if (camera == nullptr)
	{
		lCamera = CameraManager::GetInstance().usingCamera_;
	}

	//行列マッピング
	IObject3D::MatMap(lCamera, model);

	//テクスチャを設定していなかったら
	uint64_t textureHandleL = textureHandle;
	TextureManager::CheckTexHandle(textureHandleL);
	uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
	TextureManager::CheckTexHandle(dissolveTextureHandleL);
	uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
	TextureManager::CheckTexHandle(specularMapTextureHandleL);
	uint64_t normalMapTextureHandleL = normalMapTextureHandle_;
	TextureManager::CheckTexHandle(normalMapTextureHandleL);

	//ラムダ式でプリミティブのコマンド関数
	std::function<void()>SetRootPipeR = [=]() {SetRootPipe(primitivePipeLineSet_, pipelineNum, primitivePipeLineSet_->rootSignature.Get()); };
	std::function<void()>SetMaterialTex = [=]() {SetMaterialLightMTex(textureHandleL, dissolveTextureHandleL, specularMapTextureHandleL,
		normalMapTextureHandleL, constBuffTransform); };

	if (indexNum == TRIANGLE)
	{
		primitive_.TriangleDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == BOX)
	{
		primitive_.BoxDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CUBE)
	{
		primitive_.CubeDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == LINE)
	{
		primitive_.LineDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CIRCLE)
	{
		primitive_.CircleDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == SPHERE)
	{
		primitive_.SphereDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == OBJ)
	{
		//モデル用
		DrawModelInternal((int32_t)isWireFrame);
	}
}

void Object::DrawTriangle(Camera* camera, uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(TRIANGLE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawBox(Camera* camera, uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(BOX, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawCube3D(Camera* camera, uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(CUBE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawLine(Camera* camera,
	uint64_t textureHandle)
{
	//線用のパイプラインを使うので2
	DrawUpdate(LINE, 2, textureHandle, &cbt_, camera, nullptr);
}

void Object::DrawCircle(Camera* camera,
	uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(CIRCLE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawSphere(Camera* camera,
	uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(SPHERE, pipelineNum, textureHandle, &cbt_, camera);
}

void Object::DrawModel(IModel* model, Camera* camera,
	int32_t pipelineNum)
{
	//モデルがヌルだったらエラー起きるので
	if (model == nullptr)
	{
		if (model_)
		{
			model = model_;
		}
		else
		{
			return;
		}
	}

	DrawUpdate(OBJ, pipelineNum, NULL, &cbt_, camera, model);
}
