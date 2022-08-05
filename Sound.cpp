#include "Sound.h"

SoundData SoundLoadWave(const char* filename)
{
    //�t�@�C�����̓X�g���[���̃C���X�^���X
    std::ifstream file;
    
    //wav�t�@�C�����o�C�i�����[�h�ŊJ��
    file.open(filename, std::ios_base::binary);
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
    //assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);
    //16,18byte�̃t�H�[�}�b�g�`�����N�ɑΉ�
    //�i40byte�́~��40�ǂݍ��񂾌�ŁA�擪18byte�����R�s�[����Ƃ����j


//Data�`�����N�̓ǂݍ���
    ChunkHeader data;
    file.read((char*)&data, sizeof(data));
    //junk�`�����N�����o������
    if (strncmp(data.id, "JUNK", 4) == 0)
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
    //Data�`�����N�̃f�[�^���i�g�`�f�[�^�̓ǂݍ��݁j
    char* pBuffer = new char[data.size];
    file.read(pBuffer, data.size);


//wave�t�@�C�������
    file.close();


//return���邽�߂̉����f�[�^
    SoundData soundData = {};

    soundData.wfex = format.fmt;//�g�`�̃t�H�[�}�b�g
    soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);//�g�`�f�[�^
    soundData.bufferSize = data.size;//�g�`�f�[�^�̃T�C�Y

    return SoundData(soundData);
}

void SoundUnLoad(SoundData* soundData)
{
    //�o�b�t�@�̃������J��
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}

void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
    HRESULT result;

    //�g�`�t�H�[�}�b�g������SourceVoice�̐���
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    //�Đ�����g�`�f�[�^�̐ݒ�
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    //�g�`�f�[�^�̍Đ�
    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}
