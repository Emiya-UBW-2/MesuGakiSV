#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#include "Draw/MainDraw.hpp"

#include "Util/SceneManager.hpp"
#include "Util/Enum.hpp"
#include "Util/Key.hpp"
#include "Scene/TitleScene.hpp"
#include "Scene/MainScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Draw::MainDraw::Create();
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* SceneMngr = Util::SceneManager::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	TitleScene Title{};
	MainScene Main{};
	//シーン設定
	SceneMngr->AddScenePtrList(&Title);
	SceneMngr->AddScenePtrList(&Main);
	SceneMngr->SetFirstScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Title)));
	//メインループ開始
	DrawerMngr->LoopStart();
	while (DrawerMngr->CanContinueLoop()) {
		//更新
		DrawerMngr->Update();
		for (int loop = 0; loop < DrawerMngr->GetUpdateTickCount(); ++loop) {
			KeyMngr->Update();
			SceneMngr->Update();
		}
		//描画
		DrawerMngr->StartDraw();
		SceneMngr->Draw();
		DrawerMngr->EndDraw();
		//終了判定
		if (SceneMngr->IsEndScene()) {
			break;
		}
	}
	Draw::MainDraw::Release();
	return 0;// ソフトの終了 
}
