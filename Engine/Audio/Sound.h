#pragma once
#include<xaudio2.h>
#include<cassert>
#include <fstream>
#include <string>
#include <wrl.h>
#include <map>
#include <vector>


class Sound final
{
private:
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//音
	static IXAudio2MasteringVoice* sMasterVoice_;
	//音
	static ComPtr<IXAudio2> sXAudio2_;


public://サブクラス
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

	struct CheckSoundData
	{
		IXAudio2SourceVoice* pSourceVoice;
		XAUDIO2_BUFFER* pBuf;
	};

	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		std::vector<BYTE> pBuffer;
		//バッファのサイズ
		uint32_t bufferSize;
		//
		std::vector<CheckSoundData> checkDatas;
	};

private:
	//サウンドデータの連想配列
	static std::map < std::string, SoundData> sSoundDatas_;
	//サウンド格納ディレクトリ
	static std::string sDirectoryPath_;


private:
	Sound();
	~Sound();

private:
	SoundData* GetSoundData(const std::string& name);

public://メンバ関数

	static void Initialize(const std::string& directoryPath = "Resources/sound/");

	/// <summary>
/// 音の読み込み（第2は基本true,読み込めない/再生されないときはfalseにする）
/// </summary>
/// <param name="filename"></param>
/// <param name="isConvert"></param>
/// <returns></returns>
	void LoadWave(const std::string& filename, bool isConvert);
	/// <summary>
	/// 解放処理
	/// </summary>
	/// <param name="soudData"></param>
	/// <returns></returns>
	void UnLoad(SoundData* soundData);


	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="">wavファイル名</param>
	/// <param name="volume">音量</param>
	/// <param name="Loop">ループ再生</param>
	void PlayWave(const std::string& filename, float volume = 1.0f, bool Loop = false);

	void StopWave(const std::string& filename);

	//再生止まったか
	bool CheckPlayingWave(const std::string& name);


public://シングルトン
	//コピーコンストラクタを無効
	Sound(const Sound& obj) = delete;
	//代入演算子も
	Sound& operator=(const Sound& obj) = delete;

	//関数
	static Sound& GetInstance();
};






