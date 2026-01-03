#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)

#include "../OptionWindow.hpp"
#include "../TitleUI.hpp"

#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Util/Util.hpp"

#include "../Draw/MainDraw.hpp"

class CreditControl {
	static const int			CharMax = 256;
	int							m_CreditCoulm{ 0 };
	std::array<std::pair<char[CharMax], char[CharMax]>, 64>	m_CreditStr{};
public:
	CreditControl(void) noexcept;
	CreditControl(const CreditControl&) = delete;
	CreditControl(CreditControl&&) = delete;
	CreditControl& operator=(const CreditControl&) = delete;
	CreditControl& operator=(CreditControl&&) = delete;

	virtual ~CreditControl(void) noexcept;
public:
	void Draw(int xmin, int ymin, int xmax) const noexcept;
};

class TitleScene : public Util::SceneBase {
	OptionWindow	m_OptionWindow;
	TitleUI			m_TitleUI;
	EndUI			m_EndUI;

	Sound::SoundUniqueID			m_OKID{ InvalidID };

	Sound::SoundUniqueID			m_TitleBGMID{ InvalidID };
	std::unique_ptr<CreditControl>		m_CreditControl{};
	bool								m_IsActiveCredit{};
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
		// クレジット
		this->m_CreditControl = std::make_unique<CreditControl>();
		m_IsActiveCredit = false;

		this->m_OptionWindow.Init();

		this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Toho/decide.wav", false);

		this->m_TitleBGMID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::BGM, 1, "data/Sound/BGM/Title.wav", false);

		this->m_TitleUI.Init();
		this->m_TitleUI.SetEvent(0, [this]() {
			this->m_TitleUI.SetEnd();
			});
		this->m_TitleUI.SetEvent(1, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_TitleUI.SetEvent(2, [this]() {
			m_IsActiveCredit = true;
			});

		this->m_EndUI.Init();
		this->m_EndUI.SetEvent([this]() {
			Util::SceneBase::SetEndGame();
			});
		this->m_EndUI.SetActive(false);

		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->SetGuideFlip();

		Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_TitleBGMID)->Play(DX_PLAYTYPE_LOOP, TRUE);
	}
	void Update_Sub(void) noexcept override {
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->ChangeGuide(
			[]() {
				auto* Localize = Util::LocalizePool::Instance();
				auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Esc), Localize->Get(340));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::UP), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::DOWN), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::LEFT), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::RIGHT), Localize->Get(332));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Diside), Localize->Get(330));
			}
		);
		DxLib::SetMouseDispFlag(true);
		this->m_EndUI.Update();
		if (this->m_EndUI.IsActive()) { return; }
		this->m_TitleUI.SetActive(!this->m_OptionWindow.IsActive() && !m_IsActiveCredit);
		this->m_TitleUI.Update();
		if (this->m_TitleUI.IsEnd()) {
			this->m_OptionWindow.SetActive(false);
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
		if (this->m_TitleUI.IsActive() || this->m_OptionWindow.IsActive()) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Esc)) {
				//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
				this->m_EndUI.SetActive(true);
			}
		}
		if (m_IsActiveCredit) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Diside)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
				m_IsActiveCredit = false;
			}
		}
		this->m_OptionWindow.Update();
	}
	void BGDraw_Sub(void) noexcept override {}
	void SetShadowDrawRigid_Sub(void) noexcept override {}
	void SetShadowDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) noexcept override {}
	void DepthDraw_Sub(void) noexcept override {}
	void ShadowDrawFar_Sub(void) noexcept override {}
	void ShadowDraw_Sub(void) noexcept override {}
	void UIDraw_Sub(void) noexcept override {
		this->m_TitleUI.Draw();
		this->m_OptionWindow.Draw();
		this->m_EndUI.Draw();
		if (m_IsActiveCredit) {
			{
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				DxLib::DrawBox(0, 0, 1920, 1080, ColorPalette::Black, true);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			this->m_CreditControl->Draw(1920 / 2 - 640 * 3 / 4, 1080 / 2 - 320, 1920 / 2 + 640 * 3 / 4);
		}
	}
	void Dispose_Sub(void) noexcept override {
		this->m_TitleUI.Dispose();
		this->m_OptionWindow.Dispose();
		this->m_EndUI.Dispose();
		this->m_CreditControl.reset();

		Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_TitleBGMID)->StopAll();
	}
};
