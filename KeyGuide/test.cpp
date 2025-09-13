#define NOMINMAX

#include "DxLib.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <fstream>

static const int draw_x = 960;			//�E�B���h�E�T�C�YX
static const int draw_y = 720;			//�E�B���h�E�T�C�YY

//���낢��܂Ƃ߂�N���X
class DXDraw {
public:
	//�R���X�g���N�^
	DXDraw(void) noexcept {
		SetOutApplicationLogValidFlag(FALSE);           /*log*/
		SetMainWindowText("game title");                /*�^�C�g��*/
		ChangeWindowMode(TRUE);                         /*���\��*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);          /*directX ver*/
		SetGraphMode(draw_x, draw_y, 32);               /*�𑜓x*/
		SetUseDirectInputFlag(TRUE);                    /*DirectInput�g�p*/
		SetDirectInputMouseMode(TRUE);                  /*DirectInput�}�E�X�g�p*/
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);    /*�E�C���h�E�T�C�Y���蓮�ύX�s�A�E�C���h�E�T�C�Y�ɍ��킹�Ċg������Ȃ��悤�ɂ���*/
		SetUsePixelLighting(TRUE);                      /*�s�N�Z�����C�e�B���O�̎g�p*/
		SetFullSceneAntiAliasingMode(4, 2);             /*�A���`�G�C���A�X*/
		SetWaitVSyncFlag(TRUE);                         /*��������*/
		DxLib_Init();                                   /*�c�w���C�u��������������*/
		SetSysCommandOffFlag(TRUE);                     /*�^�X�N�X�C�b�`��L���ɂ��邩�ǂ�����ݒ肷��*/
		SetAlwaysRunFlag(TRUE);                         /*background*/
	}
	//�f�X�g���N�^
	~DXDraw(void) noexcept {
		DxLib_End();
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto maincontrol = std::make_unique<DXDraw>();
	//���C�����[�v�J�n
	while ((ProcessMessage() == 0) && (CheckHitKey(KEY_INPUT_ESCAPE) == 0)) {
		//�`��
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			//�f�o�b�O�\��
			clsDx();
			printfDx("FPS:%4.1f\n", GetFPS());

		}
		ScreenFlip();
	}
	return 0;// �\�t�g�̏I�� 
}
