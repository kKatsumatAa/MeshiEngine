#include "PointLight.h"
#include "ImGuiManager.h"
#include <string>


void PointLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("PointLight" + indexStr).c_str()))
	{
		ImGui::InputFloat3("pos", &lightpos_.x);
		ImGui::ColorEdit3("color", &lightcolor_.x);
		ImGui::SliderFloat3("atten", &lightatten_.x, IMGUI_LIGHT_ATTEN_MIN_, IMGUI_LIGHT_ATTEN_MAX_);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
