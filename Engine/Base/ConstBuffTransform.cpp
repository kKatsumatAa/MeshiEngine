#include "ConstBuffTransform.h"
#include "TextureManager.h"

std::map<uint64_t, ConstBuffTransform::ConstBuffTransformSaveData> ConstBuffTransform::sCbtDatas_;
D3D12_DESCRIPTOR_RANGE ConstBuffTransform::sDescRange_[2];
std::vector<uint64_t> ConstBuffTransform::canUseBuffsHandle_;


void ConstBuffTransform::StaticInitialize()
{
	//デスクリプタレンジの設定
	sDescRange_[0].NumDescriptors = 1;   //一度の描画に使う数
	sDescRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	sDescRange_[0].BaseShaderRegister = 1;  //b1
	sDescRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//メッシュ用
	sDescRange_[1] = sDescRange_[0];
	sDescRange_[1].BaseShaderRegister += 1;
}


ConstBuffTransform::~ConstBuffTransform()
{
	auto itr = sCbtDatas_.find(cbvGPUHandle_.ptr);

	//使用可能なバッファに登録
	if (itr != sCbtDatas_.end())
	{
		canUseBuffsHandle_.push_back(itr->first);
	}
}

void ConstBuffTransform::Initialize()
{
	//バッファのデータをもらう
	ConstBuffTransformDrawData data = CreateBuffAndAdd();

	//メンバ変数に格納
	cbvGPUHandle_ = data.cbvGPUHandle;
	constMapTransform_ = data.constMapTransform;
}

void ConstBuffTransform::DrawCommand(int32_t index)
{
	//定数バッファビュー(CBV)の設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(index, cbvGPUHandle_);
}

//----------------------------------------------------------------------------------------
ConstBuffTransform::ConstBuffTransformDrawData ConstBuffTransform::CreateBuffAndAdd()
{
	ConstBuffTransformDrawData data;
	//使用されてない作成済みバッファがあれば使わせる
	if (GetSaveData(data))
	{
		return data;
	}
	//なければ新たに作って保存、情報をもらう
	else
	{
		return CreateBuff();
	}
}

bool ConstBuffTransform::GetSaveData(ConstBuffTransformDrawData& data)
{
	for (auto& itr : sCbtDatas_)
	{
		//使用されてないバッファがあれば
		if (canUseBuffsHandle_.size())
		{
			uint64_t gpuHandle = canUseBuffsHandle_.back();

			auto itr = sCbtDatas_.find(gpuHandle);

			//実際にあればバッファの情報
			if (itr != sCbtDatas_.end())
			{
				data.cbvGPUHandle.ptr = itr->first;
				data.constMapTransform = itr->second.constMapTransform;
			}

			//使用されているので排除
			canUseBuffsHandle_.pop_back();
		}
	}

	return false;
}

ConstBuffTransform::ConstBuffTransformAllData ConstBuffTransform::CreateBuffInternal()
{
	ConstBuffTransformAllData alldata;

	HRESULT result;

	//バッファ作成
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};

		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff/*256バイトアライメント*/;
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&cbHeapProp,//ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,//リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&alldata.saveData.constBuffTransform));
		assert(SUCCEEDED(result));

	}
	//定数バッファのマッピング
	ConstBufferDataTransform* map;
	result = alldata.saveData.constBuffTransform->Map(0, nullptr, (void**)&map);//マッピング
	assert(SUCCEEDED(result));
	alldata.drawData.constMapTransform = map;
	alldata.saveData.constMapTransform = map;


	{
		//ビュー作成
		D3D12_CPU_DESCRIPTOR_HANDLE cbvCPUHandle;
		cbvCPUHandle = TextureManager::GetDescHeapP()->GetCPUDescriptorHandleForHeapStart();
		cbvCPUHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TextureManager::GetSRVCount();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = alldata.saveData.constBuffTransform->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (uint32_t)alldata.saveData.constBuffTransform->GetDesc().Width;

		DirectXWrapper::GetInstance().GetDevice()->CreateConstantBufferView(&cbvDesc, cbvCPUHandle);
	}

	//Gpuのハンドルをゲット
	{
		//gpuでのスタート位置+インクリメントサイズ*カウント
		alldata.drawData.cbvGPUHandle = TextureManager::GetDescHeapP()->GetGPUDescriptorHandleForHeapStart();
		alldata.drawData.cbvGPUHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TextureManager::GetSRVCount();
	}

	//ヒープのカウント進める
	TextureManager::AddSRVHandleCount();

	return alldata;
}

ConstBuffTransform::ConstBuffTransformDrawData ConstBuffTransform::CreateBuff()
{
	//バッファを作成し情報をもらう
	ConstBuffTransformAllData alldata = CreateBuffInternal();

	//情報を保存
	sCbtDatas_.insert(std::make_pair(alldata.drawData.cbvGPUHandle.ptr, alldata.saveData));

	//描画に必要なものだけ返す
	return alldata.drawData;
}
