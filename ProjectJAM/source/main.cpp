#include "Manage/Enum.hpp"
#include "Manage/SceneManager.hpp"
#include "Manage/MainDraw.hpp"

//シーンをinclude
#include "Scene/SceneEnum.hpp"
#include "Scene/SampleScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//常駐員の作成
	Draw::MainDraw::Create();//描画管理人
	Scene::SceneManager::Create();//シーン管理人
	auto* DrawerMngr = Draw::MainDraw::Instance();//管理人のポインタ呼び出し
	auto* SceneMngr = Scene::SceneManager::Instance();//管理人のポインタ呼び出し
	//シーン定義
	SampleScene Sample{};
	//シーン設定
	SceneMngr->AddScenePtrList(&Sample);
	SceneMngr->SetFirstScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Sample)));
	//メインループ開始
	DrawerMngr->LoopStart();
	while (DrawerMngr->CanContinueLoop()) {
		//更新
		DrawerMngr->Update();
		for (int loop = 0; loop < DrawerMngr->GetUpdateTickCount(); ++loop) {
			SceneMngr->Update();
		}
		//描画
		DrawerMngr->StartDraw();
		{
			//メイン画面
			SceneMngr->Draw();
			//UI画面
			SceneMngr->UIDraw();
		}
		DrawerMngr->EndDraw();
		//終了判定
		if (SceneMngr->IsEndScene()) {
			break;
		}
	}
	//常駐員の開放
	Scene::SceneManager::Release();
	Draw::MainDraw::Release();
	return 0;// ソフトの終了 
}
