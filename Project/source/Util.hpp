#pragma once
#define NOMINMAX
//namespace DxLib {
#include "DxLib.h"
//}

#include "FileStream.hpp"

// --------------------------------------------------------------------------------------------------
// シングルトン
// --------------------------------------------------------------------------------------------------
template <class T>
class SingletonBase {
private:
	static const T* m_Singleton;
public:
	static void Create(void) noexcept {
		m_Singleton = new T();
	}
	static T* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new T(); }
		return (T*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
protected:
	SingletonBase(void) noexcept {}
	virtual ~SingletonBase(void) noexcept {}
private:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
	SingletonBase(SingletonBase&&) = delete;
	SingletonBase& operator=(SingletonBase&&) = delete;
};
// 子のサンプル
/*
class A : public SingletonBase<A> {
private:
	friend class SingletonBase<A>;
}
//*/

enum class EnumOptionSelectType {
	Bool,
	Select,
	Max,
};
static const char* OptionSelectTypeStr[static_cast<int>(EnumOptionSelectType::Max)] = {
	"Bool",
	"Select",
};

class OptionParam : public SingletonBase<OptionParam> {
private:
	friend class SingletonBase<OptionParam>;
private:
	struct Param {
		std::string					m_Type{};
		EnumOptionSelectType		m_SelectType{};
		std::vector<std::string>	m_ValueList{};
		int							m_Value{};
	public:
		const std::string GetValueNow() const noexcept { return this->m_ValueList.at(this->m_Value); }
		bool IsActive() const noexcept {
			if (m_SelectType == EnumOptionSelectType::Bool) {
				return GetValueNow() == "True";
			}
			return false;
		}
	};

	std::vector<Param>	m_ParamList;
private:
	//コンストラクタ
	OptionParam(void) noexcept {
		std::ifstream file("data/ProjectSetting/Option.json");
		nlohmann::json data = nlohmann::json::parse(file);
		for (auto& d : data["data"]) {
			m_ParamList.emplace_back();
			//Type
			m_ParamList.back().m_Type = d["Type"];
			//SelectType
			std::string SelType = d["SelectType"];
			for (int loop = 0; loop < static_cast<int>(EnumOptionSelectType::Max); ++loop) {
				if (SelType == OptionSelectTypeStr[loop]) {
					m_ParamList.back().m_SelectType = static_cast<EnumOptionSelectType>(loop);
					break;
				}
			}
			//Value
			switch (m_ParamList.back().m_SelectType) {
			case EnumOptionSelectType::Bool:
				m_ParamList.back().m_ValueList.emplace_back("False");
				m_ParamList.back().m_ValueList.emplace_back("True");
				break;
			case EnumOptionSelectType::Select:
				if (d.contains("Value")) {
					for (auto& v : d["Value"]) {
						m_ParamList.back().m_ValueList.emplace_back(v);
					}
				}
				break;
			default:
				break;
			}
			//DefaultValue
			std::string DefaultValue = d["DefaultValue"];
			for (auto& v : m_ParamList.back().m_ValueList) {
				if (v == DefaultValue) {
					m_ParamList.back().m_Value = static_cast<int>(&v - &m_ParamList.back().m_ValueList.front());
					break;
				}
			}
		}
	}
	//デストラクタ
	~OptionParam(void) noexcept {
		for (auto& p : m_ParamList) {
			p.m_ValueList.clear();
		}
		m_ParamList.clear();
	}
public:
	const Param* GetParam(const char* Type) const noexcept {
		for (auto& p : m_ParamList) {
			if (p.m_Type == Type) {
				return &p;
			}
		}
		return nullptr;
	}
	void SetParam(const char* Type, int Param) noexcept {
		for (auto& p : m_ParamList) {
			if (p.m_Type == Type) {
				p.m_Value = Param;
				break;
			}
		}
	}
};

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
	int			m_WindowDrawWidth{ 960 };		//ウィンドウサイズX
	int			m_WindowDrawHeight{ 720 };		//ウィンドウサイズY
	int			m_WindowWidth{ 960 };			//ディスプレイ表示X
	int			m_WindowHeight{ 720 };			//ディスプレイ表示Y
	LONGLONG	m_TickTime = 0;
	LONGLONG	m_StartTime = 0;
	bool		m_WaitVSync = true;
	int			m_BufferScreen{ -1 };
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
		int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		DxLib::SetOutApplicationLogValidFlag(FALSE);
		DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
		DxLib::ChangeWindowMode(TRUE);
		DxLib::SetUseDirect3DVersion(DX_DIRECT3D_11);
		DxLib::SetGraphMode(ScreenWidth, ScreenHeight, 32);
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
				m_DispWidth = std::stoi(RLeft);
				m_DispHeight = std::stoi(RRight);
			}
			else if (Left == "CalculateTick") {
				m_CalculateTick = std::stoi(Right);
			}
		}
		//
		DxLib::DxLib_Init();
		DxLib::SetSysCommandOffFlag(TRUE);
		DxLib::SetAlwaysRunFlag(TRUE);
		m_BufferScreen = DxLib::MakeScreen(m_DispWidth, m_DispHeight, FALSE);
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
		m_FPSLimit = std::stoi(pOption->GetParam("FPSLimit")->GetValueNow());
	}
	//デストラクタ
	~DXLibDrawer(void) noexcept {
		DxLib::DeleteGraph(m_BufferScreen);
		DxLib::DxLib_End();

		OptionParam::Release();
	}
public:
	const auto GetWindowWidth() const noexcept { return m_WindowWidth; }
	const auto GetWindowHeight() const noexcept { return m_WindowHeight; }
public:
	const auto GetDispWidth() const noexcept { return m_DispWidth; }
	const auto GetDispHeight() const noexcept { return m_DispHeight; }
public:
	void		SetWindowMode(EnumWindowMode value) noexcept {
		int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		switch (value) {
		case EnumWindowMode::Window:
			DxLib::SetWindowStyleMode(0);
			DxLib::SetWindowPosition(-8, 0);
			DxLib::SetWindowSize(ScreenWidth * GetDPI() / BaseDPI, (ScreenHeight - 79) * GetDPI() / BaseDPI);
			DxLib::ChangeWindowMode(TRUE);
			break;
		case EnumWindowMode::BorderLess:
			DxLib::SetWindowStyleMode(4);
			DxLib::SetWindowPosition(-8, -8);
			DxLib::SetWindowSize(ScreenWidth * GetDPI() / BaseDPI, ScreenHeight * GetDPI() / BaseDPI);
			DxLib::ChangeWindowMode(TRUE);
			break;
		case EnumWindowMode::FullScreen:
			DxLib::SetWindowStyleMode(4);
			DxLib::SetWindowPosition(-8, -8);
			DxLib::SetWindowSize(ScreenWidth * GetDPI() / BaseDPI, ScreenHeight * GetDPI() / BaseDPI);
			DxLib::SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
			DxLib::SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
			DxLib::ChangeWindowMode(FALSE);
			break;
		default:
			break;
		}
	}
	void		SetWaitVSync(bool value) noexcept {
		m_WaitVSync = value;
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
		m_TickTime = DxLib::GetNowHiPerformanceCount();
		this->m_StartTime = DxLib::GetNowHiPerformanceCount();
	}
public:
	//ループ中に実施
	bool		CanContinueLoop() const noexcept {
		return DxLib::ProcessMessage() == 0;
	}
	const int	CalcUpdateTickCount() noexcept {
		LONGLONG WaitTime = DxLib::GetNowHiPerformanceCount();
		LONGLONG EndFrame = 1000LL * 1000LL / static_cast<LONGLONG>(m_CalculateTick);
		if ((WaitTime - m_TickTime) > EndFrame) {
			int Count = static_cast<int>((WaitTime - m_TickTime) / EndFrame);
			m_TickTime += Count * EndFrame;
			return Count;
		}
		return 0;
	}
	void		StartDraw() noexcept {
		{
			int prevX = m_WindowDrawWidth;
			int prevY = m_WindowDrawHeight;
			DxLib::GetWindowSize(&m_WindowDrawWidth, &m_WindowDrawHeight);
			if ((prevX |= m_WindowDrawWidth) || (prevY |= m_WindowDrawHeight)) {
				int ScreenWidthT = m_WindowDrawWidth;
				int ScreenHeightT = m_WindowDrawHeight;
				if (ScreenHeightT >= (ScreenWidthT * m_DispHeight / m_DispWidth)) {// 4:3
					ScreenHeightT = (ScreenWidthT * m_DispHeight / m_DispWidth);
				}
				else {// 16:9より横長
					ScreenWidthT = (ScreenHeightT * m_DispWidth / m_DispHeight);
				}
				m_WindowWidth = ScreenWidthT * GetDPI() / BaseDPI;
				m_WindowHeight = ScreenHeightT * GetDPI() / BaseDPI;
			}
		}
		DxLib::SetDrawScreen(m_BufferScreen);
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
			m_WindowDrawWidth / 2 - GetWindowWidth() / 2, m_WindowDrawHeight / 2 - GetWindowHeight() / 2,
			m_WindowDrawWidth / 2 + GetWindowWidth() / 2, m_WindowDrawHeight / 2 + GetWindowHeight() / 2,
			m_BufferScreen, FALSE);
		DxLib::ScreenFlip();
		if (!m_WaitVSync) {
			// 4msだけスリープ
			while ((DxLib::GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * (1000 / m_FPSLimit - 4))) {
				if (DxLib::ProcessMessage() != 0) { return; }
				DxLib::SleepThread(1);	// 1msecスリープする
			}
			while ((DxLib::GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * 1000 / m_FPSLimit)) {
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

