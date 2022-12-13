#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<cassert>
#include <fstream>
#include <string>
#include <wrl.h>
using namespace Microsoft::WRL;


class Sound final
{
private:

public:
	//��
	static IXAudio2MasteringVoice* masterVoice;
	//��
	static ComPtr<IXAudio2> xAudio2;

public://�T�u�N���X
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];//�`�����N���Ƃ�ID
		int32_t size;//�`�����N�T�C�Y
	};
	//RIFF�w�b�_
	struct RiffHeader
	{
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};
	//fmt�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//"�g�`�t�H�[�}�b�g"
	};

	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		BYTE* pBuffer = nullptr;
		//�o�b�t�@�̃T�C�Y
		unsigned int bufferSize;
		//
		IXAudio2SourceVoice* pSourceVoice = nullptr;
	};


private:
	Sound();
	~Sound();

public://�����o�֐�

	static void Initialize();

	/// <summary>
/// ���̓ǂݍ��݁i��2�͊�{true,�ǂݍ��߂Ȃ�/�Đ�����Ȃ��Ƃ���false�ɂ���j
/// </summary>
/// <param name="filename"></param>
/// <param name="isConvert"></param>
/// <returns></returns>
	static SoundData SoundLoadWave(const char* filename, const bool& isConvert);
	/// <summary>
	/// �������
	/// </summary>
	/// <param name="soudData"></param>
	/// <returns></returns>
	static void SoundUnLoad(SoundData* soundData);

	/// <summary>
	/// wave���f�[�^�Đ�
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	static void SoundPlayWave(SoundData& soundData, const float& volume = 5.0f, const bool& Loop = false);

	static void SoundStopWave(const SoundData& soundData);


public://�V���O���g��
	//�R�s�[�R���X�g���N�^�𖳌�
	Sound(const Sound& obj) = delete;
	//������Z�q��
	Sound& operator=(const Sound& obj) = delete;

	//�֐�
	static Sound& GetInstance();
};






