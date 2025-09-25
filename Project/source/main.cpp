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
	Util::OptionParam::Create();

	Draw::MainDraw::Create();

	Util::SceneManager::Create();
	Util::KeyParam::Create();
	Draw::FontPool::Create();
	Draw::GraphPool::Create();
	Util::LocalizePool::Create();
	Camera::Camera3D::Create();
	DXLibRef::PostPassEffect::Create();

	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* SceneMngr = Util::SceneManager::Instance();

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
			Util::KeyParam::Instance()->Update();
			SceneMngr->Update();
			Camera::Camera3D::Instance()->Update();
		}
		//描画

		// キューブマップをセット
		SceneMngr->ReadyDraw();
		DrawerMngr->StartDraw();
		SceneMngr->Draw();
		DrawerMngr->EndDraw();
		//終了判定
		if (SceneMngr->IsEndScene()) {
			break;
		}
	}
	DXLibRef::PostPassEffect::Release();
	Camera::Camera3D::Release();
	Util::LocalizePool::Release();
	Draw::GraphPool::Release();
	Draw::FontPool::Release();
	Util::KeyParam::Release();
	Util::SceneManager::Release();

	Draw::MainDraw::Release();

	Util::OptionParam::Release();
	return 0;// ソフトの終了 
}
