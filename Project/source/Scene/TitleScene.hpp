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

	int m_CloseButton = -1;
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
		DrawUISystem::Instance()->Init("data/UI000.json");

		m_CloseButton = DrawUISystem::Instance()->GetID("OptionUI/CloseButton");
	}
	void Update_Sub(void) noexcept override {
		//*
		auto* SceneMngr = SceneManager::Instance();
		auto* KeyMngr = KeyParam::Instance();
		if (DrawUISystem::Instance()->Get(m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
		//*/
		DrawUISystem::Instance()->Update();
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
