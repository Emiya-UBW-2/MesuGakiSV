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
#include "Draw/Light.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Util::OptionParam::Create();

	Draw::MainDraw::Create();

	Util::SceneManager::Create();
	Util::KeyParam::Create();
	Draw::FontPool::Create();
	Draw::GraphPool::Create();
	Util::LocalizePool::Create();
	Camera::Camera3D::Create();
	Draw::PostPassEffect::Create();
	DXLibRef::LightPool::Create();

	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* SceneMngr = Util::SceneManager::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();

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
			CameraParts->Update();
		}
		//描画
		{
			// キューブマップをセット
			{
				Util::VECTOR3D Pos = CameraParts->GetMainCamera().GetCamPos(); Pos.y *= -1.f;
				PostPassParts->Update_CubeMap([]() { Util::SceneManager::Instance()->CubeMapDraw(); }, Pos);
			}
			// 影をセット
			{
				if (PostPassParts->UpdateShadowActive() && false) {
					Util::VECTOR3D Pos = CameraParts->GetMainCamera().GetCamPos();
					Pos.x = 0.f;
					Pos.z = 0.f;
					PostPassParts->Update_Shadow([]() { Util::SceneManager::Instance()->ShadowFarDraw3D(); }, Pos, true);
				}
				PostPassParts->Update_Shadow([]() { Util::SceneManager::Instance()->ShadowDraw3D(); }, CameraParts->GetMainCamera().GetCamPos(), false);
			}
			// 全ての画面を初期化
			PostPassParts->StartDraw();
			// 空
			PostPassParts->DrawGBuffer(1000.0f, 50000.0f, []() { Util::SceneManager::Instance()->BGDraw(); });
			// 3距離
			{
				Draw::Camera3DInfo camInfo = CameraParts->GetMainCamera();
				float Far = 1000000.f;
				float Near = camInfo.GetCamFar();
				for (int loop = 0; loop < 3; ++loop) {
					PostPassParts->DrawGBuffer(Near, Far, []() { Util::SceneManager::Instance()->Draw3D(); });
					Far = Near;
					if (loop == 0) {
						Near = camInfo.GetCamNear();
					}
					else if (loop == 1) {
						Near = 0.01f;
					}
				}
			}
			// 影
			PostPassParts->SetDrawShadow([]() { Util::SceneManager::Instance()->Draw3D(); }, []() { Util::SceneManager::Instance()->Draw3D(); });
			// ポストプロセス
			PostPassParts->DrawPostProcess();
		}
		DrawerMngr->StartDraw();
		{
			PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), false);
			SceneMngr->UIDraw();
		}
		DrawerMngr->EndDraw();
		//終了判定
		if (SceneMngr->IsEndScene()) {
			break;
		}
	}
	DXLibRef::LightPool::Release();
	Draw::PostPassEffect::Release();
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
