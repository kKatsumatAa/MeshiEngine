#include "DirLight.h"
#include "ImGuiManager.h"
#include <string>


void DirLight::SetLightDir(const XMVECTOR& lightdir)
{
	XMVECTOR dir = DirectX::XMVector3Normalize(lightdir);

	lightdir_ = { dir.m128_f32[0],dir.m128_f32[1],dir.m128_f32[2] };
}

DirLight::XMVECTOR DirLight::GetLightDir()
{
	XMVECTOR vec = { lightdir_.x,lightdir_.y,lightdir_.z };

	return vec;
}

void DirLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("DirLight" + indexStr).c_str()))
	{
		ImGui::SliderFloat3("dir", &lightdir_.x, IMGUI_LIGHT_DIR_MIN_, IMGUI_LIGHT_DIR_MAX_);
		ImGui::ColorEdit3("color", &lightColor_.x);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
