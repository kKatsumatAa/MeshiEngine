#include "SpotLight.h"
#include "ImGuiManager.h"
#include <string>


void SpotLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("SpotLight" + indexStr).c_str()))
	{
		//���C�g����
		XMVECTOR lightdir_ = { 1.0f,0,0 };
		//���C�g���W�i���[���h���W�n�j
		XMFLOAT3 lightpos_ = { 0,0,0 };
		//���C�g�F
		XMFLOAT3 lightcolor_ = { 1,1,1 };
		//���C�g���������W��
		XMFLOAT3 lightAtten_ = { 1.0f,1.0f,1.0f };
		//���C�g�����p�x�i�J�n�p�x�A�I���p�x�j
		XMFLOAT2 lightFactorAngleCos_ = { 0.5f,0.2f };

		ImGui::SliderFloat3("pos", &lightdir_.m128_f32[0], -1.0f, 1.0f);
		ImGui::InputFloat3("pos", &lightpos_.x);
		ImGui::ColorEdit3("color", &lightcolor_.x);
		ImGui::SliderFloat3("atten", &lightAtten_.x, 0, 3.0f);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
