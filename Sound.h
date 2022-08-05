#pragma once
#include <wrl.h>
using namespace Microsoft::WRL;
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<cassert>
#include <fstream>



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
	BYTE* pBuffer;
	//�o�b�t�@�̃T�C�Y
	unsigned int bufferSize;
};

/// <summary>
/// ���f�[�^�ǂݍ���
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
SoundData SoundLoadWave(const char* filename);
/// <summary>
/// �������
/// </summary>
/// <param name="soudData"></param>
/// <returns></returns>
void SoundUnLoad(SoundData* soundData);

/// <summary>
/// wave���f�[�^�Đ�
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);


