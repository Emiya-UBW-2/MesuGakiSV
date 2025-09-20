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

class OptionWindow {
	DrawUISystem*	m_DrawUI{ nullptr };
	int				m_UIBase = -1;
	int				m_TabButton[4] = { -1,-1,-1,-1 };
	int				m_CloseButton = -1;
	int				m_NowSelect = 0;
	char		padding[4]{};
public:
	OptionWindow(void) noexcept {}
	OptionWindow(const OptionWindow&) = delete;
	OptionWindow(OptionWindow&&) = delete;
	OptionWindow& operator=(const OptionWindow&) = delete;
	OptionWindow& operator=(OptionWindow&&) = delete;
	virtual ~OptionWindow(void) noexcept {}
public:
	bool IsActive(void) const noexcept {
		return m_DrawUI->Get(m_UIBase).IsActive();
	}
	void SetActive(bool value) noexcept {
		m_DrawUI->Get(m_UIBase).SetActive(value);
		if (IsActive()) {
			std::string Path = "OptionUI/Child";
			Path += std::to_string(m_NowSelect + 1);
			Param2D Param;
			Param.OfsNoRad = VECTOR2D(1170, 220);
			m_DrawUI->AddChild(Path.c_str(), "data/UI001B.json", Param);
		}
		else {
			std::string Path = "OptionUI/Child";
			Path += std::to_string(m_NowSelect + 1);
			m_DrawUI->DeleteChild(Path.c_str());
		}
		m_NowSelect = 0;
	}
public:
	void Init(void) noexcept {
		m_DrawUI = new DrawUISystem();

		m_DrawUI->Init("data/UI000.json");

		m_UIBase = m_DrawUI->GetID("");
		for (int loop = 0; loop < 4; ++loop) {
			std::string Path = "OptionUI/Tab";
			Path += std::to_string(loop + 1);
			m_TabButton[loop] = m_DrawUI->GetID(Path.c_str());
		}
		m_NowSelect = 0;
		m_CloseButton = m_DrawUI->GetID("OptionUI/CloseButton");

		m_DrawUI->Get(m_UIBase).SetActive(false);
	}
	void Update(void) noexcept {
		auto* KeyMngr = KeyParam::Instance();
		if (m_DrawUI->Get(m_UIBase).IsActive()) {
			for (int loop = 0; loop < 4; ++loop) {
				if (m_DrawUI->Get(m_TabButton[loop]).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
					{
						std::string Path = "OptionUI/Child";
						Path += std::to_string(m_NowSelect + 1);
						m_DrawUI->DeleteChild(Path.c_str());
					}
					m_NowSelect = loop;
					{
						std::string Path = "OptionUI/Child";
						Path += std::to_string(m_NowSelect + 1);
						Param2D Param;
						Param.OfsNoRad = VECTOR2D(1170, 220);
						m_DrawUI->AddChild(Path.c_str(), "data/UI001B.json", Param);
					}
				}
			}
			if (m_DrawUI->Get(m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
				SetActive(false);
			}
		}
		m_DrawUI->Update();
	}
	void Draw(void) noexcept {
		m_DrawUI->Draw();
	}
	void Dispose(void) noexcept {
		delete m_DrawUI;
	}
};

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
