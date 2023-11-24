#include "Object.h"
#include"BaseCollider.h"

using namespace DirectX;

//------------------------------------------
//パイプラインなどの設定
RootPipe Object::pipelineSetM_[PipelineStateNumObj::COUNT_O];
//図形用
Primitive Object::primitive_;
RootPipe Object::primitivePipeLineSet_[PipelineStateNumPrim::COUNT_P];

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
	//図形クラスの
	primitive_.Initialize();

	// パイプランステートの生成
	//.obj(objはfbxと違い頂点シェーダーでボーンの処理をしないので)
	{
		//パイプラインなどの設定
		PipeLineSetting(D3D12_FILL_MODE_SOLID, pipelineSetM_[NORMAL_OBJ],
			"Resources/shaders/OBJVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
			"", "", "",
			sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		//ワイヤーフレーム
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, pipelineSetM_[WIREFRAME_OBJ],
			"Resources/shaders/OBJVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
			"", "", "",
			sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		//シャドウマップ
		PipeLineSetting(D3D12_FILL_MODE_SOLID, pipelineSetM_[SHADOW_OBJ],
			"Resources/shaders/OBJShadowVS.hlsl", "",
			"", "", "",
			sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 0);
		//メッシュ分割
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, pipelineSetM_[HULL_DOMAIN_OBJ],
			"Resources/shaders/OBJVSTess.hlsl", "Resources/shaders/OBJPixelShader.hlsl",
			"", "Resources/shaders/WaveHullShader.hlsl", "Resources/shaders/WaveDomainShader.hlsl",
			sInputLayoutM_, _countof(sInputLayoutM_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH);
	}
	//primitive用
	{
		PipeLineSetting(D3D12_FILL_MODE_SOLID, primitivePipeLineSet_[NORMAL_PRIM],
			"Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl",
			"", "", "",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		//ワイヤーフレーム用
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, primitivePipeLineSet_[WIREFRAME_PRIM],
			"Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl",
			"", "", "",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		//シャドウマップ
		PipeLineSetting(D3D12_FILL_MODE_SOLID, primitivePipeLineSet_[SHADOW_PRIM],
			"Resources/shaders/BasicShadowVS.hlsl", "",
			"", "", "",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 0);
		//メッシュ分割
		PipeLineSetting(D3D12_FILL_MODE_SOLID, primitivePipeLineSet_[HULL_DOMAIN_PRIM],
			"Resources/shaders/BasicVSTess.hlsl", "Resources/shaders/BasicPS.hlsl",
			"", "Resources/shaders/WaveHullShader.hlsl", "Resources/shaders/WaveDomainShader.hlsl",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH);

		//線
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, primitivePipeLineSet_[NORMAL_LINE],
			"Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl",
			"", "", "",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
		//線のシャドウマップ
		PipeLineSetting(D3D12_FILL_MODE_WIREFRAME, primitivePipeLineSet_[SHADOW_LINE],
			"Resources/shaders/BasicShadowVS.hlsl", "",
			"", "", "",
			sInputLayoutPrimitive_, _countof(sInputLayoutPrimitive_), D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, 0);
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

//------------------------------------------------------------------
void Object::PreDrawShadow()
{

}

//-------------------------------------------------------------------------------------
void Object::Draw()
{
	//とりあえず球を描画する
	DrawSphere();
}

void Object::DrawModelInternal(int32_t pipelineNum)
{
	bool isShadow = true;

	//ラムダ式でコマンド関数
	std::function<void()>SetRootPipeRM = [=]() {IObject::SetRootPipe(pipelineSetM_, pipelineNum, pipelineSetM_[pipelineNum].rootSignature.Get()); };
	std::function<void()>SetMaterialTexM = [=]() {
		//行列
		cbt_.DrawCommand(MATRIX);
	};

	//シャドウマップ用の前描画じゃなかったら
	if (pipelineNum != PipelineStateNumObj::SHADOW_OBJ)
	{
		isShadow = false;

		//テクスチャを設定していなかったら
		uint64_t dissolveTextureHandleL = dissolveTextureHandle_;
		TextureManager::CheckTexHandle(dissolveTextureHandleL);
		uint64_t specularMapTextureHandleL = specularMapTextureHandle_;
		TextureManager::CheckTexHandle(specularMapTextureHandleL);
		uint64_t normalMapTextureHandleL = normalMapTextureHandle_;
		TextureManager::CheckTexHandle(normalMapTextureHandleL);

		//ラムダ式でコマンド関数
		SetMaterialTexM = [=]() {IObject3D::SetMaterialLightMTex(
			NULL, dissolveTextureHandleL, specularMapTextureHandleL, normalMapTextureHandleL, false); };
	}

	//メッシュのオフセットデータセット
	IObject3D::GetModel()->SetPolygonOffsetData(meshOffsetData_);

	//プリミティブ形状
	auto primitiveT = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	if (pipelineNum == PipelineStateNumObj::HULL_DOMAIN_OBJ)
	{
		primitiveT = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	}

	model_->Draw(primitiveT, SetRootPipeRM, SetMaterialTexM, cbt_, isShadow);
}

void Object::DrawUpdate(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
	Camera* camera, Camera* lightCamera, IModel* model)
{
	//シャドウマップ用の前描画かどうか
	bool isShadow = false;
	if ((indexNum == OBJ && pipelineNum == SHADOW_OBJ) ||
		(indexNum != OBJ && (pipelineNum == SHADOW_LINE || pipelineNum == SHADOW_PRIM)))
	{
		isShadow = true;
	}

	//プリミティブ形状
	auto primT = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	if ((indexNum != OBJ && pipelineNum == PipelineStateNumPrim::HULL_DOMAIN_PRIM)||
		(indexNum == OBJ && pipelineNum == PipelineStateNumObj::HULL_DOMAIN_OBJ))
	{
		primT = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	}

	//カメラがセットされてなければ使用されてるカメラを使う
	Camera* lCamera = camera;
	Camera* lLightCamera = lightCamera;
	if (camera == nullptr)
	{
		lCamera = CameraManager::GetInstance().usingCamera_;
	}
	if (lightCamera == nullptr)
	{
		lLightCamera = CameraManager::GetInstance().usingLightCamera_;
	}

	//行列マッピング
	IObject3D::MatMap(lCamera, lLightCamera, model);

	//ラムダ式でプリミティブのコマンド関数
	std::function<void()>SetRootPipeR = [=]() {SetRootPipe(primitivePipeLineSet_, pipelineNum,
		primitivePipeLineSet_[pipelineNum].rootSignature.Get()); };
	std::function<void()>SetMaterialTex = NULL;

	//シャドウマップ用の前描画では必要ない
	if (!isShadow)
	{
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
		SetMaterialTex = [=]() {SetMaterialLightMTex(textureHandleL, dissolveTextureHandleL, specularMapTextureHandleL,
			normalMapTextureHandleL, constBuffTransform); };
	}

	if (indexNum == TRIANGLE)
	{
		primitive_.TriangleDraw(primT, SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == BOX)
	{
		primitive_.BoxDraw(primT, SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CUBE)
	{
		primitive_.CubeDraw(primT, SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == LINE)
	{
		primitive_.LineDraw(SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == CIRCLE)
	{
		primitive_.CircleDraw(primT, SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == SPHERE)
	{
		primitive_.SphereDraw(primT, SetRootPipeR, SetMaterialTex);
	}
	else if (indexNum == OBJ)
	{
		//モデル用
		DrawModelInternal(pipelineNum);
	}
}

void Object::DrawTriangle(Camera* camera, Camera* lightCamera, uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(TRIANGLE, pipelineNum, textureHandle, &cbt_, camera, lightCamera);
}

void Object::DrawBox(Camera* camera, Camera* lightCamera, uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(BOX, pipelineNum, textureHandle, &cbt_, camera, lightCamera);
}

void Object::DrawCube3D(Camera* camera, Camera* lightCamera, uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(CUBE, pipelineNum, textureHandle, &cbt_, camera, lightCamera);
}

void Object::DrawLine(Camera* camera, Camera* lightCamera,
	uint64_t textureHandle)
{
	//線用のパイプラインを使うので2
	DrawUpdate(LINE, 2, textureHandle, &cbt_, camera, lightCamera, nullptr);
}

void Object::DrawCircle(Camera* camera, Camera* lightCamera,
	uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(CIRCLE, pipelineNum, textureHandle, &cbt_, camera, lightCamera);
}

void Object::DrawSphere(Camera* camera, Camera* lightCamera,
	uint64_t textureHandle, int32_t pipelineNum)
{
	DrawUpdate(SPHERE, pipelineNum, textureHandle, &cbt_, camera, lightCamera);
}

void Object::DrawModel(IModel* model, Camera* camera, Camera* lightCamera,
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

	DrawUpdate(OBJ, pipelineNum, NULL, &cbt_, camera, lightCamera, model);
}
