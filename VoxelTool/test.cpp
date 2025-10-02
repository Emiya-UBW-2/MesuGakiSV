#include "DxLib.h"

#include	"BackGroundSub.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	int ShadowMapHandle = -1;
	BackGround::VoxelControl Voxel;
	VECTOR ShadowVec = VGet(0.1f, -0.8f, -0.1f);
	VECTOR MyPos = VGet(0.f, 0.f, 0.f);
	VECTOR MyDir = VGet(0.f, 0.f, -1.f);
	VECTOR MyVec = VGet(0.f, 0.f, 0.f);
	float Yrad = 0.f;
	float Speed = 0.f;
	// ������
	DxLib_Init();												// �c�w���C�u��������������
	ChangeWindowMode(TRUE);										// �E�B���h�E���[�h
	SetAlwaysRunFlag(TRUE);										// ��A�N�e�B�u�ł������悤�ɂ���
	// �ǂݍ���
	Voxel.Load();												// ���O�ǂݍ���
	ShadowMapHandle = MakeShadowMap(2048, 2048);				// �V���h�E�}�b�v�n���h���̍쐬
	// ������
	SetLightDirection(ShadowVec);								// ���C�g�̕�����ݒ�
	SetShadowMapLightDirection(ShadowMapHandle, ShadowVec);		// �V���h�E�}�b�v���z�肷�郉�C�g�̕������Z�b�g

	Voxel.InitStart();											// �������J�n������
	Voxel.LoadCellsFile("data/Map.txt");						// �{�N�Z���f�[�^�̓ǂݍ���
	Voxel.InitEnd();											// �������I��������
	// �����̍��W�ƌ������w��
	MyPos = VGet(0.f, 0.f, 0.f);
	MyDir = VGet(0.f, 0.f, -1.f);
	// �����̍��W��n�`�ɍ��킹��
	MyPos = VGet(MyPos.x, -50.f, MyPos.z);
	Voxel.CheckLine(VGet(MyPos.x, 0.f, MyPos.z), &MyPos);
	// ���C�����[�v
	while (ProcessMessage() == 0) {
		// ���E��]
		if (CheckHitKey(KEY_INPUT_A) != 0) {
			Yrad -= 1.f / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_D) != 0) {
			Yrad += 1.f / GetFPS();
		}
		// �i�s�����ɑO�i���
		if (CheckHitKey(KEY_INPUT_W) != 0) {
			Speed = 1.f / GetFPS();
		}
		else if (CheckHitKey(KEY_INPUT_S) != 0) {
			Speed = -1.f / GetFPS();
		}
		else {
			Speed = 0.f;
		}
		MyDir = VTransform(VGet(0.f, 0.f, -1.f), MGetRotAxis(VGet(0.f, 1.f, 0.f), Yrad));
		MyVec = VScale(MyDir, Speed);

		// �ړ��x�N�g�������Z���������W���쐬
		VECTOR PosBuffer = VAdd(MyPos, MyVec);
		// �ǔ���
		std::vector<int> addonColObj;
		Voxel.CheckWall(MyPos, &PosBuffer, VGet(0.f, 0.7f, 0.f), VGet(0.f, 1.6f, 0.f), 0.7f, addonColObj);// ���ݒn���牼���W�ɐi�񂾏ꍇ
		// �n�ʔ���
		PosBuffer = VGet(PosBuffer.x, PosBuffer.y - 0.1f, PosBuffer.z);
		if (!Voxel.CheckLine(VGet(PosBuffer.x, PosBuffer.y + 1.f, PosBuffer.z), &PosBuffer)) {
			// �q�b�g���Ă��Ȃ��ۂ͗���������(�����x�͖���)
			PosBuffer.y -= 9.8f / (GetFPS() * GetFPS());
		}
		// �����W�𔽉f
		MyPos = PosBuffer;
		// �J�������W���w��
		VECTOR CamPos = MyPos; CamPos.y += 1.f;// �����̍��W��1m�������
		VECTOR CamTarget = VAdd(CamPos, MyDir);// ���g�������Ă�������𒍎��_�Ƃ���
		// FPS��\��
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		// �V���h�E�}�b�v�ɕ`�悷��͈͂�ݒ�
		SetShadowMapDrawArea(ShadowMapHandle, VAdd(CamPos, VGet(-10.0f, -10.0f, -10.0f)), VAdd(CamPos, VGet(10.0f, 10.0f, 10.0f)));
		// �{�N�Z������
		Voxel.SetDrawInfo(CamPos, VNorm(VSub(CamTarget, CamPos)));// �`�悷��ۂ̕`�撆�S���W�ƕ`�悷��������w��
		Voxel.SetShadowDrawInfo(CamPos, ShadowVec);// �V���h�E�}�b�v�ɕ`�悷��ۂ̕`�撆�S���W�ƕ`�悷��������w��
		Voxel.Update();
		// �V���h�E�}�b�v�ւ̕`��
		ShadowMap_DrawSetup(ShadowMapHandle);		// �V���h�E�}�b�v�ւ̕`��̏���
		Voxel.DrawShadow();							// �V���h�E�}�b�v�ւ̕`��
		ShadowMap_DrawEnd();						// �V���h�E�}�b�v�ւ̕`����I��
		// ����ʂւ̕`��
		SetDrawScreen(DX_SCREEN_BACK);				// �`���𗠉�ʂɕύX
		{
			ClearDrawScreen();											// ��ʂ��N���A
			DrawBox(0, 0, 640, 480, GetColor(150, 250, 255), TRUE);		// �w�i�F��h��

			SetCameraPositionAndTarget_UpVecY(CamPos, CamTarget);// �J�����̈ʒu�ƌ�����ݒ�
			SetCameraNearFar(0.5f, 100.0f);			// �`�悷�鉜�s�������͈̔͂�ݒ�

			SetUseShadowMap(0, ShadowMapHandle);	// �`��ɃV���h�E�}�b�v���g�p����
			Voxel.Draw();							// �`��
			SetUseShadowMap(0, -1);					// �`��Ɏg�p����V���h�E�}�b�v�̐ݒ������
		}
		ScreenFlip();								// ����ʂ̓��e��\��ʂɔ��f
	}
	// �I�����̏���
	Voxel.Dispose();
	// �ǂݍ��݂̔j������
	Voxel.Dispose_Load();
	DeleteShadowMap(ShadowMapHandle);	// �V���h�E�}�b�v�̍폜

	DxLib_End();				// �c�w���C�u�����g�p�̏I������
	return 0;					// �\�t�g�̏I��
}
