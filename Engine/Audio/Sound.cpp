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
	//delete sMasterVoice_;

	sXAudio2_.Reset();

	//�����f�[�^���
	std::map < std::string, Sound::SoundData>::iterator it = sSoundDatas_.begin();

	for (; it != sSoundDatas_.end(); ++it)
	{
		//second�̓y�A�̓��
		UnLoad(&it->second);
	}
	sSoundDatas_.clear();
}

void Sound::Initialize(const std::string& directoryPath)
{
	Sound::sDirectoryPath_ = directoryPath;

	HRESULT result;

	// XAudio�G���W���̃C���X�^���X�𐶐�
	result = XAudio2Create(&sXAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if FAILED(result) {
		assert(0);
	}

	// �}�X�^�[�{�C�X�𐶐�
	result = sXAudio2_->CreateMasteringVoice(&sMasterVoice_);
	if FAILED(result) {
		assert(0);
	}
}

void Sound::LoadWave(const std::string& filename, bool isConvert)
{
	std::string fullpath = sDirectoryPath_ + filename;

	//�����������[�h���Ă��Ȃ���
	if (sSoundDatas_.find(fullpath) != sSoundDatas_.end()) { return; }

	//�t�@�C�����̓X�g���[���̃C���X�^���X
	std::ifstream file;

	//wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(fullpath, std::ios_base::binary);
	assert(file.is_open());

	//RIFF�w�b�_�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//file��Riff���`�F�b�N(�擪�l�������`�F�b�N)
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//�^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}
	//���f�[�^��mp3���Ɓ~�I�i�g���q��ς��Ă��_���˃R���o�[�^�ŕϊ��j


//Format�`�����N�̓ǂݍ���
	FormatChunk format = {};
	//�`�����N�w�b�_�[�̊m�F
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)//"fmt "�Ō㔼�p�X�y�[�X�I�I�I
	{
		assert(0);
	}

	//�`�����N�{�̂̓ǂݍ���
	if (format.chunk.size > sizeof(format.fmt))//18byte���傫��������
	{
		int32_t size = format.chunk.size - sizeof(format.fmt);
		//18byte���ǂݎ���Ă���
		file.read((char*)&format.fmt, sizeof(format.fmt));

		//����format
		FormatChunk format2 = {};
		//���̍����ǂݎ���Đi�߂�
		file.read((char*)&format2.fmt, size);
	}
	else
	{
		//assert(format.chunk.size <= sizeof(format.fmt));
		file.read((char*)&format.fmt, format.chunk.size);
		//16,18byte�̃t�H�[�}�b�g�`�����N�ɑΉ�
		//�i40byte�́~��40�ǂݍ��񂾌�ŁA�擪18byte�����R�s�[����Ƃ����j
	}

	//Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//junk�`�����N�����o������
	if ((strncmp(data.id, "JUNK", 4) > 0) != isConvert)//�����ɂ���ĕς�����
	{
		//junk�`�����N�̍Ō�܂œǂݔ�΂�(�ǂݎ��w�b�h�𓮂���)
		file.seekg(data.size, std::ios_base::cur);
		//(�`�����N�w�b�_��)�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	//�{���̃f�[�^�`�����N
	if (strncmp(data.id, "data", 4 != 0))
	{
		assert(0);
	}

	//return���邽�߂̉����f�[�^
	SoundData soundData = {};

	soundData.wfex = format.fmt;//�g�`�̃t�H�[�}�b�g
	//Data�`�����N�̃f�[�^���i�g�`�f�[�^�̓ǂݍ��݁j
	soundData.pBuffer.resize(data.size);
	file.read((char*)&soundData.pBuffer[0], data.size);

	soundData.bufferSize = data.size;//�g�`�f�[�^�̃T�C�Y


	//wave�t�@�C�������
	file.close();

	//�T�E���h�f�[�^��A�z�z��Ɋi�[(�������ăZ�b�g�Ń}�b�v�Ɋi�[)
	sSoundDatas_.insert(std::make_pair(fullpath, soundData));
}

void Sound::UnLoad(SoundData* soundData)
{
	//�o�b�t�@�̃������J��
	soundData->pBuffer.clear();

	//soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Sound::PlayWave(const std::string& filename, float volume, bool Loop)
{
	HRESULT result;

	std::string fullpath = sDirectoryPath_ + filename;

	//�t�@�C��������T��
	std::map < std::string, Sound::SoundData>::iterator it
		= sSoundDatas_.find(fullpath);
	//���ǂݍ��݂̌��o
	assert(it != sSoundDatas_.end());
	//�T�E���h�f�[�^�̎Q�Ƃ��擾
	SoundData& soundData = it->second;

	//�g�`�t�H�[�}�b�g������SourceVoice�̐���
	IXAudio2SourceVoice* sourceVoice = nullptr;
	result = sXAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = &soundData.pBuffer[0];
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//���[�v�Đ�
	if (Loop == true) { buf.LoopCount = XAUDIO2_LOOP_INFINITE; }

	//�g�`�f�[�^�̍Đ�&�ݒ�
	result = sourceVoice->SubmitSourceBuffer(&buf);
	result = sourceVoice->SetVolume(volume);
	result = sourceVoice->Start();

	//��~�p
	soundData.pSourceVoice.push_back(sourceVoice);
}

Sound::SoundData* Sound::GetSoundData(const std::string& name)
{
	std::string fullpath = sDirectoryPath_ + name;

	//�t�@�C��������T��
	std::map < std::string, Sound::SoundData>::iterator it
		= sSoundDatas_.find(fullpath);
	//���ǂݍ��݂̌��o
	assert(it != sSoundDatas_.end());

	//���̃t�@�C�����������
	return &it->second;
}

void Sound::StopWave(const std::string& filename)
{
	//���̃t�@�C�����������
	SoundData* soundData = GetSoundData(filename);

	if (soundData->pSourceVoice.size() > 0)
	{
		for (IXAudio2SourceVoice* source : soundData->pSourceVoice)
		{
			//�Đ����̓������O�̂����ׂĎ~�߂�
			if (source != nullptr) { source->Stop(); }
		}

		//vector����ɂ���
		soundData->pSourceVoice.clear();
	}
}
bool Sound::CheckPlayingWave(const std::string& name)
{
	SoundData* soundData = GetSoundData(name);

	if (soundData->pSourceVoice.size() > 0)
	{
		for (IXAudio2SourceVoice* source : soundData->pSourceVoice)
		{
			if (source != nullptr)
			{
				//�Đ����I����ĂȂ����
				HRESULT result = source->ExitLoop();
				if (result == S_OK)
				{
					return false;
				}
			}
		}
	}

	return true;
}
Sound& Sound::GetInstance()
{
	static Sound sInst; // private �ȃR���X�g���N�^���Ăяo���B
	return sInst;
}