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

#pragma warning( push, 3 )
#include <functional>
#pragma warning( pop )
class TitleUI {
private:
	Draw::DrawUISystem* m_DrawUI{ nullptr };
	int				m_UIBase = -1;
	int				m_ButtonID[4] = { -1,-1,-1,-1 };
	std::array<std::function<void()>, 4>	m_ButtonDo{};
public:
	TitleUI(void) noexcept {}
	TitleUI(const TitleUI&) = delete;
	TitleUI(TitleUI&&) = delete;
	TitleUI& operator=(const TitleUI&) = delete;
	TitleUI& operator=(TitleUI&&) = delete;
	virtual ~TitleUI(void) noexcept {}
public:
	bool		IsActive(void) const noexcept { return this->m_DrawUI->Get(this->m_UIBase).IsActive(); }
	void		SetActive(bool value) noexcept { this->m_DrawUI->Get(this->m_UIBase).SetActive(value); }
	void		SetEvent(int ID, const std::function<void()>& value) noexcept { this->m_ButtonDo[ID] = value; }
public:
	void		Init(void) noexcept {
		this->m_DrawUI = new Draw::DrawUISystem();

		this->m_DrawUI->Init("data/UI/Title/TitleUI.json");

		this->m_UIBase = this->m_DrawUI->GetID("");
		for (int loop = 0; loop < 4; ++loop) {
			std::string Path = "Tab";
			Path += std::to_string(loop + 1);
			this->m_ButtonID[loop] = this->m_DrawUI->GetID(Path.c_str());
			this->m_DrawUI->Get(this->m_ButtonID[loop]).GetParts("String1")->SetString(Util::LocalizePool::Instance()->Get(1000 + loop));
		}
	}
	void		Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (this->m_DrawUI->Get(this->m_UIBase).IsActive()) {
			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				for (int loop = 0; loop < 4; ++loop) {
					if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
						if (m_ButtonDo[loop]) {
							m_ButtonDo[loop]();
						}
						break;
					}
				}
			}
		}
		this->m_DrawUI->Update(IsActive());
	}
	void		Draw(void) noexcept {
		this->m_DrawUI->Draw();
	}
	void		Dispose(void) noexcept {
		delete this->m_DrawUI;
	}
};

class TitleScene : public Util::SceneBase {
	OptionWindow m_OptionWindow;
	TitleUI m_TitleUI;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		this->m_OptionWindow.Init();
		this->m_TitleUI.Init();

		this->m_TitleUI.SetEvent(0, [this]() {
			auto* SceneMngr = Util::SceneManager::Instance();
			this->m_OptionWindow.SetActive(false);
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
			});
		this->m_TitleUI.SetEvent(1, [this]() {
			});
		this->m_TitleUI.SetEvent(2, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_TitleUI.SetEvent(3, [this]() {
			});
	}
	void Update_Sub(void) noexcept override {
		this->m_TitleUI.SetActive(!this->m_OptionWindow.IsActive());
		this->m_TitleUI.Update();
		this->m_OptionWindow.Update();
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 0, 255), TRUE);

		this->m_TitleUI.Draw();
		this->m_OptionWindow.Draw();
	}
	void Dispose_Sub(void) noexcept override {
		this->m_TitleUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
