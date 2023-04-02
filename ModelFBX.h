#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>


struct Node
{
	//���O
	std::string name;
	//���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//���[�J����]�p
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//���[�J���ړ�
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//���[�J���ό`�s��
	DirectX::XMMATRIX transform;
	//�O���[�o���ό`�s��i�e�̉e�����܂߂��j
	DirectX::XMMATRIX globalTransform;
	//�e�m�[�h
	Node* parent = nullptr;
};

class ModelFBX
{
public:
	//�t�����h�N���X
	friend class FbxLoader;

#pragma region �ϐ�
private:
	//���f����
	std::string name;
	//�m�[�h�s��
	std::vector<Node> nodes;
public:

#pragma endregion

#pragma region �֐�
private:

public:
#pragma endregion
};

