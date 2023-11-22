#include "SpotLight.h"
#include "ImGuiManager.h"
#include <string>


void SpotLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("SpotLight" + indexStr).c_str()))
	{
		ImGui::SliderFloat3("dir", &lightdir_.m128_f32[0], -1.0f, 1.0f);
		ImGui::InputFloat3("pos", &lightpos_.x);
		ImGui::ColorEdit3("color", &lightcolor_.x);
		ImGui::SliderFloat3("atten", &lightAtten_.x, 0, 3.0f);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
