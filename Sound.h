#pragma once
#include <wrl.h>
using namespace Microsoft::WRL;
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<cassert>
#include <fstream>



//チャンクヘッダ
struct ChunkHeader
{
	char id[4];//チャンクごとのID
	int32_t size;//チャンクサイズ
};
//RIFFヘッダ
struct RiffHeader
{
	ChunkHeader chunk;//"RIFF"
	char type[4];//"WAVE"
};
//fmtチャンク
struct FormatChunk
{
	ChunkHeader chunk;//"fmt"
	WAVEFORMATEX fmt;//"波形フォーマット"
};

struct SoundData
{
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;
	//
	IXAudio2SourceVoice* pSourceVoice = nullptr;
};

/// <summary>
/// 音データ読み込み
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
SoundData SoundLoadWave(const char* filename, const bool& isConvert);
/// <summary>
/// 解放処理
/// </summary>
/// <param name="soudData"></param>
/// <returns></returns>
void SoundUnLoad(SoundData* soundData);

/// <summary>
/// wave音データ再生
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
void SoundPlayWave(IXAudio2* xAudio2, SoundData& soundData, const float& volume = 10.0f, const bool& Loop = false);

void SoundStopWave(const SoundData& soundData);


