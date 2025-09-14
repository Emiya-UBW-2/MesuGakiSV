#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Draw/MainDraw.hpp"

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
