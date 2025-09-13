#include "Util.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXLibDrawer::Create();
	auto* DrawerMngr = DXLibDrawer::Instance();
	//���C�����[�v�J�n
	DrawerMngr->LoopStart();
	while (DrawerMngr->CanContinueLoop()) {
		int Count = DrawerMngr->CalcUpdateTickCount();//���̎���ł͉���A�b�v�f�[�g�ł��邩���`�F�b�N����
		for (int loop = 0; loop < Count; ++loop) {
			//�X�V
		}
		DrawerMngr->StartDraw();
		{
			//�`��
			DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(255, 255, 255), TRUE);
		}
		DrawerMngr->EndDraw();
	}
	DXLibDrawer::Release();
	return 0;// �\�t�g�̏I�� 
}
