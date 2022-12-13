#include "Sound.h"

//音
IXAudio2MasteringVoice* Sound::masterVoice;
//音
ComPtr<IXAudio2> Sound::xAudio2;


Sound::Sound()
{
}

Sound::~Sound()
{
}

void Sound::Initialize()
{
	HRESULT result;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if FAILED(result) {
		assert(0);
	}

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	if FAILED(result) {
		assert(0);
	}
}

Sound::SoundData Sound::SoundLoadWave(const char* filename, const bool& isConvert)
{
	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	assert(file.is_open());

	//RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//fileがRiffかチェック(先頭四文字をチェック)
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}
	//元データがmp3だと×！（拡張子を変えてもダメ⇒コンバータで変換）


//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)//"fmt "最後半角スペース！！！
	{
		assert(0);
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
	if (strncmp(data.id, "JUNK", 4) != isConvert)//引数によって変えられる
	{
		//junkチャンクの最後まで読み飛ばす(読み取りヘッドを動かす)
		file.seekg(data.size, std::ios_base::cur);
		//(チャンクヘッダを)再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//本物のデータチャンク
	if (strncmp(data.id, "data", 4 != 0))
	{
		assert(0);
	}
	//Dataチャンクのデータ部（波形データの読み込み）
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);


	//waveファイルを閉じる
	file.close();


	//returnするための音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;//波形のフォーマット
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);//波形データ
	soundData.bufferSize = data.size;//波形データのサイズ

	return SoundData(soundData);
}

void Sound::SoundUnLoad(SoundData* soundData)
{
	//バッファのメモリ開放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Sound::SoundPlayWave(SoundData& soundData, const float& volume, const bool& Loop)
{
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//ループ再生
	if (Loop == true) { buf.LoopCount = XAUDIO2_LOOP_INFINITE; }

	//波形データの再生&設定
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();

	soundData.pSourceVoice = pSourceVoice;
}

void Sound::SoundStopWave(const SoundData& soundData)
{
	if (soundData.pSourceVoice != nullptr)
		soundData.pSourceVoice->Stop();
}

Sound& Sound::GetInstance()
{
	static Sound inst; // private なコンストラクタを呼び出す。
	return inst;
}

//void SetpSourceVoice(SoundData& soundData, IXAudio2SourceVoice* pSourceVoice)
//{
//    soundData.pSourceVoice = pSourceVoice;
//}