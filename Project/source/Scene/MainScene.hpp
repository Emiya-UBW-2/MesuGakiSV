#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"

class MainScene : public Util::SceneBase {
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = Util::SceneManager::Instance();
		auto* KeyMngr = Util::KeyParam::Instance();
		if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 255, 0), TRUE);
		DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, GetColor(255, 0, 0));
	}
	void Dispose_Sub(void) noexcept override {}
};
