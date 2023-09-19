#include "CameraManager.h"
#include "MouseInput.h"
#include "ImGuiManager.h"

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
	cameraAndNames_.clear();
}

void CameraManager::ChangeCamera()
{
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_V))
	{
		cameraItr_++;

		//�Ō�܂ōs������
		if (cameraItr_ == cameraAndNames_.end())
		{
			cameraItr_ = cameraAndNames_.begin();
		}

		//�J�����|�C���^���Z�b�g
		usingCamera_ = cameraItr_->second.get();
	}
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_B))
	{
		camera2DItr_++;

		//�Ō�܂ōs������
		if (camera2DItr_ == camera2DAndNames_.end())
		{
			camera2DItr_ = camera2DAndNames_.begin();
		}

		//�J�����|�C���^���Z�b�g
		usingCamera2D_ = camera2DItr_->second.get();
	}
}

void CameraManager::Initialize()
{
	cameraAndNames_.clear();

#ifdef _DEBUG
	//3D
	AddCamera("debugCamera");
	GetCamera("debugCamera")->Initialize();
	SetUsingCamera("debugCamera");
	//2D
	AddCamera2D("debugCamera");
	GetCamera2D("debugCamera")->Initialize();
	SetUsingCamera2D("debugCamera");

#endif

	//�J���������ɐ؂�ւ��邽�߂̃C�e���[�^
	cameraItr_ = cameraAndNames_.begin();
	camera2DItr_ = camera2DAndNames_.begin();
}

void CameraManager::Update()
{
#ifdef _DEBUG
	{
		Camera* debugCamera = GetCamera("debugCamera");

		if (KeyboardInput::GetInstance().KeyTrigger(DIK_C))
		{
			//�f�o�b�O�J�����ɕύX
			if (usingCamera_ != debugCamera)
			{
				isDebugCamera_ = true;
				afterCamera_ = usingCamera_;
				usingCamera_ = debugCamera;
			}
			else if (afterCamera_)
			{
				isDebugCamera_ = false;
				usingCamera_ = afterCamera_;
			}
		}

		//�f�o�b�O�J�������L���̎�
		if (usingCamera_ == debugCamera)
		{

			if (MouseInput::GetInstance().GetClick(CLICK_RIGHT))
			{
				Vec2 vel = MouseInput::GetInstance().GetCursorVelocity();

				//��]
				Vec3 rotMove = {
					vel.y * 0.01f,
					vel.x * 0.01f,
					0
				};

				debugWorldMat_.rot_ += rotMove;
				debugWorldMat_.rot_.x = min(debugWorldMat_.rot_.x, PI / 2.0f);
				debugWorldMat_.rot_.x = max(debugWorldMat_.rot_.x, -PI / 2.0f);
			}

			cameraPos_.z = min(cameraPos_.z + (float)MouseInput::GetInstance().GetWheelAmountOfRot() * 0.02f, -1.0f);
			cameraPos_.z = max(cameraPos_.z + (float)MouseInput::GetInstance().GetWheelAmountOfRot() * 0.02f, -1000.0f);

			debugWorldMat_.CalcWorldMat();
			Vec3 pos = cameraPos_;
			Vec3xM4(pos, debugWorldMat_.matWorld_, 0);
			debugCamera->SetEye(debugCamera->GetTarget() + pos);

			debugCamera->UpdateViewMatrix();
		}
	}
#endif
	//�J�����؂�ւ�
	ChangeCamera();

	if (usingCamera_)
	{
		usingCamera_->Update();
	}
	if (usingCamera2D_)
	{
		usingCamera2D_->Update();
	}
}

void CameraManager::ImguiUpdate()
{
	ImGui::Begin("CameraManager");

	if (ImGui::TreeNode("UsingCamera") && usingCamera_)
	{
		Vec3 eye = usingCamera_->GetEye();
		Vec3 target = usingCamera_->GetTarget();
		Vec3 up = usingCamera_->GetUp();

		ImGui::DragFloat3("Eye", &eye.x);
		ImGui::DragFloat3("Target", &target.x);
		ImGui::DragFloat3("Up", &up.x);

		usingCamera_->SetEye(eye);
		usingCamera_->SetTarget(target);
		usingCamera_->SetUp(up);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("UsingCamera2D") && usingCamera2D_)
	{
		Vec2 pos = usingCamera2D_->GetPos();
		float rot = usingCamera2D_->GetRot();
		Vec2 zoom = usingCamera2D_->GetZoom();

		ImGui::DragFloat2("Pos", &pos.x);
		ImGui::DragFloat("Rot", &rot);
		ImGui::DragFloat2("Zoom", &zoom.x);

		usingCamera2D_->SetPos(pos);
		usingCamera2D_->SetRot(rot);
		usingCamera2D_->SetZoom(zoom); 
		
		ImGui::TreePop();
	}

	for (auto& cameras : cameraAndNames_)
	{
		if (ImGui::TreeNode(cameras.first.c_str()))
		{
			Vec3 eye = cameras.second->GetEye();
			Vec3 target = cameras.second->GetTarget();
			Vec3 up = cameras.second->GetUp();

			ImGui::DragFloat3("Eye", &eye.x);
			ImGui::DragFloat3("Target", &target.x);
			ImGui::DragFloat3("Up", &up.x);

			cameras.second->SetEye(eye);
			cameras.second->SetTarget(target);
			cameras.second->SetUp(up);

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void CameraManager::AddCamera(const std::string& cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//���łɓ������O�̃J��������������
	if (it != cameraAndNames_.end())
	{
		return;
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Initialize();

	//�ۑ����Ă���
	cameraAndNames_.insert(std::make_pair(cameraName, std::move(camera)));
}

void CameraManager::AddCamera2D(const std::string& cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera2D>>::iterator it = camera2DAndNames_.find(cameraName);
	//���łɓ������O�̃J��������������G���[
	if (it != camera2DAndNames_.end())
	{
		return;
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<Camera2D> camera = std::make_unique<Camera2D>();
	camera->Initialize();

	//�ۑ����Ă���
	camera2DAndNames_.insert(std::make_pair(cameraName, std::move(camera)));
}

Camera* CameraManager::GetCamera(const std::string& cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//�w�肵�����O�̃J����������΃|�C���^��Ԃ�
	if (it != cameraAndNames_.end())
	{
		return it->second.get();
	}

	//�Ȃ����nullptr
	assert(false);
	return nullptr;
}

Camera* CameraManager::GetCamera()
{
	return usingCamera_;
}

Camera2D* CameraManager::GetCamera2D(const std::string& cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera2D>>::iterator it = camera2DAndNames_.find(cameraName);
	//�w�肵�����O�̃J����������΃|�C���^��Ԃ�
	if (it != camera2DAndNames_.end())
	{
		return it->second.get();
	}

	//�Ȃ����nullptr
	assert(false);
	return nullptr;
}

Camera2D* CameraManager::GetCamera2D()
{
	return usingCamera2D_;
}

void CameraManager::SetUsingCamera(const std::string& cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//�w�肵�����O�̃J����������΃|�C���^usingCamera_�Ƀ|�C���^�Z�b�g
	if (it != cameraAndNames_.end())
	{
		usingCamera_ = it->second.get();
		return;
	}

	//�Ȃ����
	assert(false);
}

void CameraManager::SetUsingCamera2D(const std::string& cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera2D>>::iterator it = camera2DAndNames_.find(cameraName);
	//�w�肵�����O�̃J����������΃|�C���^usingCamera_�Ƀ|�C���^�Z�b�g
	if (it != camera2DAndNames_.end())
	{
		usingCamera2D_ = it->second.get();
		return;
	}

	//�Ȃ����
	assert(false);
}
