#include "MainDraw.hpp"

const MainDraw* SingletonBase<MainDraw>::m_Singleton = nullptr;

MainDraw::MainDraw(void) noexcept {
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
MainDraw::~MainDraw(void) noexcept {
	DxLib::DeleteGraph(this->m_BufferScreen);
	DxLib::DxLib_End();

	OptionParam::Release();
}

void MainDraw::Update(void) noexcept {
	LONGLONG WaitTime = DxLib::GetNowHiPerformanceCount() - this->m_TickTime;
	LONGLONG EndFrame = 1000LL * 1000LL / static_cast<LONGLONG>(this->m_CalculateTick);
	if (WaitTime > EndFrame) {
		this->m_UpdateTickCount = static_cast<int>(WaitTime / EndFrame);
		this->m_TickTime += this->m_UpdateTickCount * EndFrame;
	}
	else {
		this->m_UpdateTickCount = 0;
	}
	//
	{
		int prevX = this->m_WindowDrawWidth;
		int prevY = this->m_WindowDrawHeight;
		DxLib::GetWindowSize(&this->m_WindowDrawWidth, &this->m_WindowDrawHeight);
		if ((prevX |= this->m_WindowDrawWidth) || (prevY |= this->m_WindowDrawHeight)) {
			int WidthT = this->m_WindowDrawWidth;
			int HeightT = this->m_WindowDrawHeight;
			if (HeightT >= (WidthT * GetDispHeight() / GetDispWidth())) {// 4:3
				HeightT = (WidthT * GetDispHeight() / GetDispWidth());
			}
			else {// 16:9より横長
				WidthT = (HeightT * GetDispWidth() / GetDispHeight());
			}
			this->m_WindowWidth = WidthT * GetDPI() / BaseDPI;
			this->m_WindowHeight = HeightT * GetDPI() / BaseDPI;
		}
	}
	//マウス座標取得
	GetMousePoint(&this->m_MouseX, &this->m_MouseY);
	this->m_MouseX = (-(this->m_WindowDrawWidth - this->m_WindowWidth) / 2 + this->m_MouseX) * GetDispWidth() / this->m_WindowWidth;
	this->m_MouseY = (-(this->m_WindowDrawHeight - this->m_WindowHeight) / 2 + this->m_MouseY) * GetDispHeight() / this->m_WindowHeight;
}
void MainDraw::StartDraw(void) noexcept {
	DxLib::SetDrawScreen(this->m_BufferScreen);
	DxLib::ClearDrawScreen();
}
void MainDraw::EndDraw(void) noexcept {
#if _DEBUG
	//デバッグ表示
	DxLib::printfDx("FPS:[%4.1f]\n", GetFPS());
#endif
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
	DxLib::ClearDrawScreen();
	{
		auto prev = GetDrawMode();
		DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
		DxLib::DrawExtendGraph(
			this->m_WindowDrawWidth / 2 - this->m_WindowWidth / 2, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2,
			this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2, this->m_WindowDrawHeight / 2 + this->m_WindowHeight / 2,
			this->m_BufferScreen, FALSE);
		DxLib::SetDrawMode(prev);
	}
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