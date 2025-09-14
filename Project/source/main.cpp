#include "MainDraw.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	MainDraw::Create();
	auto* DrawerMngr = MainDraw::Instance();
	//���C�����[�v�J�n
	DrawerMngr->LoopStart();
	while (DrawerMngr->CanContinueLoop()) {
		DrawerMngr->Update();
		for (int loop = 0; loop < DrawerMngr->GetUpdateTickCount(); ++loop) {
			//�X�V
		}
		DrawerMngr->StartDraw();
		{
			//�`��
			DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(255, 255, 255), TRUE);
			DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, GetColor(255, 0, 0));
		}
		DrawerMngr->EndDraw();
	}
	MainDraw::Release();
	return 0;// �\�t�g�̏I�� 
}
