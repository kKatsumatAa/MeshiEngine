#include "RootPipe.h"
#include "Util.h"

void RootPipe::CreateBlob(const char* vsName, const char* psName, const char* gsName,
	const char* hsName, const char* dsName,
	std::string vEPName, std::string pEPName, std::string gEPName
	, std::string hEPName, std::string dEPName)
{
	HRESULT result = {};

	wchar_t vsNameL[128];
	ConstCharToWcharT(vsName, vsNameL);

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		vsNameL,	// シェーダファイル名
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
	if (strlen(psName))
	{
		wchar_t psNameL[128];
		ConstCharToWcharT(psName, psNameL);
		result = D3DCompileFromFile(
			psNameL,	// シェーダファイル名
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
	}

	//ジオメトリシェーダは基本読み込まない
	if (gsName != nullptr && strlen(gsName))
	{
		wchar_t gsNameL[128];
		ConstCharToWcharT(gsName, gsNameL);

		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			gsNameL,	// シェーダファイル名
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

	//ハルシェーダは基本読み込まない
	if (hsName != nullptr && strlen(hsName))
	{
		wchar_t hsNameL[128];
		ConstCharToWcharT(hsName, hsNameL);

		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			hsNameL,	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			hEPName.c_str(), "hs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&hsBlob, &errorBlob);
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

	//ドメインシェーダは基本読み込まない
	if (dsName != nullptr && strlen(dsName))
	{
		wchar_t dsNameL[128];
		ConstCharToWcharT(dsName, dsNameL);

		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			dsNameL,	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			dEPName.c_str(), "ds_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&dsBlob, &errorBlob);
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
