#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)

#include "../OptionWindow.hpp"

#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Util/Util.hpp"

#include "../Draw/MainDraw.hpp"

class TitleScene : public SceneBase {
	OptionWindow m_OptionWindow;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		m_OptionWindow.Init();
	}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = SceneManager::Instance();
		auto* KeyMngr = KeyParam::Instance();

		if (!m_OptionWindow.IsActive()) {
			if (KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
				m_OptionWindow.SetActive(true);
			}
		}
		if (KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Cancel)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
		m_OptionWindow.Update();
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 0, 255), TRUE);

		m_OptionWindow.Draw();
	}
	void Dispose_Sub(void) noexcept override {
		m_OptionWindow.Dispose();
	}
};
