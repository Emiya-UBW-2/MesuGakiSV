#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "Draw/MainDraw.hpp"

#include "Util/SceneManager.hpp"
#include "Util/Enum.hpp"
#include "Util/Key.hpp"
#include "Scene/TitleScene.hpp"
#include "Scene/MainScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	MainDraw::Create();
	SceneManager::Create();
	KeyParam::Create();
	auto* DrawerMngr = MainDraw::Instance();
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
			KeyParam::Instance()->Update();
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
	KeyParam::Release();
	SceneManager::Release();
	MainDraw::Release();
	return 0;// �\�t�g�̏I�� 
}
