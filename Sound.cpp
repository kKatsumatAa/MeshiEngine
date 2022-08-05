#include "Sound.h"

SoundData SoundLoadWave(const char* filename)
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
    //assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);
    //16,18byteのフォーマットチャンクに対応
    //（40byteは×⇒40読み込んだ後で、先頭18byteだけコピーするといい）


//Dataチャンクの読み込み
    ChunkHeader data;
    file.read((char*)&data, sizeof(data));
    //junkチャンクを検出したら
    if (strncmp(data.id, "JUNK", 4) == 0)
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

void SoundUnLoad(SoundData* soundData)
{
    //バッファのメモリ開放
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}

void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
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

    //波形データの再生
    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}
