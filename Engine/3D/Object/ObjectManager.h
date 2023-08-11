#pragma once
#include"Object.h"


class ObjectManager final
{
private:
	//�I�𒆂̃I�u�W�F�N�g
	Object* selectObj_ = nullptr;
	//�I�u�W�F�N�g�̔z����O���[�v���Ƃɕ�����
	std::map<std::string, std::vector<std::unique_ptr<Object>>> objsGroups_;


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

	std::map<std::string, std::vector<std::unique_ptr<Object>>>::iterator
		GetObjGroup(const std::string& name);

	//�O���[�v��ǉ�����
	void AddGroup(const std::string& groupName, std::unique_ptr<Object> inst);
	//�O���[�v�ɃI�u�W�F�N�g�ǉ��A�O���[�v���܂��Ȃ���΃O���[�v���z��ɒǉ�
	void AddObjAndGroup(std::unique_ptr<Object> obj, const std::string& groupName);

	//���葮���Ɠ����Ȃ�
	bool GetSameAttribute(Object* obj, uint16_t attribute);
	//�I�u�W�F�N�g�̖��O�Ƃ����Ă�����
	bool GetSameObjName(Object* obj, const std::string& name);



	//���O��t���ăI�u�W�F��ǉ��A�o�^
	std::vector<Object*> GetObjectInternal(const std::string& groupName,
		const std::function<bool(Object* obj)>& isF);

public:
	//���O��t���ăI�u�W�F��ǉ��A�o�^
	void AddObject(const std::string& groupName, const std::string& name);
	//���̂܂܃C���X�^���X��o�^()
	void AddObject(const std::string& groupName, std::unique_ptr<Object>inst);
	//���O���w�肵�ăI�u�W�F�̃|�C���g��z��ŕԂ�
	std::vector<Object*> GetObjs(const std::string& groupName, const std::string& name);
	//���葮�����w�肵�ăI�u�W�F�̃|�C���g��z��ŕԂ�
	std::vector<Object*> GetObjs(const std::string& groupName, uint16_t attribute);
	//�O���[�v�S��
	std::vector<Object*>GetObjs(const std::string& groupName);
	//�S��(�����Ȃ�)
	std::vector<Object*>GetObjs();

	//obj�N���A
	void ClearAllObj();
	//�O���[�v�N���A
	void ClearGroup(const std::string& groupName);
};