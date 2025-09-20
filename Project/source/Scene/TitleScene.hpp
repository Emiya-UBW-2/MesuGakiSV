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

	int m_UIBase = -1;
	int m_CloseButton = -1;
	//char		padding[4]{};
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		DrawUISystem::Create();
		auto* DrawUI = DrawUISystem::Instance();
		DrawUI->Init("data/UI000.json");

		m_UIBase = DrawUI->GetID("");
		m_CloseButton = DrawUI->GetID("OptionUI/CloseButton");

		DrawUI->Get(m_UIBase).SetActive(false);
	}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = SceneManager::Instance();
		auto* KeyMngr = KeyParam::Instance();
		auto* DrawUI = DrawUISystem::Instance();
		if (!DrawUI->Get(m_UIBase).IsActive()) {
			if (KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
				DrawUI->Get(m_UIBase).SetActive(true);
				Param2D Param;
				Param.OfsNoRad = VECTOR2D(1170, 220);
				DrawUI->AddChild("OptionUI/Child1", "data/UI001B.json", Param);
			}
		}
		else {
			if (DrawUI->Get(m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
				DrawUI->Get(m_UIBase).SetActive(false);

				DrawUI->DeleteChild("OptionUI/Child1");
			}
		}

		if (KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Cancel)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
		DrawUI->Update();
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 0, 255), TRUE);

		DrawUISystem::Instance()->Draw();
	}
	void Dispose_Sub(void) noexcept override {
		DrawUISystem::Release();
	}
};
