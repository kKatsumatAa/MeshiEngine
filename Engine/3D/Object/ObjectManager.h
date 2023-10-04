#pragma once
#include"IObject3D.h"


//����3D�݂̂̊Ǘ�
class ObjectManager final
{
private:
	//�I�𒆂̃I�u�W�F�N�g
	IObject3D* selectObj_ = nullptr;
	//�I�u�W�F�N�g�̔z����O���[�v���Ƃɕ�����
	std::map<std::string, std::vector<std::unique_ptr<IObject3D>>> objsGroups_;


private:
	ObjectManager() { ; }
	~ObjectManager() { ; }

public:
	ObjectManager& operator=(const ObjectManager& inst) = delete;
	ObjectManager(const ObjectManager& inst) = delete;

	static ObjectManager& GetInstance();

public:
	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();
	void PostUpdate();

public:
	//�f�o�b�O�p�A�b�v�f�[�g(�R���C�_�[���Ȃ��ƃN���b�N���Ă��������Ȃ�)
	void DebugUpdate();

private:
	//�O���[�v�����邩���ׂ�
	bool FindObjGroup(const std::string& name);

	std::map<std::string, std::vector<std::unique_ptr<IObject3D>>>::iterator
		GetObjGroup(const std::string& name);

	//�O���[�v��ǉ�����
	void AddGroup(const std::string& groupName, std::unique_ptr<IObject3D> inst);
	//�O���[�v�ɃI�u�W�F�N�g�ǉ��A�O���[�v���܂��Ȃ���΃O���[�v���z��ɒǉ�
	void AddObjAndGroup(std::unique_ptr<IObject3D> obj, const std::string& groupName);

	//���葮���Ɠ����Ȃ�
	bool GetSameAttribute(IObject3D* obj, uint16_t attribute);
	//�I�u�W�F�N�g�̖��O�Ƃ����Ă�����
	bool GetSameObjName(IObject3D* obj, const std::string& name);



	//���O��t���ăI�u�W�F��ǉ��A�o�^
	std::vector<IObject3D*> GetObjectInternal(const std::string& groupName,
		const std::function<bool(IObject3D* obj)>& isF);

public:
	//���O��t���ăI�u�W�F��ǉ��A�o�^
	void AddObject(const std::string& groupName, const std::string& name);
	//���̂܂܃C���X�^���X��o�^()
	void AddObject(const std::string& groupName, std::unique_ptr<IObject3D>inst);
	//���O���w�肵�ăI�u�W�F�̃|�C���g��z��ŕԂ�
	std::vector<IObject3D*> GetObjs(const std::string& groupName, const std::string& name);
	//���葮�����w�肵�ăI�u�W�F�̃|�C���g��z��ŕԂ�
	std::vector<IObject3D*> GetObjs(const std::string& groupName, uint16_t attribute);
	//�O���[�v�S��
	std::vector<IObject3D*>GetObjs(const std::string& groupName);
	//�S��(�����Ȃ�)
	std::vector<IObject3D*>GetObjs();

	//obj�N���A
	void ClearAllObj();
	//�O���[�v�N���A
	void ClearGroup(const std::string& groupName);
};