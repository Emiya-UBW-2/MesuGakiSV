#include "Draw/MainDraw.hpp"

#include "Util/SceneManager.hpp"
#include "Util/Enum.hpp"
#include "Scene/TitleScene.hpp"
#include "Scene/MainScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	MainDraw::Create();
	auto* DrawerMngr = MainDraw::Instance();
	SceneManager::Create();
	auto* SceneMngr = SceneManager::Instance();
	TitleScene Title{};
	MainScene Main{};
	//�V�[���ݒ�
	SceneMngr->AddScenePtrList(&Title);
	SceneMngr->AddScenePtrList(&Main);
	SceneMngr->SetFirstScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Title)));
	//���C�����[�v�J�n
	DrawerMngr->LoopStart();
	while (DrawerMngr->CanContinueLoop()) {
		//�X�V
		DrawerMngr->Update();
		for (int loop = 0; loop < DrawerMngr->GetUpdateTickCount(); ++loop) {
			SceneMngr->Update();
		}
		//�`��
		DrawerMngr->StartDraw();
		SceneMngr->Draw();
		DrawerMngr->EndDraw();
		//�I������
		if (SceneMngr->IsEndScene()) {
			break;
		}
	}
	SceneManager::Release();

	MainDraw::Release();
	return 0;// �\�t�g�̏I�� 
}
