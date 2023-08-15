#include "ImGuiManager.h"
#include<imgui_impl_win32.h>
#include<imgui_impl_dx12.h>


void ImGuiManager::Initialize()
{
	//imgui�̃R���e�L�X�g�𐶐�
	ImGui::CreateContext();
	//imgui�̃X�^�C����ݒ�
	ImGui::StyleColorsDark();

	//win32�p������
	ImGui_ImplWin32_Init(WindowsApp::GetInstance().Gethwnd());

	//directx12�p������
	ImGui_ImplDX12_Init(
		DirectXWrapper::GetInstance().GetDevice(),
		static_cast<int>(DirectXWrapper::GetInstance().GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, TextureManager::GetDescHeapP(),
		TextureManager::GetDescHeapP()->GetCPUDescriptorHandleForHeapStart(),
		TextureManager::GetDescHeapP()->GetGPUDescriptorHandleForHeapStart()
	);

	TextureManager::AddSRVHandleCount();

	//�W���t�H���g�̒ǉ�
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Begin()
{
	//imgui�t���[���J�n
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	//�`��J�n����(�����̕\������\���f�[�^�Ƃ��Ă܂Ƃ߂�̂ŁA�`��ł͂Ȃ�)
	ImGui::Render();
}

void ImGuiManager::Draw()
{
	ID3D12GraphicsCommandList* commandlist = DirectXWrapper::GetInstance().GetCommandList();

	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetDescHeapP() };
	commandlist->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�`��R�}���h�𔭍s
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandlist);
}
