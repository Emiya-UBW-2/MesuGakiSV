﻿#pragma once
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#pragma warning( pop )

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "../File/FileStream.hpp"
#include "../Util/Util.hpp"
#include "../Util/Option.hpp"

namespace Draw {
	enum class EnumWindowMode : size_t {
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
	class MainDraw : public Util::SingletonBase<MainDraw> {
	private:
		friend class Util::SingletonBase<MainDraw>;
	private:
		LONGLONG	m_TickTime = 0;
		LONGLONG	m_StartTime = 0;
		int			m_ScreenWidth{ 960 };			//スクリーンサイズX
		int			m_ScreenHeight{ 720 };			//スクリーンサイズY
		int			m_WindowDrawWidth{ 960 };		//ウィンドウサイズX
		int			m_WindowDrawHeight{ 720 };		//ウィンドウサイズY
		int			m_WindowWidth{ 960 };			//ディスプレイ表示X
		int			m_WindowHeight{ 720 };			//ディスプレイ表示Y
		int			m_BufferScreen{ -1 };
		int			m_MouseX{ -1 };
		int			m_MouseY{ -1 };
		int			m_DispWidth{ 1920 };			//UI描画などの基準X
		int			m_DispHeight{ 1080 };			//UI描画などの基準Y
		int			m_CalculateTick = 60;			//更新レート
		int			m_UpdateTickCount = 1;
		int			m_FPSLimit = 60;			//更新レート
		bool		m_WaitVSync = true;
		char		padding[7]{};
	private:
		static const int		BaseDPI = 96;
		static int GetDPI(void) noexcept {
			int DPI = BaseDPI;
			DxLib::GetMonitorDpi(NULL, &DPI);
			if (DPI == 0) {
				DPI = BaseDPI;
			}
			return DPI;
		}
	private://コンストラクタ、デストラクタ
		MainDraw(void) noexcept;
		MainDraw(const MainDraw&) = delete;
		MainDraw(MainDraw&&) = delete;
		MainDraw& operator=(const MainDraw&) = delete;
		MainDraw& operator=(MainDraw&&) = delete;
		~MainDraw(void) noexcept;
	public://Getter
		auto	GetDispWidth(void) const noexcept { return this->m_DispWidth; }//描画範囲のX座標
		auto	GetDispHeight(void) const noexcept { return this->m_DispHeight; }//描画範囲のY座標
		auto	GetMousePositionX(void) const noexcept { return this->m_MouseX; }//マウスのX座標
		auto	GetMousePositionY(void) const noexcept { return this->m_MouseY; }//マウスのY座標
		auto	GetUpdateTickCount(void) const noexcept { return this->m_UpdateTickCount; }//この周回では何回アップデートできるかをチェックする
	public://Setter
		void		SetWindowMode(EnumWindowMode value) const noexcept {
			switch (value) {
			case EnumWindowMode::Window:
				DxLib::SetWindowStyleMode(0);
				DxLib::SetWindowPosition(-8, 0);
				DxLib::SetWindowSize(this->m_ScreenWidth * GetDPI() / BaseDPI, (this->m_ScreenHeight - 79) * GetDPI() / BaseDPI);
				DxLib::ChangeWindowMode(TRUE);
				break;
			case EnumWindowMode::BorderLess:
				DxLib::SetWindowStyleMode(4);
				DxLib::SetWindowPosition(-8, -8);
				DxLib::SetWindowSize(this->m_ScreenWidth * GetDPI() / BaseDPI, this->m_ScreenHeight * GetDPI() / BaseDPI);
				DxLib::ChangeWindowMode(TRUE);
				break;
			case EnumWindowMode::FullScreen:
				DxLib::SetWindowStyleMode(4);
				DxLib::SetWindowPosition(-8, -8);
				DxLib::SetWindowSize(this->m_ScreenWidth * GetDPI() / BaseDPI, this->m_ScreenHeight * GetDPI() / BaseDPI);
				DxLib::SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
				DxLib::SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
				DxLib::ChangeWindowMode(FALSE);
				break;
			case EnumWindowMode::Max:
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

		void		FlipSetting(void) noexcept;
	public:
		//ループ前に実施
		void		LoopStart(void) noexcept {
			this->m_TickTime = DxLib::GetNowHiPerformanceCount();
			this->m_StartTime = DxLib::GetNowHiPerformanceCount();
		}
	public:
		//ループ中に実施
		bool		CanContinueLoop(void) const noexcept {
			return DxLib::ProcessMessage() == 0;
		}
		void		Update(void) noexcept;
		void		StartDraw(void) const noexcept;
		void		EndDraw(void) noexcept;
	};
}
