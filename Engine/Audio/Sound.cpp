#include "Sound.h"
#include <iostream>

using namespace Microsoft::WRL;

#pragma comment(lib,"xaudio2.lib")

IXAudio2MasteringVoice* Sound::sMasterVoice_;
ComPtr<IXAudio2> Sound::sXAudio2_;
std::map < std::string, Sound::SoundData> Sound::sSoundDatas_;
std::string Sound::sDirectoryPath_ = "Resources/sound/";

Sound::Sound()
{
}

Sound::~Sound()
{
	sMasterVoice_->DestroyVoice();

	sXAudio2_.Reset();

	//音声データ解放
	std::map < std::string, Sound::SoundData>::iterator it = sSoundDatas_.begin();

	for (; it != sSoundDatas_.end(); ++it)
	{
		//secondはペアの二つ目
		UnLoad(&it->second);
	}
	sSoundDatas_.clear();
}

void Sound::Initialize(const std::string& directoryPath)
{
	Sound::sDirectoryPath_ = directoryPath;

	HRESULT result;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&sXAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if FAILED(result) {
		assert(false);
	}

	// マスターボイスを生成
	result = sXAudio2_->CreateMasteringVoice(&sMasterVoice_);
	if FAILED(result) {
		assert(false);
	}
}

void Sound::LoadWave(const std::string& filename, bool isConvert)
{
	std::string fullpath = sDirectoryPath_ + filename;

	//同じ音をロードしていないか
	if (sSoundDatas_.find(fullpath) != sSoundDatas_.end()) { return; }

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//wavファイルをバイナリモードで開く
	file.open(fullpath, std::ios_base::binary);
	assert(file.is_open());

	//RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//fileがRiffかチェック(先頭四文字をチェック)
	if (strncmp(riff.chunk.id, RIFF_STR_.c_str(), RIFF_STR_.size()) != 0)
	{
		assert(false);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, WAVE_STR_.c_str(), WAVE_STR_.size()) != 0)
	{
		assert(false);
	}
	//元データがmp3だと×！（拡張子を変えてもダメ⇒コンバータで変換）


//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, FMT_STR_.c_str(), FMT_STR_.size()) != 0)//"fmt "最後半角スペース！！！
	{
		assert(false);
	}

	//チャンク本体の読み込み
	if (format.chunk.size > sizeof(format.fmt))//18byteより大きかったら
	{
		int32_t size = format.chunk.size - sizeof(format.fmt);
		//18byte分読み取っておく
		file.read((char*)&format.fmt, sizeof(format.fmt));

		//仮のformat
		FormatChunk format2 = {};
		//その差分読み取って進める
		file.read((char*)&format2.fmt, size);
	}
	else
	{
		//assert(format.chunk.size <= sizeof(format.fmt));
		file.read((char*)&format.fmt, format.chunk.size);
		//16,18byteのフォーマットチャンクに対応
		//（40byteは×⇒40読み込んだ後で、先頭18byteだけコピーするといい）
	}

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//junkチャンクを検出したら
	if ((strncmp(data.id, JUNK_STR_.c_str(), JUNK_STR_.size()) > 0) != isConvert)//引数によって変えられる
	{
		//junkチャンクの最後まで読み飛ばす(読み取りヘッドを動かす)
		file.seekg(data.size, std::ios_base::cur);
		//(チャンクヘッダを)再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//本物のデータチャンク
	if (strncmp(data.id, DATA_STR_.c_str(), DATA_STR_.size() != 0))
	{
		assert(false);
	}

	//returnするための音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;//波形のフォーマット
	//Dataチャンクのデータ部（波形データの読み込み）
	soundData.pBuffer.resize(data.size);
	file.read((char*)&soundData.pBuffer[0], data.size);

	soundData.bufferSize = data.size;//波形データのサイズ


	//waveファイルを閉じる
	file.close();

	//サウンドデータを連想配列に格納(複製してセットでマップに格納)
	sSoundDatas_.insert(std::make_pair(fullpath, soundData));
}

void Sound::UnLoad(SoundData* soundData)
{
	//バッファのメモリ開放
	soundData->pBuffer.clear();

	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Sound::PlayWave(const std::string& filename, float volume, bool Loop)
{
	HRESULT result;

	std::string fullpath = sDirectoryPath_ + filename;

	//ファイル名から探す
	std::map < std::string, Sound::SoundData>::iterator it
		= sSoundDatas_.find(fullpath);
	//未読み込みの検出
	assert(it != sSoundDatas_.end());
	//サウンドデータの参照を取得
	SoundData& soundData = it->second;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	result = sXAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = &soundData.pBuffer[0];
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//ループ再生
	if (Loop == true) { buf.LoopCount = XAUDIO2_LOOP_INFINITE; }

	//波形データの再生&設定
	result = sourceVoice->SubmitSourceBuffer(&buf);
	result = sourceVoice->SetVolume(volume);
	result = sourceVoice->Start();

	//停止用
	CheckSoundData checkDataL = { sourceVoice, &buf };
	soundData.checkDatas.push_back(checkDataL);
}

Sound::SoundData* Sound::GetSoundData(const std::string& name)
{
	std::string fullpath = sDirectoryPath_ + name;

	//ファイル名から探す
	std::map < std::string, Sound::SoundData>::iterator it
		= sSoundDatas_.find(fullpath);
	//未読み込みの検出
	assert(it != sSoundDatas_.end());

	//そのファイル名があれば
	return &it->second;
}

void Sound::StopWave(const std::string& filename)
{
	//そのファイル名があれば
	SoundData* soundData = GetSoundData(filename);

	if (soundData->checkDatas.size() > 0)
	{
		for (auto checkData : soundData->checkDatas)
		{
			//再生中の同じ名前のをすべて止める
			if (checkData.pSourceVoice != nullptr) { checkData.pSourceVoice->Stop(); }
		}

		//vectorを空にする
		soundData->checkDatas.clear();
	}
}
bool Sound::CheckPlayingWave(const std::string& name)
{
	SoundData* soundData = GetSoundData(name);

	if (soundData->checkDatas.size() > 0)
	{
		//一つでも再生されてたら
		for (auto checkData : soundData->checkDatas)
		{
			if (checkData.pSourceVoice == nullptr)
			{
				continue;
			}

			XAUDIO2_VOICE_STATE state;
			checkData.pSourceVoice->GetState(&state);
			if (state.SamplesPlayed < checkData.pBuf->PlayLength && 0 != state.SamplesPlayed)
			{
				return true;
			}
		}
	}

	return false;
}
Sound& Sound::GetInstance()
{
	static Sound sInst; // private なコンストラクタを呼び出す。
	return sInst;
}