#include "DxLib.h"
#include	"BackGroundSub.hpp"

#include <algorithm>

class DX {
	int ShadowMapHandle = -1;
	VECTOR ShadowVec = VGet(0.1f, -0.3f, -0.1f);
public:
	DX() {
		DxLib::SetGraphMode(1280, 1000, 32);
		DxLib_Init();												// �c�w���C�u��������������
		ChangeWindowMode(TRUE);										// �E�B���h�E���[�h
		SetAlwaysRunFlag(TRUE);										// ��A�N�e�B�u�ł������悤�ɂ���
		SetMouseDispFlag(TRUE);
		ShadowMapHandle = MakeShadowMap(2048, 2048);				// �V���h�E�}�b�v�n���h���̍쐬
		SetLightDirection(ShadowVec);								// ���C�g�̕�����ݒ�
		SetShadowMapLightDirection(ShadowMapHandle, ShadowVec);		// �V���h�E�}�b�v���z�肷�郉�C�g�̕������Z�b�g
	}
	~DX() {
		DeleteShadowMap(ShadowMapHandle);	// �V���h�E�}�b�v�̍폜
		DxLib_End();				// �c�w���C�u�����g�p�̏I������
	}
public:
	VECTOR GetShadowVec() { return ShadowVec; }
public:
	void SetShadowMapStart(VECTOR CamPos) {
		SetShadowMapDrawArea(ShadowMapHandle, VAdd(CamPos, VGet(-50.0f, -50.0f, -50.0f)), VAdd(CamPos, VGet(50.0f, 0.0f, 50.0f)));
		ShadowMap_DrawSetup(ShadowMapHandle);		// �V���h�E�}�b�v�ւ̕`��̏���
	}
	void SetShadowMapEnd() {
		ShadowMap_DrawEnd();						// �V���h�E�}�b�v�ւ̕`����I��
	}
	void SetUseShadowMapStart() {
		SetUseShadowMap(0, ShadowMapHandle);	// �`��ɃV���h�E�}�b�v���g�p����
	}
	void SetUseShadowMapEnd() {
		SetUseShadowMap(0, -1);					// �`��Ɏg�p����V���h�E�}�b�v�̐ݒ������
	}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	BackGround::VoxelControl Voxel;
	VECTOR MyPos = VGet(0.f, 0.f, 0.f);
	VECTOR MyVec = VGet(0.f, 0.f, 0.f);
	float Xrad = -45.f * DX_PI_F / 180.f;
	float Yrad = 0.f;
	float Speed = 0.f;
	// ������
	DX DXParam;

	// �ǂݍ���
	Voxel.Load();												// ���O�ǂݍ���

	Voxel.InitStart();											// �������J�n������
	Voxel.LoadCellsFile("data/Map.txt");						// �{�N�Z���f�[�^�̓ǂݍ���
	Voxel.InitEnd();											// �������I��������
	// �����̍��W�ƌ������w��
	MyPos = VGet(0.f, 0.f, 0.f);
	// �����̍��W��n�`�ɍ��킹��
	MyPos = VGet(MyPos.x, -50.f, MyPos.z);
	Voxel.CheckLine(VGet(MyPos.x, 0.f, MyPos.z), &MyPos);

	int Lange = 1;
	int MX{}, MY{};
	int PMX{}, PMY{};
	int DMX{}, DMY{};
	// ���C�����[�v
	while (ProcessMessage() == 0) {
		PMX = MX;
		PMY = MY;
		GetMousePoint(&MX, &MY);
		//
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			float X = static_cast<float>(MY - PMY) / 100.f;
			float Y = static_cast<float>(MX - PMX) / 100.f;
			if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				X = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				Y = 0.f;
			}
			Xrad -= X;
			Yrad += Y;
		}
		if (CheckHitKey(KEY_INPUT_F1) != 0) {
			Xrad = -89.f * DX_PI_F / 180.f;
			Yrad = -90.f * DX_PI_F / 180.f;
		}
		if (CheckHitKey(KEY_INPUT_F2) != 0) {
			Xrad = -89.f * DX_PI_F / 180.f;
			Yrad = -0.f * DX_PI_F / 180.f;
		}
		if (CheckHitKey(KEY_INPUT_F3) != 0) {
			Xrad = -0.f * DX_PI_F / 180.f;
			Yrad = -0.f * DX_PI_F / 180.f;
		}

		Xrad = std::clamp(Xrad, -89.f * DX_PI_F / 180.f, 0.f * DX_PI_F / 180.f);
		MATRIX Mat = MMult(MGetRotAxis(VGet(1.f, 0.f, 0.f), Xrad), MGetRotAxis(VGet(0.f, 1.f, 0.f), Yrad));
		//
		Lange -= GetMouseWheelRotVol();
		Lange = std::clamp(Lange, 1, 17);
		//
		MyVec = VGet(0.f, 0.f, 0.f);
		if ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) {
			float X = static_cast<float>(MY - PMY) / 100.f;
			float Y = static_cast<float>(MX - PMX) / 100.f;
			if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				X = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				Y = 0.f;
			}
			MyVec = VAdd(MyVec, VTransform(VGet(Y, X, 0.f), Mat));
		}



		// �����W�𔽉f
		MyPos = VAdd(MyPos, MyVec);
		// �J�������W���w��
		VECTOR CamPos = VSub(MyPos, VScale(VTransform(VGet(0.f, 0.f, -1.f), Mat), 30.f));
		VECTOR CamTarget = MyPos;// ���g�������Ă�������𒍎��_�Ƃ���
		// FPS��\��
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		// �V���h�E�}�b�v�ɕ`�悷��͈͂�ݒ�
		// �{�N�Z������
		Voxel.SetDrawInfo(VGet(0.f, -20.f,0.f), VNorm(VSub(CamTarget, CamPos)));// �`�悷��ۂ̕`�撆�S���W�ƕ`�悷��������w��
		Voxel.SetShadowDrawInfo(VGet(0.f, -20.f, 0.f), DXParam.GetShadowVec());// �V���h�E�}�b�v�ɕ`�悷��ۂ̕`�撆�S���W�ƕ`�悷��������w��
		Voxel.Update();
		// �V���h�E�}�b�v�ւ̕`��
		DXParam.SetShadowMapStart(CamPos);
		Voxel.DrawShadow();							// �V���h�E�}�b�v�ւ̕`��
		DXParam.SetShadowMapEnd();
		// ����ʂւ̕`��
		SetDrawScreen(DX_SCREEN_BACK);				// �`���𗠉�ʂɕύX
		{
			ClearDrawScreen();											// ��ʂ��N���A
			DrawBox(0, 0, 640, 480, GetColor(150, 250, 255), TRUE);		// �w�i�F��h��

			SetCameraPositionAndTarget_UpVecY(CamPos, CamTarget);// �J�����̈ʒu�ƌ�����ݒ�
			SetCameraNearFar(0.5f, 100.0f);			// �`�悷�鉜�s�������͈̔͂�ݒ�
			SetupCamera_Ortho(2.f* static_cast<float>(Lange));

			DXParam.SetUseShadowMapStart();
			Voxel.Draw();							// �`��
			DXParam.SetUseShadowMapEnd();
		}
		ScreenFlip();								// ����ʂ̓��e��\��ʂɔ��f
	}
	Voxel.Dispose();
	Voxel.Dispose_Load();
	return 0;					// �\�t�g�̏I��
}
