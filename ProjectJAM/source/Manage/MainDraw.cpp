#include "MainDraw.hpp"

#include "SceneManager.hpp"

const Draw::MainDraw* Util::SingletonBase<Draw::MainDraw>::m_Singleton = nullptr;

namespace Draw {
	MainDraw::MainDraw(void) noexcept {
		this->m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		this->m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		DxLib::SetOutApplicationLogValidFlag(FALSE);
		DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
		DxLib::ChangeWindowMode(TRUE);
		DxLib::SetUseDirect3DVersion(DX_DIRECT3D_11);
		DxLib::SetGraphMode(this->m_ScreenWidth, this->m_ScreenHeight, 32);
		DxLib::SetUseDirectInputFlag(TRUE);
		DxLib::SetDirectInputMouseMode(TRUE);
		DxLib::SetWindowSizeChangeEnableFlag(TRUE, FALSE);
		DxLib::SetUsePixelLighting(TRUE);
		DxLib::SetWaitVSyncFlag(false);
		DxLib::Set3DSoundOneMetre(1.0f);									// 
		DxLib::SetMainWindowText("Title");//タイトルの文字
		//
		DxLib::DxLib_Init();
		DxLib::SetSysCommandOffFlag(TRUE);
		DxLib::SetAlwaysRunFlag(TRUE);
		this->m_BufferScreen = MakeScreen(DispWidth, DispHeight, false);
		//
		FlipSetting();
	}
	MainDraw::~MainDraw(void) noexcept {
		DeleteGraph(this->m_BufferScreen);
		this->m_BufferScreen = -1;
		DxLib::DxLib_End();
	}

	void MainDraw::FlipSetting(void) noexcept {
		SetWindowMode(EnumWindowMode::Window);
		SetWaitVSync(false);
		this->m_FPSLimit = 60;
	}

	void MainDraw::Update(void) noexcept {
		LONGLONG WaitTime = DxLib::GetNowHiPerformanceCount() - this->m_TickTime;
		LONGLONG EndFrame = 1000LL * 1000LL / static_cast<LONGLONG>(FrameRate);
		if (WaitTime > EndFrame) {
			this->m_UpdateTickCount = static_cast<int>(WaitTime / EndFrame);
			//this->m_UpdateTickCount = std::min(this->m_UpdateTickCount, 30);//3回(20FPS)未満は処理落ちさせる
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
				if (HeightT >= (WidthT * DispHeight / DispWidth)) {//  16:9より縦長
					HeightT = (WidthT * DispHeight / DispWidth);
				}
				else {// 16:9より横長
					WidthT = (HeightT * DispWidth / DispHeight);
				}
				this->m_WindowWidth = WidthT * GetDPI() / BaseDPI;
				this->m_WindowHeight = HeightT * GetDPI() / BaseDPI;
			}
		}
		if (this->m_WindowWidth != 0 && this->m_WindowHeight != 0) {
			//マウス座標取得
			DxLib::GetMousePoint(&this->m_MouseX, &this->m_MouseY);
			this->m_MouseX = (-(this->m_WindowDrawWidth - this->m_WindowWidth) / 2 + this->m_MouseX) * DispWidth / this->m_WindowWidth;
			this->m_MouseY = (-(this->m_WindowDrawHeight - this->m_WindowHeight) / 2 + this->m_MouseY) * DispHeight / this->m_WindowHeight;
		}
	}
	void MainDraw::StartDraw(void) noexcept {
		if (this->m_UpdateTickCount > 0) {
			this->m_CalcTimer = static_cast<float>(DxLib::GetNowHiPerformanceCount() - this->m_StartTime) / 1000.f / static_cast<float>(this->m_UpdateTickCount);
		}
		SetDrawScreen(this->m_BufferScreen);
		ClearDrawScreen();
	}
	void MainDraw::EndDraw(void) noexcept {
#if _DEBUG		//デバッグ表示
		//DxLib::printfDx("FPS:[%4.1f]\n", DxLib::GetFPS());
#endif
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		DxLib::ClearDrawScreen();
		{
			auto prev = DxLib::GetDrawMode();
			DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
			DrawExtendGraph(
				this->m_WindowDrawWidth / 2 - this->m_WindowWidth / 2, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2,
				this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2, this->m_WindowDrawHeight / 2 + this->m_WindowHeight / 2,
				this->m_BufferScreen,
				false);
			DxLib::SetDrawMode(prev);
		}
		DxLib::ScreenFlip();
		if (!this->m_WaitVSync) {
			this->m_DrawTimer = static_cast<float>(DxLib::GetNowHiPerformanceCount() - this->m_StartTime) / 1000.f - this->m_CalcTimer;
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
}
