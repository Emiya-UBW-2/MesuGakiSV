#include "MainDraw.hpp"

#include "SceneManager.hpp"

enum class EnumScene {
	Title,
	Main,
};
class TitleScene : public SceneBase {
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override {
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			auto* SceneMngr = SceneManager::Instance();
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(255, 255, 255), TRUE);
		DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, GetColor(255, 0, 0));
	}
	void Dispose_Sub(void) noexcept override {}
};
class MainScene : public SceneBase {
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override {
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			auto* SceneMngr = SceneManager::Instance();
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 255, 0), TRUE);
		DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, GetColor(255, 0, 0));
	}
	void Dispose_Sub(void) noexcept override {}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	MainDraw::Create();
	auto* DrawerMngr = MainDraw::Instance();
	SceneManager::Create();
	auto* SceneMngr = SceneManager::Instance();
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
	SceneManager::Release();

	MainDraw::Release();
	return 0;// ソフトの終了 
}
