#include "DirLight.h"
#include "ImGuiManager.h"
#include <string>


void DirLight::DrawImGui(int32_t index)
{
	std::string indexStr = std::to_string(index);

	if (ImGui::TreeNode(("DirLight" + indexStr).c_str()))
	{
		ImGui::SliderFloat3("dir", lightdir_.m128_f32, -1.0f, 1.0f);
		ImGui::ColorEdit3("color", &lightColor_.x);
		ImGui::Checkbox("active", &active_);

		ImGui::TreePop();
	}
}
