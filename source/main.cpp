#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>


#include "kmpLoader.h"

#include <DxLib.h>

#define bairitu (1.0)
#define WinWidth (1039*bairitu) //1366  //解像度でもある
#define WinHeight (345*bairitu)
#define _GetPath(str) (dirpath +TEXT(str))
std::wstring dirpath;

HHOOK khook;
KMP kmp;
int DSendCnt = 0;
constexpr int NumOfChangeQwertyBySpaceRenda = 5;
int SpaceCnt = 0;
std::pair<KMP::Dir, std::vector<KeyCode>>Sendq;
bool qwertyMode = false;
bool isyouon = false;
bool issokuon = false;//促音

bool isShifted = false;
KMP::Dir previousDir = KMP::Dir::None;

#include "wndp.h"


HRESULT SetHook(HINSTANCE hIns, HHOOK& hook) {
	hook = NULL;
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)MyHookProc, hIns, 0);

	if (hook == NULL)return E_FAIL;
	return S_OK;
}

void EndHook(HHOOK& hook) {
	UnhookWindowsHookEx(hook);
}

std::wstring GetNowCurrentDirectoryW() {
	wchar_t _path[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	if (::GetModuleFileNameW(NULL, _path, MAX_PATH) != 0)
	{
		::_wsplitpath_s(_path, drive, dir, fname, ext);
		return std::wstring(drive) + std::wstring(dir);
	}
	return L"";
}

HWND MakeWindow(HINSTANCE hInstance, int nWinWidth, int nWinHeight, LPCWSTR lpWinTitle, bool isFullScreen, WNDPROC wp) {
	constexpr LPCWSTR windowClsName = TEXT("DirectX11Engine");
	HWND rhWnd;
	WNDCLASS winc;
	winc.style = CS_HREDRAW | CS_VREDRAW;	       //ウィンドウの基本スタイル
	winc.lpfnWndProc = wp;                         //ウィンドウプロシージャ
	winc.cbClsExtra = 0;                           //クラス構造体追加領域予約
	winc.cbWndExtra = 0;                           //ウィンドウ構造体追加領域予約
	winc.hInstance = hInstance;                    //インスタンスハンドル
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  //アイコン
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);    //マウスカーソル
	winc.hbrBackground = NULL;                     //背景色
	winc.lpszMenuName = NULL;                      //クラスメニュー
	winc.lpszClassName = windowClsName;            //クラス名

	if (!RegisterClass(&winc))return NULL;

	rhWnd = CreateWindow(
		windowClsName, lpWinTitle,
		(isFullScreen ? WS_POPUP : WS_OVERLAPPEDWINDOW) | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nWinWidth, nWinHeight+60,
		NULL, NULL,
		hInstance, NULL);

	return rhWnd;
}
int DrawExtendGrapht(int x, int y, int grHandle, int TransFlag,double _bairitu=bairitu) {
	int width = 0;
	int height = 0;
	GetGraphSize(grHandle, &width, &height);
	return DrawExtendGraph(x, y, x + width, y + height, grHandle, TransFlag);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	HWND hwnd;
	MSG msg;

	hwnd = MakeWindow(hInstance, WinWidth, WinHeight, TEXT("Q-ELF入力補助キーボード"), false, WndProc);

	dirpath = GetNowCurrentDirectoryW();
	//G.Init(hwnd, WinWidth, WinHeight);
	SetUserWindow(hwnd);
	if (DxLib_Init() < 0)return -1;

	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	
	int tekeyboardnochange = 0;
	int tekeyboardq_elfconst = 0;
	int tekeyboardboon = 0;
	int tekeyboardshiin = 0;
	int tekeyboardyouon = 0;
	int tekeyboardsokuon = 0;
	int tekeyboardqwerty = 0;
	int tekeyboarddakukiyo = 0;
	int tekeyboardkiyodaku = 0;
	int tekeyboardSpaceCounter[6] = { 0,0,0,0,0,0 };

	tekeyboardnochange = LoadGraph(_GetPath("kh\\keyboardnochange.png").c_str());
	tekeyboardq_elfconst = LoadGraph(_GetPath("kh\\keyboardq-elfconstpng.png").c_str());
	tekeyboardboon = LoadGraph(_GetPath("kh\\keyboardboon.png").c_str());
	tekeyboardshiin = LoadGraph(_GetPath("kh\\keyboardshiin.png").c_str());
	tekeyboardyouon = LoadGraph(_GetPath("kh\\keyboardyouon.png").c_str());
	tekeyboardsokuon = LoadGraph(_GetPath("kh\\keyboardsokuon.png").c_str());
	tekeyboardqwerty = LoadGraph(_GetPath("kh\\keyboardqwerty.png").c_str());
	tekeyboarddakukiyo = LoadGraph(_GetPath("kh\\keyboarddakukiyo.png").c_str());
	tekeyboardkiyodaku = LoadGraph(_GetPath("kh\\keyboardkiyodaku.png").c_str());

	tekeyboardSpaceCounter[1] = LoadGraph(_GetPath("kh\\keyboardspacecounter1.png").c_str());
	tekeyboardSpaceCounter[2] = LoadGraph(_GetPath("kh\\keyboardspacecounter2.png").c_str());
	tekeyboardSpaceCounter[3] = LoadGraph(_GetPath("kh\\keyboardspacecounter3.png").c_str());
	tekeyboardSpaceCounter[4] = LoadGraph(_GetPath("kh\\keyboardspacecounter4.png").c_str());
	tekeyboardSpaceCounter[5] = LoadGraph(_GetPath("kh\\keyboardspacecounter5.png").c_str());


	Loadkmp(R"(q-,lfc.kmp)", kmp);

	if (FAILED(SetHook(hInstance, khook))) {
		return 0;
	}

	SetDrawScreen(DX_SCREEN_BACK);


	while (TRUE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {

			ClearDrawScreen();
			DrawExtendGrapht(0, 0, tekeyboardnochange, FALSE);
			if (qwertyMode || (GetAsyncKeyState(VK_CONTROL) & 0xf0000000)) {
				DrawExtendGrapht(0, 0, tekeyboardqwerty, TRUE);
			}
			else {
				DrawExtendGrapht(0, 0, tekeyboardq_elfconst, TRUE);

				if (DSendCnt == 0) {
					DrawExtendGrapht(0, 0, tekeyboardshiin, TRUE);
				}
				else {
					DrawExtendGrapht(0, 0, tekeyboardboon, TRUE);
					if (previousDir == KMP::Dir::L) {
						DrawExtendGrapht(0, 0, tekeyboarddakukiyo, TRUE);
					}
					else if (previousDir == KMP::Dir::R) {
						DrawExtendGrapht(0, 0, tekeyboardkiyodaku, TRUE);
					}

				}

				if (isyouon) {
					DrawExtendGrapht(0, 0, tekeyboardyouon, TRUE);
				}
				if (issokuon) {
					DrawExtendGrapht(0, 0, tekeyboardsokuon, TRUE);
				}

				if (SpaceCnt > 0 && SpaceCnt <= 5) {
					DrawExtendGrapht(0, 0, tekeyboardSpaceCounter[SpaceCnt], TRUE);
				}
			}
			ScreenFlip();
		}
	}

	EndHook(khook);
	DxLib_End();
	return msg.wParam;
}