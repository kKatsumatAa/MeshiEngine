#include "RootPipe.h"

void RootPipe::CreateBlob(const wchar_t* vsName, const wchar_t* psName, const wchar_t* gsName,
	std::string vEPName, std::string pEPName, std::string gEPName)
{
	HRESULT result = {};

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		vsName,	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		vEPName.c_str(), "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		psName,	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		pEPName.c_str(), "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob_からエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
	//ジオメトリシェーダは基本読み込まない
	if (gsName != nullptr)
	{
		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			gsName,	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			gEPName.c_str(), "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlob_からエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
	}
}
