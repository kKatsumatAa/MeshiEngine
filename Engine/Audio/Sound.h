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
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//��
	static IXAudio2MasteringVoice* sMasterVoice_;
	//��
	static ComPtr<IXAudio2> sXAudio2_;


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

	struct CheckSoundData
	{
		IXAudio2SourceVoice* pSourceVoice;
		XAUDIO2_BUFFER* pBuf;
	};

	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		std::vector<BYTE> pBuffer;
		//�o�b�t�@�̃T�C�Y
		uint32_t bufferSize;
		//
		std::vector<CheckSoundData> checkDatas;
	};

private:
	//�T�E���h�f�[�^�̘A�z�z��
	static std::map < std::string, SoundData> sSoundDatas_;
	//�T�E���h�i�[�f�B���N�g��
	static std::string sDirectoryPath_;


private:
	Sound();
	~Sound();

private:
	SoundData* GetSoundData(const std::string& name);

public://�����o�֐�

	static void Initialize(const std::string& directoryPath = "Resources/sound/");

	/// <summary>
/// ���̓ǂݍ��݁i��2�͊�{true,�ǂݍ��߂Ȃ�/�Đ�����Ȃ��Ƃ���false�ɂ���j
/// </summary>
/// <param name="filename"></param>
/// <param name="isConvert"></param>
/// <returns></returns>
	void LoadWave(const std::string& filename, bool isConvert);
	/// <summary>
	/// �������
	/// </summary>
	/// <param name="soudData"></param>
	/// <returns></returns>
	void UnLoad(SoundData* soundData);


	/// <summary>
	/// �����Đ�
	/// </summary>
	/// <param name="">wav�t�@�C����</param>
	/// <param name="volume">����</param>
	/// <param name="Loop">���[�v�Đ�</param>
	void PlayWave(const std::string& filename, float volume = 1.0f, bool Loop = false);

	void StopWave(const std::string& filename);

	//�Đ��~�܂�����
	bool CheckPlayingWave(const std::string& name);


public://�V���O���g��
	//�R�s�[�R���X�g���N�^�𖳌�
	Sound(const Sound& obj) = delete;
	//������Z�q��
	Sound& operator=(const Sound& obj) = delete;

	//�֐�
	static Sound& GetInstance();
};






