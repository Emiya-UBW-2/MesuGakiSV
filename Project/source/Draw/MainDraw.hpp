#pragma once
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

//DXLib�`��N���X
class MainDraw : public SingletonBase<MainDraw> {
private:
	friend class SingletonBase<MainDraw>;
private:
	LONGLONG	m_TickTime = 0;
	LONGLONG	m_StartTime = 0;
	int			m_ScreenWidth{ 960 };			//�X�N���[���T�C�YX
	int			m_ScreenHeight{ 720 };			//�X�N���[���T�C�YY
	int			m_WindowDrawWidth{ 960 };		//�E�B���h�E�T�C�YX
	int			m_WindowDrawHeight{ 720 };		//�E�B���h�E�T�C�YY
	int			m_WindowWidth{ 960 };			//�f�B�X�v���C�\��X
	int			m_WindowHeight{ 720 };			//�f�B�X�v���C�\��Y
	int			m_BufferScreen{ -1 };
	int			m_MouseX{ -1 };
	int			m_MouseY{ -1 };
	int			m_DispWidth{ 1920 };			//UI�`��Ȃǂ̊X
	int			m_DispHeight{ 1080 };			//UI�`��Ȃǂ̊Y
	int			m_CalculateTick = 60;			//�X�V���[�g
	int			m_UpdateTickCount = 1;
	int			m_FPSLimit = 60;			//�X�V���[�g
	bool		m_WaitVSync = true;
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
private://�R���X�g���N�^�A�f�X�g���N�^
	MainDraw(void) noexcept;
	MainDraw(const MainDraw&) = delete;
	MainDraw(MainDraw&&) = delete;
	MainDraw& operator=(const MainDraw&) = delete;
	MainDraw& operator=(MainDraw&&) = delete;
	~MainDraw(void) noexcept;
public://Getter
	auto	GetDispWidth(void) const noexcept { return this->m_DispWidth; }//�`��͈͂�X���W
	auto	GetDispHeight(void) const noexcept { return this->m_DispHeight; }//�`��͈͂�Y���W
	auto	GetMousePositionX(void) const noexcept { return this->m_MouseX; }//�}�E�X��X���W
	auto	GetMousePositionY(void) const noexcept { return this->m_MouseY; }//�}�E�X��Y���W
	auto	GetUpdateTickCount(void) const noexcept { return this->m_UpdateTickCount; }//���̎���ł͉���A�b�v�f�[�g�ł��邩���`�F�b�N����
public://Setter
	void		SetWindowMode(EnumWindowMode value) const noexcept {
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
		case EnumWindowMode::Max:
			break;
		default:
			break;
		}
	}
	void		SetWaitVSync(bool value) noexcept {
		this->m_WaitVSync = value;
		if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) {
			DxLib::SetWaitVSyncFlag(FALSE);//DirectX9�ł͌ォ��̕ύX�������Ȃ�
		}
		else {
			DxLib::SetWaitVSyncFlag(value);//DirectX11�ł͂��Ƃ���̕ύX������
		}
	}
public:
	//���[�v�O�Ɏ��{
	void		LoopStart(void) noexcept {
		this->m_TickTime = DxLib::GetNowHiPerformanceCount();
		this->m_StartTime = DxLib::GetNowHiPerformanceCount();
	}
public:
	//���[�v���Ɏ��{
	bool		CanContinueLoop(void) const noexcept {
		return DxLib::ProcessMessage() == 0;
	}
	void		Update(void) noexcept;
	void		StartDraw(void) const noexcept;
	void		EndDraw(void) noexcept;
};

