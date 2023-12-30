#include "SpotLight.h"
#include "ImGuiManager.h"
#include <string>


void SpotLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("SpotLight" + indexStr).c_str()))
	{
		ImGui::SliderFloat3("dir", &lightdir_.m128_f32[0], IMGUI_LIGHT_DIR_MIN_, IMGUI_LIGHT_DIR_MAX_);
		ImGui::InputFloat3("pos", &lightpos_.x);
		ImGui::ColorEdit3("color", &lightcolor_.x);
		ImGui::SliderFloat3("atten", &lightAtten_.x, IMGUI_LIGHT_ATTEN_MIN_, IMGUI_LIGHT_ATTEN_MAX_);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
