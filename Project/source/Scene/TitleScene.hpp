#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Util/Util.hpp"
#include "../Util/Localize.hpp"

#include "../Draw/MainDraw.hpp"
#include "../Draw/DrawUISystem.hpp"


#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

class OptionWindow {
	DrawUISystem*	m_DrawUI{ nullptr };
	int				m_UIBase = -1;
	int				m_TabButton[4] = { -1,-1,-1,-1 };
	int				m_ParamID[9]{};

	int				m_ParamMinID[9]{};
	int				m_ParamMaxID[9]{};

	int				m_CloseButton = -1;
	int				m_NowSelectTab = 0;
	int				m_NowTabMax = 0;
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
			SetTab();
		}
		else {
			EndTab();
		}
		m_NowSelectTab = 0;
	}
private:
	void SetTab() noexcept {
		std::string ChildBase = "OptionUI/Child";
		ChildBase += std::to_string(m_NowSelectTab + 1);
		Param2D Param;
		Param.OfsNoRad = VECTOR2D(980, 220);
		{
			std::string Path = "data/UI/Option/Tab";
			Path += std::to_string(m_NowSelectTab + 1);
			Path += ".json";
			m_DrawUI->AddChild(ChildBase.c_str(), Path.c_str(), Param);
		}
		m_NowTabMax = 9;
		std::string ParamStr[9]{};
		{
			auto* pOption = OptionParam::Instance();
			switch (m_NowSelectTab) {
			case 0:
			{
				ParamStr[0] = std::to_string(pOption->GetParam("MasterVolume")->GetSelect());
				ParamStr[1] = std::to_string(pOption->GetParam("BGMVolume")->GetSelect());
				ParamStr[2] = std::to_string(pOption->GetParam("SEVolume")->GetSelect());
			}
				break;
			case 1:
			{
				ParamStr[0] = pOption->GetParam("WindowMode")->GetValueNow();
				ParamStr[1] = pOption->GetParam("VSync")->GetValueNow();
				ParamStr[2] = pOption->GetParam("FPSLimit")->GetValueNow();
			}
			break;
			case 2:
				break;
			case 3:
				break;
			default:
				break;
			}
		}
		for (int loop = 0; loop < m_NowTabMax; ++loop) {
			std::string Child = ChildBase;
			Child += "/Column";
			Child += std::to_string(loop + 1);
			m_ParamID[loop] = m_DrawUI->GetID(Child.c_str());
			if (m_ParamID[loop] == -1) {
				m_NowTabMax = loop;
				break;
			}
			m_ParamMinID[loop] = m_DrawUI->GetID((Child + "/Box1").c_str());
			m_ParamMaxID[loop] = m_DrawUI->GetID((Child + "/Box0").c_str());

			m_DrawUI->Get(m_ParamID[loop]).GetParts("String1")->SetString(LocalizePool::Instance()->Get(10 * (m_NowSelectTab + 1) + loop));
			m_DrawUI->Get(m_ParamID[loop]).GetParts("String0")->SetString(ParamStr[loop]);
			m_DrawUI->Get(m_ParamMinID[loop]).GetParts("String1")->SetString("<");
			m_DrawUI->Get(m_ParamMaxID[loop]).GetParts("String1")->SetString(">");
		}
		m_DrawUI->Get(m_TabButton[m_NowSelectTab]).SetActive(true);
	}
	void EndTab() noexcept {
		std::string Path = "OptionUI/Child";
		Path += std::to_string(m_NowSelectTab + 1);
		m_DrawUI->DeleteChild(Path.c_str());
		m_DrawUI->Get(m_TabButton[m_NowSelectTab]).SetActive(false);
	}
public:
	void Init(void) noexcept {
		m_DrawUI = new DrawUISystem();

		m_DrawUI->Init("data/UI/Option/OptionBase.json");

		m_UIBase = m_DrawUI->GetID("");
		for (int loop = 0; loop < 4; ++loop) {
			std::string Path = "OptionUI/Tab";
			Path += std::to_string(loop + 1);
			m_TabButton[loop] = m_DrawUI->GetID(Path.c_str());
			m_DrawUI->Get(m_TabButton[loop]).GetParts("String1")->SetString(LocalizePool::Instance()->Get(loop));
			m_DrawUI->Get(m_TabButton[loop]).SetActive(false);
		}
		m_NowSelectTab = 0;
		m_CloseButton = m_DrawUI->GetID("OptionUI/CloseButton");

		m_DrawUI->Get(m_UIBase).SetActive(false);
	}
	void Update(void) noexcept {
		auto* KeyMngr = KeyParam::Instance();
		if (m_DrawUI->Get(m_UIBase).IsActive()) {
			for (int loop = 0; loop < 4; ++loop) {
				if (m_DrawUI->Get(m_TabButton[loop]).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
					EndTab();
					m_NowSelectTab = loop;
					SetTab();
				}
			}
			if (m_DrawUI->Get(m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
				SetActive(false);
			}
			if (KeyMngr->GetMenuKeyRepeat(EnumMenu::Diside)) {
				std::string ParamStr[9]{};
				{
					auto* pOption = OptionParam::Instance();
					switch (m_NowSelectTab) {
					case 0:
					{
						//
						if (m_DrawUI->Get(m_ParamMinID[0]).IsSelectButton()) {
							pOption->SetParam("MasterVolume", pOption->GetParam("MasterVolume")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_ParamMaxID[0]).IsSelectButton()) {
							pOption->SetParam("MasterVolume", pOption->GetParam("MasterVolume")->GetSelect() + 1);
						}
						ParamStr[0] = std::to_string(pOption->GetParam("MasterVolume")->GetSelect());
						//
						if (m_DrawUI->Get(m_ParamMinID[1]).IsSelectButton()) {
							pOption->SetParam("BGMVolume", pOption->GetParam("BGMVolume")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_ParamMaxID[1]).IsSelectButton()) {
							pOption->SetParam("BGMVolume", pOption->GetParam("BGMVolume")->GetSelect() + 1);
						}
						ParamStr[1] = std::to_string(pOption->GetParam("BGMVolume")->GetSelect());
						//
						if (m_DrawUI->Get(m_ParamMinID[2]).IsSelectButton()) {
							pOption->SetParam("SEVolume", pOption->GetParam("SEVolume")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_ParamMaxID[2]).IsSelectButton()) {
							pOption->SetParam("SEVolume", pOption->GetParam("SEVolume")->GetSelect() + 1);
						}
						ParamStr[2] = std::to_string(pOption->GetParam("SEVolume")->GetSelect());
						//
					}
					break;
					case 1:
					{
						//
						if (m_DrawUI->Get(m_ParamMinID[0]).IsSelectButton()) {
							pOption->SetParam("WindowMode", pOption->GetParam("WindowMode")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_ParamMaxID[0]).IsSelectButton()) {
							pOption->SetParam("WindowMode", pOption->GetParam("WindowMode")->GetSelect() + 1);
						}
						ParamStr[0] = pOption->GetParam("WindowMode")->GetValueNow();
						//
						if (m_DrawUI->Get(m_ParamMinID[1]).IsSelectButton()) {
							pOption->SetParam("VSync", pOption->GetParam("VSync")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_ParamMaxID[1]).IsSelectButton()) {
							pOption->SetParam("VSync", pOption->GetParam("VSync")->GetSelect() + 1);
						}
						ParamStr[1] = pOption->GetParam("VSync")->GetValueNow();
						//
						if (m_DrawUI->Get(m_ParamMinID[2]).IsSelectButton()) {
							pOption->SetParam("FPSLimit", pOption->GetParam("FPSLimit")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_ParamMaxID[2]).IsSelectButton()) {
							pOption->SetParam("FPSLimit", pOption->GetParam("FPSLimit")->GetSelect() + 1);
						}
						ParamStr[2] = pOption->GetParam("FPSLimit")->GetValueNow();
						//
					}
					break;
					case 2:
						break;
					case 3:
						break;
					default:
						break;
					}
				}
				for (int loop = 0; loop < m_NowTabMax; ++loop) {
					m_DrawUI->Get(m_ParamID[loop]).GetParts("String0")->SetString(ParamStr[loop]);
				}
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
