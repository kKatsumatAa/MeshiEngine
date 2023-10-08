#include "SpotLight.h"
#include "ImGuiManager.h"
#include <string>


void SpotLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("SpotLight" + indexStr).c_str()))
	{
		//ライト方向
		XMFLOAT3 lightdir = { 1.0f,0,0 };
		//ライト座標（ワールド座標系）
		XMFLOAT3 lightpos = { 0,0,0 };
		//ライト色
		XMFLOAT3 lightcolor = { 1,1,1 };
		//ライト距離減衰係数
		XMFLOAT3 lightAtten = { 1.0f,1.0f,1.0f };
		//ライト減衰角度（開始角度、終了角度）
		XMFLOAT2 lightFactorAngleCos = { 0.5f,0.2f };

		ImGui::SliderFloat3("pos", &lightdir.x, -1.0f, 1.0f);
		ImGui::InputFloat3("pos", &lightpos.x);
		ImGui::ColorEdit3("color", &lightcolor.x);
		ImGui::SliderFloat3("atten", &lightAtten.x, 0, 3.0f);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
