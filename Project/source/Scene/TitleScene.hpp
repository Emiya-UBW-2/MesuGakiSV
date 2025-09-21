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
	struct ParamButton {
		std::string		Str{};
		std::string		SubStr{};
		int				ID{};
		int				MinID{};
		int				MaxID{};
		char		padding[4]{};
	};

	constexpr static int	m_ParamMax{17};

	DrawUISystem*	m_DrawUI{ nullptr };
	int				m_TabButton[4] = { -1,-1,-1,-1 };
	int				m_UIBase = -1;
	int				m_CloseButton = -1;
	int				m_NowSelectTab = 0;
	int				m_NowTabMax = 0;
	ParamButton		m_Param[m_ParamMax]{};
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
		Param.OfsNoRad = VECTOR2D(980, 205);
		{
			std::string Path = "data/UI/Option/Tab";
			Path += std::to_string(m_NowSelectTab + 1);
			Path += ".json";
			m_DrawUI->AddChild(ChildBase.c_str(), Path.c_str(), Param);
		}
		m_NowTabMax = m_ParamMax;
		{
			auto* pOption = OptionParam::Instance();
			switch (m_NowSelectTab) {
			case 0:
			{
				m_Param[0].Str = std::to_string(pOption->GetParam("MasterVolume")->GetSelect());
				m_Param[1].Str = std::to_string(pOption->GetParam("BGMVolume")->GetSelect());
				m_Param[2].Str = std::to_string(pOption->GetParam("SEVolume")->GetSelect());
			}
				break;
			case 1:
			{
				m_Param[0].Str = pOption->GetParam("WindowMode")->GetValueNow();
				m_Param[1].Str = pOption->GetParam("VSync")->GetValueNow();
				m_Param[2].Str = pOption->GetParam("FPSLimit")->GetValueNow();
			}
			break;
			case 2:
			{
				m_Param[0].Str = std::to_string(pOption->GetParam("XSensing")->GetSelect());
				m_Param[1].Str = std::to_string(pOption->GetParam("YSensing")->GetSelect());

				m_Param[2].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::W, 0));
				m_Param[2].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::W, 1));
				m_Param[3].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::S, 0));
				m_Param[3].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::S, 1));
				m_Param[4].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::A, 0));
				m_Param[4].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::A, 1));
				m_Param[5].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::D, 0));
				m_Param[5].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::D, 1));

				m_Param[6].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Q, 0));
				m_Param[6].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Q, 1));
				m_Param[7].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::E, 0));
				m_Param[7].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::E, 1));
				m_Param[8].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Run, 0));
				m_Param[8].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Run, 1));
				m_Param[9].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Walk, 0));
				m_Param[9].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Walk, 1));
				m_Param[10].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Squat, 0));
				m_Param[10].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Squat, 1));
				m_Param[11].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Prone, 0));
				m_Param[11].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Prone, 1));
				m_Param[12].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Jump, 0));
				m_Param[12].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Jump, 1));
				m_Param[13].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Attack, 0));
				m_Param[13].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Attack, 1));
				m_Param[14].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Aim, 0));
				m_Param[14].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Aim, 1));
				m_Param[15].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::ChangeWeapon, 0));
				m_Param[15].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::ChangeWeapon, 1));
				m_Param[16].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Reload, 0));
				m_Param[16].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Reload, 1));
			}
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
			m_Param[loop].ID = m_DrawUI->GetID(Child.c_str());
			if (m_Param[loop].ID == -1) {
				m_NowTabMax = loop;
				break;
			}
			m_Param[loop].MinID = m_DrawUI->GetID((Child + "/Box1").c_str());
			m_Param[loop].MaxID = m_DrawUI->GetID((Child + "/Box0").c_str());

			m_DrawUI->Get(m_Param[loop].ID).GetParts("String1")->SetString(LocalizePool::Instance()->Get(100 * (m_NowSelectTab + 1) + loop));
			m_DrawUI->Get(m_Param[loop].ID).GetParts("String0")->SetString(m_Param[loop].Str);
			if (m_Param[loop].MinID != -1) {
				m_DrawUI->Get(m_Param[loop].MinID).GetParts("String1")->SetString("<");
			}
			if (m_Param[loop].MaxID != -1) {
				m_DrawUI->Get(m_Param[loop].MaxID).GetParts("String1")->SetString(">");
			}
			if (m_DrawUI->Get(m_Param[loop].ID).GetParts("String2")) {
				m_DrawUI->Get(m_Param[loop].ID).GetParts("String2")->SetString(m_Param[loop].SubStr);
			}
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
					if (m_NowSelectTab != loop) {
						EndTab();
						m_NowSelectTab = loop;
						SetTab();
					}
				}
			}
			if (m_DrawUI->Get(m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
				SetActive(false);
			}
			if (KeyMngr->GetMenuKeyRepeat(EnumMenu::Diside)) {
				{
					auto* pOption = OptionParam::Instance();
					switch (m_NowSelectTab) {
					case 0:
					{
						//
						if (m_DrawUI->Get(m_Param[0].MinID).IsSelectButton()) {
							pOption->SetParam("MasterVolume", pOption->GetParam("MasterVolume")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[0].MaxID).IsSelectButton()) {
							pOption->SetParam("MasterVolume", pOption->GetParam("MasterVolume")->GetSelect() + 1);
						}
						m_Param[0].Str = std::to_string(pOption->GetParam("MasterVolume")->GetSelect());
						//
						if (m_DrawUI->Get(m_Param[1].MinID).IsSelectButton()) {
							pOption->SetParam("BGMVolume", pOption->GetParam("BGMVolume")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[1].MaxID).IsSelectButton()) {
							pOption->SetParam("BGMVolume", pOption->GetParam("BGMVolume")->GetSelect() + 1);
						}
						m_Param[1].Str = std::to_string(pOption->GetParam("BGMVolume")->GetSelect());
						//
						if (m_DrawUI->Get(m_Param[2].MinID).IsSelectButton()) {
							pOption->SetParam("SEVolume", pOption->GetParam("SEVolume")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[2].MaxID).IsSelectButton()) {
							pOption->SetParam("SEVolume", pOption->GetParam("SEVolume")->GetSelect() + 1);
						}
						m_Param[2].Str = std::to_string(pOption->GetParam("SEVolume")->GetSelect());
						//
					}
					break;
					case 1:
					{
						//
						if (m_DrawUI->Get(m_Param[0].MinID).IsSelectButton()) {
							pOption->SetParam("WindowMode", pOption->GetParam("WindowMode")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[0].MaxID).IsSelectButton()) {
							pOption->SetParam("WindowMode", pOption->GetParam("WindowMode")->GetSelect() + 1);
						}
						m_Param[0].Str = pOption->GetParam("WindowMode")->GetValueNow();
						//
						if (m_DrawUI->Get(m_Param[1].MinID).IsSelectButton()) {
							pOption->SetParam("VSync", pOption->GetParam("VSync")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[1].MaxID).IsSelectButton()) {
							pOption->SetParam("VSync", pOption->GetParam("VSync")->GetSelect() + 1);
						}
						m_Param[1].Str = pOption->GetParam("VSync")->GetValueNow();
						//
						if (m_DrawUI->Get(m_Param[2].MinID).IsSelectButton()) {
							pOption->SetParam("FPSLimit", pOption->GetParam("FPSLimit")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[2].MaxID).IsSelectButton()) {
							pOption->SetParam("FPSLimit", pOption->GetParam("FPSLimit")->GetSelect() + 1);
						}
						m_Param[2].Str = pOption->GetParam("FPSLimit")->GetValueNow();
						//
					}
					break;
					case 2:
					{
						if (m_DrawUI->Get(m_Param[0].MinID).IsSelectButton()) {
							pOption->SetParam("XSensing", pOption->GetParam("XSensing")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[0].MaxID).IsSelectButton()) {
							pOption->SetParam("XSensing", pOption->GetParam("XSensing")->GetSelect() + 1);
						}
						m_Param[0].Str = std::to_string(pOption->GetParam("XSensing")->GetSelect());
						if (m_DrawUI->Get(m_Param[1].MinID).IsSelectButton()) {
							pOption->SetParam("YSensing", pOption->GetParam("YSensing")->GetSelect() - 1);
						}
						if (m_DrawUI->Get(m_Param[1].MaxID).IsSelectButton()) {
							pOption->SetParam("YSensing", pOption->GetParam("YSensing")->GetSelect() + 1);
						}
						m_Param[1].Str = std::to_string(pOption->GetParam("YSensing")->GetSelect());
					}
					break;
					case 3:
						break;
					default:
						break;
					}
				}
				for (int loop = 0; loop < m_NowTabMax; ++loop) {
					m_DrawUI->Get(m_Param[loop].ID).GetParts("String0")->SetString(m_Param[loop].Str);
				}
			}
			if (false || KeyParam::Instance()->IsDeviceChange()) {
				if (m_NowSelectTab == 2) {
					m_Param[2].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::W, 0));
					m_Param[2].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::W, 1));
					m_Param[3].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::S, 0));
					m_Param[3].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::S, 1));
					m_Param[4].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::A, 0));
					m_Param[4].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::A, 1));
					m_Param[5].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::D, 0));
					m_Param[5].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::D, 1));

					m_Param[6].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Q, 0));
					m_Param[6].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Q, 1));
					m_Param[7].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::E, 0));
					m_Param[7].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::E, 1));
					m_Param[8].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Run, 0));
					m_Param[8].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Run, 1));
					m_Param[9].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Walk, 0));
					m_Param[9].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Walk, 1));
					m_Param[10].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Squat, 0));
					m_Param[10].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Squat, 1));
					m_Param[11].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Prone, 0));
					m_Param[11].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Prone, 1));
					m_Param[12].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Jump, 0));
					m_Param[12].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Jump, 1));
					m_Param[13].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Attack, 0));
					m_Param[13].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Attack, 1));
					m_Param[14].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Aim, 0));
					m_Param[14].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Aim, 1));
					m_Param[15].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::ChangeWeapon, 0));
					m_Param[15].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::ChangeWeapon, 1));
					m_Param[16].Str = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Reload, 0));
					m_Param[16].SubStr = KeyParam::GetKeyStr(KeyParam::Instance()->GetBattleKeyAssign(EnumBattle::Reload, 1));
					for (int loop = 2; loop < m_NowTabMax; ++loop) {
						m_DrawUI->Get(m_Param[loop].ID).GetParts("String0")->SetString(m_Param[loop].Str);
						m_DrawUI->Get(m_Param[loop].ID).GetParts("String2")->SetString(m_Param[loop].SubStr);
					}
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
