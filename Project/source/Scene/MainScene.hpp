#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"
#include "../MainScene/BackGround.hpp"
#include "../MainScene/Character.hpp"

class MainScene : public Util::SceneBase {
	OptionWindow	m_OptionWindow;
	PauseUI			m_PauseUI;
	bool			m_IsSceneEnd{ false };
	bool			m_IsPauseActive{ false };
	char		padding[6]{};

	Character		m_Character{};

	Util::VECTOR3D	m_CamOffset{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override {
		BackGround::Create();
		BackGround::Instance()->Load();
		this->m_Character.Load();
	}
	void Init_Sub(void) noexcept override {
		BackGround::Instance()->Init();
		this->m_Character.Init();

		Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.1f, -1.f, 0.1f).normalized();

		auto* PostPassParts = Draw::PostPassEffect::Instance();
		PostPassParts->SetShadowScale(0.5f);
		PostPassParts->SetAmbientLight(LightVec);

		SetLightEnable(false);
		auto* LightParts = Draw::LightPool::Instance();
		auto& FirstLight = LightParts->Put(Draw::LightType::DIRECTIONAL, LightVec);
		SetLightAmbColorHandle(FirstLight.get(), GetColorF(0.25f, 0.25f, 0.25f, 1.0f));
		SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

		//DoF
		PostPassParts->SetDoFNearFar(
			(Scale3DRate * 0.15f), Scale3DRate * 5.0f,
			(Scale3DRate * 0.05f), Scale3DRate * 30.0f);

		this->m_IsSceneEnd = false;
		this->m_IsPauseActive = false;

		this->m_OptionWindow.Init();
		this->m_PauseUI.Init();

		this->m_PauseUI.SetEvent(0, [this]() {
			this->m_IsSceneEnd = true;
			this->m_IsPauseActive = false;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
			});
		this->m_PauseUI.SetEvent(1, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_PauseUI.SetEvent(2, [this]() {
			this->m_IsPauseActive = false;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
			});

		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->SetGuideFlip();
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		auto* PostPassParts = Draw::PostPassEffect::Instance();
		KeyGuideParts->ChangeGuide(
			[this]() {
				auto* Localize = Util::LocalizePool::Instance();
				auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
				if (!this->m_IsPauseActive) {
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::W), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::A), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::D), Localize->Get(334));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Q), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::E), Localize->Get(335));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Run), Localize->Get(308));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Walk), Localize->Get(309));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Squat), Localize->Get(310));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Prone), Localize->Get(311));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Jump), Localize->Get(312));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(336));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(337));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::ChangeWeapon), Localize->Get(315));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Reload), Localize->Get(316));
				}
				else {
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::UP), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::DOWN), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::LEFT), "");
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::RIGHT), Localize->Get(332));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Diside), Localize->Get(330));
				}
			}
		);
		// 影をセット
		PostPassParts->SetShadowFarChange();
		//ポーズメニュー
		{
			if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Tab)) {
				this->m_IsPauseActive ^= 1;
				KeyGuideParts->SetGuideFlip();
			}
			this->m_PauseUI.SetActive(this->m_IsPauseActive && !this->m_OptionWindow.IsActive());
			if (!this->m_IsPauseActive) {
				this->m_OptionWindow.SetActive(false);
			}
			this->m_PauseUI.Update();
			if (this->m_IsSceneEnd && this->m_PauseUI.IsEnd()) {
				SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
				SceneBase::SetEndScene();
			}
			this->m_OptionWindow.Update();
		}
		if (this->m_IsPauseActive) {
			return;
		}
		//更新
		auto* DrawerMngr = Draw::MainDraw::Instance();
		float XPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionX() - DrawerMngr->GetDispWidth() / 2) / static_cast<float>(DrawerMngr->GetDispWidth() / 2), -1.f, 1.f);
		float YPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionY() - DrawerMngr->GetDispHeight() / 2) / static_cast<float>(DrawerMngr->GetDispHeight() / 2), -1.f, 1.f);

		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim)) {
			this->m_CamOffset = Util::Lerp(this->m_CamOffset, Util::VECTOR3D::vget(XPer * 3.f, 0.f, -YPer * 2.f), 1.f - 0.8f);
		}
		else {
			this->m_CamOffset = Util::Lerp(this->m_CamOffset, Util::VECTOR3D::zero(), 1.f - 0.8f);
		}

		/*
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::A)) {
			m_YRad += Util::deg2rad(60.f) / 60.f;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::D)) {
			m_YRad -= Util::deg2rad(60.f) / 60.f;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::W)) {
			m_XRad += Util::deg2rad(60.f) / 60.f;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::S)) {
			m_XRad -= Util::deg2rad(60.f) / 60.f;
		}
		m_XRad = std::clamp(m_XRad, Util::deg2rad(-60.f), Util::deg2rad(60.f));
		//*/
		float Per = this->m_Character.GetSpeed() / this->m_Character.GetSpeedMax();
		Util::VECTOR3D CamPos = this->m_Character.GetMat().pos() + (Util::VECTOR3D::vget(0.f, 1.f, 0.f) + this->m_CamOffset) * Scale3DRate;
		Util::VECTOR3D CamVec = Util::VECTOR3D::vget(0, 3.5f, -2.f) * Scale3DRate * Util::Lerp(1.25f, 1.f, Per);

		auto* CameraParts = Camera::Camera3D::Instance();
		CameraParts->SetCamPos(CamPos + CamVec, CamPos, Util::VECTOR3D::vget(0, 1.f, 0));
		CameraParts->SetCamInfo(Util::deg2rad(45), 0.5f, Scale3DRate * 30.0f);

		BackGround::Instance()->Update();
		this->m_Character.Update();
	}
	void BGDraw_Sub(void) noexcept override {
		BackGround::Instance()->BGDraw();
	}
	void SetShadowDrawRigid_Sub(void) noexcept override {
		BackGround::Instance()->SetShadowDrawRigid();
	}
	void SetShadowDraw_Sub(void) noexcept override {
		BackGround::Instance()->SetShadowDraw();
		this->m_Character.SetShadowDraw();
	}
	void Draw_Sub(void) noexcept override {
		BackGround::Instance()->Draw();
		this->m_Character.Draw();
	}
	void ShadowDrawFar_Sub(void) noexcept override {
		BackGround::Instance()->ShadowDrawFar();
	}
	void ShadowDraw_Sub(void) noexcept override {
		BackGround::Instance()->ShadowDraw();
		this->m_Character.ShadowDraw();
	}
	void UIDraw_Sub(void) noexcept override {
		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
	}

	void Dispose_Sub(void) noexcept override {
		BackGround::Release();
		this->m_Character.Dispose();
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
