#pragma once
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_freetype.h"
#include "../Framework.h"
#include <dwmapi.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class Overlay
{
private:
	// オーバーレイに登録する情報
	WNDCLASSEXA wc{};
	HWND m_hWnd;
	char m_szTitle[32] = "The Overlay";
	char m_szClass[32] = "WND_CLS";

	// ターゲットウィンドウに関する情報
	char m_TargetTitle[128]{};
	RECT m_GameRect{};
	POINT m_GamePoint{};

	// オーバーレイのスタイル
	LONG DefaultStyle = WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

	bool CreateOverlay();
	HWND GetTargetWindow(const std::string processName);	// 実行ファイル名からウィンドウハンドルを取得する
public:
	bool InitOverlay(const char* targetName, InitializeMode InitMode);
	void OverlayUserFunction();
	void OverlayLoop();
	void DestroyOverlay();
	void OverlayManager();
};