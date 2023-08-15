#include "ImGuiManager.h"
#include<imgui_impl_win32.h>
#include<imgui_impl_dx12.h>


void ImGuiManager::Initialize()
{
	//imguiのコンテキストを生成
	ImGui::CreateContext();
	//imguiのスタイルを設定
	ImGui::StyleColorsDark();

	//win32用初期化
	ImGui_ImplWin32_Init(WindowsApp::GetInstance().Gethwnd());

	//directx12用初期化
	ImGui_ImplDX12_Init(
		DirectXWrapper::GetInstance().GetDevice(),
		static_cast<int>(DirectXWrapper::GetInstance().GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, TextureManager::GetDescHeapP(),
		TextureManager::GetDescHeapP()->GetCPUDescriptorHandleForHeapStart(),
		TextureManager::GetDescHeapP()->GetGPUDescriptorHandleForHeapStart()
	);

	TextureManager::AddSRVHandleCount();

	//標準フォントの追加
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
	//imguiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	//描画開始準備(内部の表示物を表示データとしてまとめるので、描画ではない)
	ImGui::Render();
}

void ImGuiManager::Draw()
{
	ID3D12GraphicsCommandList* commandlist = DirectXWrapper::GetInstance().GetCommandList();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::GetDescHeapP() };
	commandlist->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandlist);
}
