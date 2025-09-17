#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"

#include "../Draw/DrawUISystem.hpp"

#include "../Util/Util.hpp"
#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

class TitleScene : public SceneBase {
	DrawModule m_DrawModule;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		m_DrawModule.Init("data/Button001.json");
	}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = SceneManager::Instance();
		auto* KeyMngr = KeyParam::Instance();
		if (m_DrawModule.IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}

		m_DrawModule.Update(VECTOR2D(400.f, 200.f), deg2rad(10.f), VECTOR2D(2.f, 1.f));
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 0, 255), TRUE);

		m_DrawModule.Draw(VECTOR2D(400.f, 200.f), deg2rad(10.f), VECTOR2D(2.f, 1.f));
	}
	void Dispose_Sub(void) noexcept override {}
};
