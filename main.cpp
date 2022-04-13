#include <Windows.h>

//Windowsアプリでのエントリーポイント(winmainがゲームそのもの)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//コンソールへの文字出力
	OutputDebugStringA("Hello,DirectX!!\n");

	return 0;
}