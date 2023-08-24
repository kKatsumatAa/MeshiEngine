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
		ImGui::SliderFloat3("atten", &lightatten_.x, 0, 3.0f);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
