/*
* @file Object.h
* @brief .objのオブジェクトクラス
*/

#pragma once
#include "IObject3D.h"
#include "Primitive.h"


//オブジェクトクラス
class Object : public IObject3D
{
private:
	// オブジェクトのタイプ
	enum ObjType
	{
		TRIANGLE,
		BOX,
		CUBE,
		LINE,
		CIRCLE,
		SPHERE,
		SPRITE,
		OBJ,
		FBX
	};

protected:
	enum PipelineStateNumObj
	{
		NORMAL_OBJ,
		WIREFRAME_OBJ,
		SHADOW_OBJ,
		HULL_DOMAIN_OBJ,
		SHADOW_HULL_DOMAIN_OBJ,

		COUNT_O//要素数
	};

	enum PipelineStateNumPrim
	{
		NORMAL_PRIM,
		WIREFRAME_PRIM,
		SHADOW_PRIM,
		HULL_DOMAIN_PRIM,

		NORMAL_LINE,
		SHADOW_LINE,

		COUNT_P//要素数
	};

private:
	// 頂点レイアウトの設定
// 頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC sInputLayoutPrimitive_[5];

private:
	//図形のクラス
	static Primitive primitive_;
	//パイプラインなどの設定
	static RootPipe primitivePipeLineSet_[PipelineStateNumPrim::COUNT_P];
	//.objの
	static RootPipe pipelineSetM_[PipelineStateNumObj::COUNT_O];

private:
	//描画する形状を指定されたら呼ぶ描画用関数
	void DrawUpdate(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
		Camera* camera, Camera* lightCamera, IModel* model = nullptr);

public:
	//コンストラクタ
	Object();

	//デストラクタ
	virtual ~Object();

public:
	//優先して最初の方に初期化
	static void CommonInitialize();

	virtual void Update()override;

	void DrawModelInternal(int32_t pipelineNum)override;

	virtual void Draw()override;

private:
	//シャドウマップ用に事前に描画
	void PreDrawShadow()override;

public:
	void DrawTriangle(Camera* camera = nullptr, Camera* lightCamera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, Camera* lightCamera = nullptr,

		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawCube3D(Camera* camera = nullptr, Camera* lightCamera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawLine(Camera* camera = nullptr, Camera* lightCamera = nullptr,
		uint64_t textureHandle = NULL);

	void DrawCircle(Camera* camera = nullptr, Camera* lightCamera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawSphere(Camera* camera = nullptr, Camera* lightCamera = nullptr,
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawModel(IModel* model, Camera* camera = nullptr, Camera* lightCamera = nullptr,
		int32_t pipelineNum = 0);
};
