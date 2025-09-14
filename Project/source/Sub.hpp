#pragma once
#define NOMINMAX
#include "DxLib.h"

#include "FileStream.hpp"
#include "json.hpp"
#include "Util.hpp"
#include "Option.hpp"

enum class EnumWindowMode {
	Window,
	BorderLess,
	FullScreen,
	Max
};
static const char* WindowModeStr[static_cast<int>(EnumWindowMode::Max)] = {
	"Window",
	"BorderLess",
	"FullScreen",
};

//DXLib描画クラス
class DXLibDrawer : public SingletonBase<DXLibDrawer> {
private:
	friend class SingletonBase<DXLibDrawer>;
private:
	int			m_ScreenWidth{ 960 };			//スクリーンサイズX
	int			m_ScreenHeight{ 720 };			//スクリーンサイズY
	int			m_WindowDrawWidth{ 960 };		//ウィンドウサイズX
	int			m_WindowDrawHeight{ 720 };		//ウィンドウサイズY
	int			m_WindowWidth{ 960 };			//ディスプレイ表示X
	int			m_WindowHeight{ 720 };			//ディスプレイ表示Y
	LONGLONG	m_TickTime = 0;
	LONGLONG	m_StartTime = 0;
	bool		m_WaitVSync = true;
	int			m_BufferScreen{ -1 };
	int			m_MouseX{ -1 };
	int			m_MouseY{ -1 };
private:
	int			m_DispWidth{ 1920 };			//UI描画などの基準X
	int			m_DispHeight{ 1080 };			//UI描画などの基準Y
	int			m_CalculateTick = 60;			//更新レート

	int			m_FPSLimit = 60;			//更新レート
private:
	static const int		BaseDPI = 96;
	static const int GetDPI() noexcept {
		int DPI = BaseDPI;
		DxLib::GetMonitorDpi(NULL, &DPI);
		if (DPI == 0) {
			DPI = BaseDPI;
		}
		return DPI;
	}
private:
	//コンストラクタ
	DXLibDrawer(void) noexcept {
		OptionParam::Create();
		//
		m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		DxLib::SetOutApplicationLogValidFlag(FALSE);
		DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
		DxLib::ChangeWindowMode(TRUE);
		DxLib::SetUseDirect3DVersion(DX_DIRECT3D_11);
		DxLib::SetGraphMode(m_ScreenWidth, m_ScreenHeight, 32);
		DxLib::SetUseDirectInputFlag(TRUE);
		DxLib::SetDirectInputMouseMode(TRUE);
		DxLib::SetWindowSizeChangeEnableFlag(TRUE, FALSE);
		DxLib::SetUsePixelLighting(TRUE);
		DxLib::SetWaitVSyncFlag(false);
		//プロジェクト固有設定
		InputFileStream Istream("data/ProjectSetting/ProjectSetting.dat");
		while (!Istream.ComeEof()) {
			std::string Line = InputFileStream::getleft(Istream.SeekLineAndGetStr(), "//");
			std::string Left = InputFileStream::getleft(Line, "=");
			std::string Right = InputFileStream::getright(Line, "=");
			if (Left == "GameTitle") {
				DxLib::SetMainWindowText(Right.c_str());
			}
			else if (Left == "BaseDispSize") {
				std::string RLeft = InputFileStream::getleft(Right, ",");
				std::string RRight = InputFileStream::getright(Right, ",");
				this->m_DispWidth = std::stoi(RLeft);
				this->m_DispHeight = std::stoi(RRight);
			}
			else if (Left == "CalculateTick") {
				this->m_CalculateTick = std::stoi(Right);
			}
		}
		//
		DxLib::DxLib_Init();
		DxLib::SetSysCommandOffFlag(TRUE);
		DxLib::SetAlwaysRunFlag(TRUE);
		this->m_BufferScreen = DxLib::MakeScreen(this->m_DispWidth, this->m_DispHeight, FALSE);
		//
		auto* pOption = OptionParam::Instance();
		{
			const auto& ModeStr = pOption->GetParam("WindowMode")->GetValueNow();
			for (int loop = 0; loop < static_cast<int>(EnumWindowMode::Max); ++loop) {
				if (ModeStr == WindowModeStr[loop]) {
					SetWindowMode(static_cast<EnumWindowMode>(loop));
					break;
				}
			}
		}
		SetWaitVSync(pOption->GetParam("VSync")->IsActive());
		this->m_FPSLimit = std::stoi(pOption->GetParam("FPSLimit")->GetValueNow());
	}
	//デストラクタ
	~DXLibDrawer(void) noexcept {
		DxLib::DeleteGraph(this->m_BufferScreen);
		DxLib::DxLib_End();

		OptionParam::Release();
	}
public:
	const auto GetWindowWidth() const noexcept { return this->m_WindowWidth; }
	const auto GetWindowHeight() const noexcept { return this->m_WindowHeight; }
public:
	const auto GetDispWidth() const noexcept { return this->m_DispWidth; }
	const auto GetDispHeight() const noexcept { return this->m_DispHeight; }

	const auto GetMousePositionX() const noexcept { return this->m_MouseX; }
	const auto GetMousePositionY() const noexcept { return this->m_MouseY; }
public:
	void		SetWindowMode(EnumWindowMode value) noexcept {
		switch (value) {
		case EnumWindowMode::Window:
			DxLib::SetWindowStyleMode(0);
			DxLib::SetWindowPosition(-8, 0);
			DxLib::SetWindowSize(m_ScreenWidth * GetDPI() / BaseDPI, (m_ScreenHeight - 79) * GetDPI() / BaseDPI);
			DxLib::ChangeWindowMode(TRUE);
			break;
		case EnumWindowMode::BorderLess:
			DxLib::SetWindowStyleMode(4);
			DxLib::SetWindowPosition(-8, -8);
			DxLib::SetWindowSize(m_ScreenWidth * GetDPI() / BaseDPI, m_ScreenHeight * GetDPI() / BaseDPI);
			DxLib::ChangeWindowMode(TRUE);
			break;
		case EnumWindowMode::FullScreen:
			DxLib::SetWindowStyleMode(4);
			DxLib::SetWindowPosition(-8, -8);
			DxLib::SetWindowSize(m_ScreenWidth * GetDPI() / BaseDPI, m_ScreenHeight * GetDPI() / BaseDPI);
			DxLib::SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
			DxLib::SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
			DxLib::ChangeWindowMode(FALSE);
			break;
		default:
			break;
		}
	}
	void		SetWaitVSync(bool value) noexcept {
		this->m_WaitVSync = value;
		if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) {
			DxLib::SetWaitVSyncFlag(FALSE);//DirectX9では後からの変更が効かない
		}
		else {
			DxLib::SetWaitVSyncFlag(value);//DirectX11ではあとからの変更が効く
		}
	}
public:
	//ループ前に実施
	void		LoopStart() noexcept {
		this->m_TickTime = DxLib::GetNowHiPerformanceCount();
		this->m_StartTime = DxLib::GetNowHiPerformanceCount();
	}
public:
	//ループ中に実施
	bool		CanContinueLoop() const noexcept {
		return DxLib::ProcessMessage() == 0;
	}
	const int	CalcUpdateTickCount() noexcept {
		LONGLONG WaitTime = DxLib::GetNowHiPerformanceCount() - this->m_TickTime;
		LONGLONG EndFrame = 1000LL * 1000LL / static_cast<LONGLONG>(this->m_CalculateTick);
		if (WaitTime > EndFrame) {
			int Count = static_cast<int>(WaitTime / EndFrame);
			this->m_TickTime += Count * EndFrame;
			return Count;
		}
		return 0;
	}
	void		Update() noexcept {
		{
			int prevX = this->m_WindowDrawWidth;
			int prevY = this->m_WindowDrawHeight;
			DxLib::GetWindowSize(&this->m_WindowDrawWidth, &this->m_WindowDrawHeight);
			if ((prevX |= this->m_WindowDrawWidth) || (prevY |= this->m_WindowDrawHeight)) {
				int WidthT = this->m_WindowDrawWidth;
				int HeightT = this->m_WindowDrawHeight;
				if (HeightT >= (WidthT * this->m_DispHeight / this->m_DispWidth)) {// 4:3
					HeightT = (WidthT * this->m_DispHeight / this->m_DispWidth);
				}
				else {// 16:9より横長
					WidthT = (HeightT * this->m_DispWidth / this->m_DispHeight);
				}
				this->m_WindowWidth = WidthT * GetDPI() / BaseDPI;
				this->m_WindowHeight = HeightT * GetDPI() / BaseDPI;
			}
		}
		GetMousePoint(&this->m_MouseX, &this->m_MouseY);
		this->m_MouseX = (-(this->m_WindowDrawWidth - this->m_WindowWidth) / 2 + this->m_MouseX) * this->m_DispWidth / this->m_WindowWidth;
		this->m_MouseY = (-(this->m_WindowDrawHeight - this->m_WindowHeight) / 2 + this->m_MouseY) * this->m_DispHeight / this->m_WindowHeight;
	}
	void		StartDraw() noexcept {
		DxLib::SetDrawScreen(this->m_BufferScreen);
		DxLib::ClearDrawScreen();
	}
	void		EndDraw() noexcept {
#if _DEBUG
		//デバッグ表示
		DxLib::printfDx("FPS:[%4.1f]\n", GetFPS());
#endif
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		DxLib::ClearDrawScreen();
		DxLib::DrawExtendGraph(
			this->m_WindowDrawWidth / 2 - GetWindowWidth() / 2, this->m_WindowDrawHeight / 2 - GetWindowHeight() / 2,
			this->m_WindowDrawWidth / 2 + GetWindowWidth() / 2, this->m_WindowDrawHeight / 2 + GetWindowHeight() / 2,
			this->m_BufferScreen, FALSE);
		DxLib::ScreenFlip();
		if (!this->m_WaitVSync) {
			// 4msだけスリープ
			while ((DxLib::GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * (1000 / this->m_FPSLimit - 4))) {
				if (DxLib::ProcessMessage() != 0) { return; }
				DxLib::SleepThread(1);	// 1msecスリープする
			}
			while ((DxLib::GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * 1000 / this->m_FPSLimit)) {
			}
		}
		else {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				DxLib::WaitVSync(1);
			}
		}
#if _DEBUG
		DxLib::clsDx();
#endif
		this->m_StartTime = DxLib::GetNowHiPerformanceCount();
	}
};

