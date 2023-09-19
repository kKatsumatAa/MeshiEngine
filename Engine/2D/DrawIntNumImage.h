#pragma once
#include"Object.h"

class DrawIntNumImage
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private:
	struct NumImage
	{
		Object obj;
		Vec4 color;
		Vec2 sizeUV;
		Vec2 pos;
		int32_t num;
		float scale;
		Vec2 numImageSize;
		bool isTrue;
	};

private:
	NumImage numImages_[6];
	//�]��
	int32_t numRemainder_;
	//���̌�
	int32_t numDigit_;

	uint64_t texhandle_;

	bool isStartDigit_ = false;

	int32_t numCount_ = 0;


public:
	void Initialize(uint64_t texhandle);

	void SetNum(int32_t num,const Vec2& pos, const Vec2& sizeUV, const Vec2& numImageSize, float scale, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f });

	void Draw(Camera2D* camera);
};

