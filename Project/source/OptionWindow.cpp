#include "OptionWindow.hpp"
#include "Util/Enum.hpp"

void OptionWindow::UpdateColumnStr() noexcept {
	auto* pOption = Util::OptionParam::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	switch (this->m_NowSelectTab) {
	case 0:
	{
		this->m_Param[0].m_Str = std::to_string(pOption->GetParam(pOption->GetOptionType(Util::OptionType::MasterVolume))->GetSelect());
		this->m_Param[1].m_Str = std::to_string(pOption->GetParam(pOption->GetOptionType(Util::OptionType::BGMVolume))->GetSelect());
		this->m_Param[2].m_Str = std::to_string(pOption->GetParam(pOption->GetOptionType(Util::OptionType::SEVolume))->GetSelect());
	}
	break;
	case 1:
	{
		this->m_Param[0].m_Str = pOption->GetParam(pOption->GetOptionType(Util::OptionType::WindowMode))->GetValueNow();
		this->m_Param[1].m_Str = pOption->GetParam(pOption->GetOptionType(Util::OptionType::VSync))->GetValueNow();
		this->m_Param[2].m_Str = pOption->GetParam(pOption->GetOptionType(Util::OptionType::FPSLimit))->GetValueNow();
	}
	break;
	case 2:
	{
		this->m_Param[0].m_Str = std::to_string(pOption->GetParam(pOption->GetOptionType(Util::OptionType::XSensing))->GetSelect());
		this->m_Param[1].m_Str = std::to_string(pOption->GetParam(pOption->GetOptionType(Util::OptionType::YSensing))->GetSelect());

		for (int loop = 2; loop < this->m_NowTabMax; ++loop) {
			this->m_Param[loop].m_Str = Util::KeyParam::GetKeyStr(KeyMngr->GetKeyAssign(static_cast<Util::EnumBattle>(loop - 2), 0));
		}
	}
	break;
	case 3:
		break;
	default:
		break;
	}
	for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
		auto& param = this->m_Param[loop];
		this->m_DrawUI->Get(param.m_ID).GetParts("String0")->SetString(param.m_Str);
	}
}
void OptionWindow::SetTab() noexcept {
	std::string ChildBase = "OptionUI/Child";
	ChildBase += std::to_string(this->m_NowSelectTab + 1);
	Draw::Param2D Param;
	Param.OfsNoRad = Util::VECTOR2D(980, 205);
	{
		std::string Path = "data/UI/Option/Tab";
		Path += std::to_string(this->m_NowSelectTab + 1);
		Path += ".json";
		this->m_DrawUI->AddChild(ChildBase.c_str(), Path.c_str(), Param);
	}
	this->m_NowTabMax = this->m_ParamMax;
	for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
		auto& param = this->m_Param[loop];
		std::string Child = ChildBase;
		Child += "/Column";
		Child += std::to_string(loop + 1);
		param.m_ID = this->m_DrawUI->GetID(Child.c_str());
		if (param.m_ID == -1) {
			this->m_NowTabMax = loop;
			break;
		}
		param.m_MinID = this->m_DrawUI->GetID((Child + "/Box1").c_str());
		param.m_MaxID = this->m_DrawUI->GetID((Child + "/Box0").c_str());

		this->m_DrawUI->Get(param.m_ID).GetParts("String1")->SetString(Util::LocalizePool::Instance()->Get(100 * (this->m_NowSelectTab + 1) + loop));
		if (param.m_MinID != -1) {
			this->m_DrawUI->Get(param.m_MinID).GetParts("String1")->SetString("<");
		}
		if (param.m_MaxID != -1) {
			this->m_DrawUI->Get(param.m_MaxID).GetParts("String1")->SetString(">");
			if (this->m_NowSelectTab == 2) {
				if (loop >= 2) {
					this->m_DrawUI->Get(param.m_MaxID).GetParts("String1")->SetString("Reset");
				}
			}
		}
	}
	UpdateColumnStr();
	this->m_DrawUI->Get(this->m_TabButton[this->m_NowSelectTab]).SetActive(true);
}
void OptionWindow::EndTab() noexcept {
	std::string Path = "OptionUI/Child";
	Path += std::to_string(this->m_NowSelectTab + 1);
	this->m_DrawUI->DeleteChild(Path.c_str());
	this->m_DrawUI->Get(this->m_TabButton[this->m_NowSelectTab]).SetActive(false);
}
void OptionWindow::Init(void) noexcept {
	this->m_DrawUI = new Draw::DrawUISystem();

	this->m_DrawUI->Init("data/UI/Option/OptionBase.json");

	this->m_UIBase = this->m_DrawUI->GetID("");
	for (int loop = 0; loop < 4; ++loop) {
		std::string Path = "OptionUI/Tab";
		Path += std::to_string(loop + 1);
		this->m_TabButton[loop] = this->m_DrawUI->GetID(Path.c_str());
		this->m_DrawUI->Get(this->m_TabButton[loop]).GetParts("String1")->SetString(Util::LocalizePool::Instance()->Get(loop));
		this->m_DrawUI->Get(this->m_TabButton[loop]).SetActive(false);
	}
	this->m_NowSelectTab = 0;
	this->m_CloseButton = this->m_DrawUI->GetID("OptionUI/CloseButton");

	this->m_DrawUI->Get(this->m_UIBase).SetActive(false);
}
void OptionWindow::Update(void) noexcept {
	auto* pOption = Util::OptionParam::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	 if (this->m_DrawUI->Get(this->m_UIBase).IsActive()) {
		 for (int loop = 0; loop < 4; ++loop) {
			 if (this->m_DrawUI->Get(this->m_TabButton[loop]).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				 if (this->m_NowSelectTab != loop) {
					 EndTab();
					 this->m_NowSelectTab = loop;
					 SetTab();
				 }
			 }
		 }
		 if (this->m_DrawUI->Get(this->m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
			 for (int loop = 0; loop < static_cast<int>(Util::InputType::Max); ++loop) {
				 KeyMngr->Save(static_cast<Util::InputType>(loop));
			 }
			 pOption->Save();
			 SetActive(false);
		 }
		 if (KeyMngr->GetMenuKeyRepeat(Util::EnumMenu::Diside)) {
			 {
				 switch (this->m_NowSelectTab) {
				 case 0:
				 {
					 //
					 if (this->m_DrawUI->Get(this->m_Param[0].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::MasterVolume), pOption->GetParam(pOption->GetOptionType(Util::OptionType::MasterVolume))->GetSelect() - 1);
					 }
					 if (this->m_DrawUI->Get(this->m_Param[0].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::MasterVolume), pOption->GetParam(pOption->GetOptionType(Util::OptionType::MasterVolume))->GetSelect() + 1);
					 }
					 //
					 if (this->m_DrawUI->Get(this->m_Param[1].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::BGMVolume), pOption->GetParam(pOption->GetOptionType(Util::OptionType::BGMVolume))->GetSelect() - 1);
					 }
					 if (this->m_DrawUI->Get(this->m_Param[1].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::BGMVolume), pOption->GetParam(pOption->GetOptionType(Util::OptionType::BGMVolume))->GetSelect() + 1);
					 }
					 //
					 if (this->m_DrawUI->Get(this->m_Param[2].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::SEVolume), pOption->GetParam(pOption->GetOptionType(Util::OptionType::SEVolume))->GetSelect() - 1);
					 }
					 if (this->m_DrawUI->Get(this->m_Param[2].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::SEVolume), pOption->GetParam(pOption->GetOptionType(Util::OptionType::SEVolume))->GetSelect() + 1);
					 }
				 }
				 break;
				 case 1:
				 {
					 //
					 if (this->m_DrawUI->Get(this->m_Param[0].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::WindowMode), pOption->GetParam(pOption->GetOptionType(Util::OptionType::WindowMode))->GetSelect() - 1);
						 Draw::MainDraw::Instance()->FlipSetting();
					 }
					 if (this->m_DrawUI->Get(this->m_Param[0].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::WindowMode), pOption->GetParam(pOption->GetOptionType(Util::OptionType::WindowMode))->GetSelect() + 1);
						 Draw::MainDraw::Instance()->FlipSetting();
					 }
					 //
					 if (this->m_DrawUI->Get(this->m_Param[1].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::VSync), pOption->GetParam(pOption->GetOptionType(Util::OptionType::VSync))->GetSelect() - 1);
						 Draw::MainDraw::Instance()->FlipSetting();
					 }
					 if (this->m_DrawUI->Get(this->m_Param[1].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::VSync), pOption->GetParam(pOption->GetOptionType(Util::OptionType::VSync))->GetSelect() + 1);
						 Draw::MainDraw::Instance()->FlipSetting();
					 }
					 //
					 if (this->m_DrawUI->Get(this->m_Param[2].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::FPSLimit), pOption->GetParam(pOption->GetOptionType(Util::OptionType::FPSLimit))->GetSelect() - 1);
						 Draw::MainDraw::Instance()->FlipSetting();
					 }
					 if (this->m_DrawUI->Get(this->m_Param[2].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::FPSLimit), pOption->GetParam(pOption->GetOptionType(Util::OptionType::FPSLimit))->GetSelect() + 1);
						 Draw::MainDraw::Instance()->FlipSetting();
					 }
					 //
				 }
				 break;
				 case 2:
				 {
					 //
					 if (this->m_DrawUI->Get(this->m_Param[0].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::XSensing), pOption->GetParam(pOption->GetOptionType(Util::OptionType::XSensing))->GetSelect() - 1);
					 }
					 if (this->m_DrawUI->Get(this->m_Param[0].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::XSensing), pOption->GetParam(pOption->GetOptionType(Util::OptionType::XSensing))->GetSelect() + 1);
					 }
					 //
					 if (this->m_DrawUI->Get(this->m_Param[1].m_MinID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::YSensing), pOption->GetParam(pOption->GetOptionType(Util::OptionType::YSensing))->GetSelect() - 1);
					 }
					 if (this->m_DrawUI->Get(this->m_Param[1].m_MaxID).IsSelectButton()) {
						 pOption->SetParam(pOption->GetOptionType(Util::OptionType::YSensing), pOption->GetParam(pOption->GetOptionType(Util::OptionType::YSensing))->GetSelect() + 1);
					 }
					 //
					 for (int loop = 2; loop < this->m_NowTabMax; ++loop) {
						 if (this->m_DrawUI->Get(this->m_Param[loop].m_MaxID).IsSelectButton()) {
							 Util::EnumBattle Battle = static_cast<Util::EnumBattle>(loop - 2);
							 KeyMngr->AssignBattleID(Battle, 0, KeyMngr->GetDefaultKeyAssign(Battle, 0));
						 }
					 }
				 }
				 break;
				 case 3:
					 break;
				 default:
					 break;
				 }
			 }
			 UpdateColumnStr();
		 }
		 if (this->m_NowSelectTab == 2) {
			 bool IsChange = false;
			 for (int loop = 2; loop < this->m_NowTabMax; ++loop) {
				 Util::EnumBattle Battle = static_cast<Util::EnumBattle>(loop - 2);
				 if (this->m_DrawUI->Get(this->m_Param[loop].m_ID).IsSelectButton()) {
					 for (int loop2 = static_cast<int>(Util::EnumInput::Begin); loop2 < static_cast<int>(Util::EnumInput::Max); ++loop2) {
						 Util::EnumInput Input = static_cast<Util::EnumInput>(loop2);
						 if (KeyMngr->GetKeyPress(Input, true)) {
							 KeyMngr->AssignBattleID(Battle, 0, Input);
							 IsChange = true;
							 break;
						 }
					 }
				 }
				 if (IsChange) { break; }
			 }
			 if (IsChange || KeyMngr->IsDeviceChange()) {
				 UpdateColumnStr();
			 }
		 }
	 }
	 this->m_DrawUI->Update();
	}