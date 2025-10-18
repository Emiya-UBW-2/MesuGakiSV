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
#include "../MainScene/EarlyCharacter.hpp"
#include "../MainScene/Character.hpp"
#include "../MainScene/Gun.hpp"

class MainScene : public Util::SceneBase {
	OptionWindow	m_OptionWindow;
	PauseUI			m_PauseUI;
	InfoType		m_EntrancePoint{ InfoType::Entrance1 };
	bool			m_Exit{ false };
	bool			m_IsSceneEnd{ false };
	bool			m_IsPauseActive{ false };
	char		padding[5]{};
	std::shared_ptr<EarlyCharacter>		m_EarlyCharacter{};
	std::shared_ptr<Character>		m_Character{};
	std::shared_ptr<Gun>			m_HandGun{};
	std::shared_ptr<Gun>			m_MainGun{};

	Util::VECTOR3D	m_CamOffset{};
	Util::VECTOR3D	m_CamVec{};
	std::string		m_MapName{ "Map1" };
	float			m_FPSPer{ 0.f };
	float			m_CamCheckLen{};
	float			m_CamCheckTimer{};
	float			m_Fade{ 1.f };

	float			m_ShotFov{ 0.f };

	Sound::SoundUniqueID OKID{ InvalidID };
	Sound::SoundUniqueID EnviID{ InvalidID };
	char		padding2[4]{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override {
		ObjectManager::Create();
		BackGround::Create();
		BackGround::Instance()->Load(m_MapName.c_str());
		ObjectManager::Instance()->LoadModel("data/Early/");
		ObjectManager::Instance()->LoadModel("data/Soldier/");
		ObjectManager::Instance()->LoadModel("data/Px4/");
		ObjectManager::Instance()->LoadModel("data/Cx4/");
	}
	void Init_Sub(void) noexcept override {
		BackGround::Instance()->Init();

		this->m_EarlyCharacter = std::make_shared<EarlyCharacter>();
		ObjectManager::Instance()->InitObject(this->m_EarlyCharacter, this->m_EarlyCharacter, "data/Early/");

		this->m_Character = std::make_shared<Character>();
		ObjectManager::Instance()->InitObject(this->m_Character, this->m_Character, "data/Soldier/");

		this->m_MainGun = std::make_shared<Gun>();
		this->m_HandGun = std::make_shared<Gun>();
		ObjectManager::Instance()->InitObject(this->m_MainGun, this->m_MainGun, "data/Cx4/");
		ObjectManager::Instance()->InitObject(this->m_HandGun, this->m_HandGun, "data/Px4/");

		this->m_Character->SetMainGunUniqueID(this->m_MainGun->GetObjectID());
		this->m_Character->SetSubGunUniqueID(this->m_HandGun->GetObjectID());

		for (auto& m : BackGround::Instance()->GetMapInfo()) {
			if (m.m_InfoType == m_EntrancePoint) {
				this->m_Character->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
			}
		}

		for (auto& m : BackGround::Instance()->GetMapInfo()) {
			if (m.m_InfoType == InfoType::WayPoint) {
				this->m_EarlyCharacter->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
				break;
			}
		}
		m_Exit = false;
		m_Fade = 1.f;

		OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);
		EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

		Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.3f, -0.7f, 0.3f).normalized();

		auto* PostPassParts = Draw::PostPassEffect::Instance();
		PostPassParts->SetShadowScale(0.5f);
		PostPassParts->SetAmbientLight(LightVec);

		SetLightEnable(false);
		auto* LightParts = Draw::LightPool::Instance();
		auto& FirstLight = LightParts->Put(Draw::LightType::DIRECTIONAL, LightVec);
		SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.f, 1.f, 1.f, 1.0f));
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

		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);
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
					//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Jump), Localize->Get(312));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(336));
					KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(337));
					//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::ChangeWeapon), Localize->Get(315));
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
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
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
			DxLib::SetMouseDispFlag(true);
			return;
		}
		ObjectManager::Instance()->UpdateObject();
		//更新
		auto* DrawerMngr = Draw::MainDraw::Instance();
		float XPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionX() - DrawerMngr->GetDispWidth() / 2) / static_cast<float>(DrawerMngr->GetDispWidth() / 2), -1.f, 1.f);
		float YPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionY() - DrawerMngr->GetDispHeight() / 2) / static_cast<float>(DrawerMngr->GetDispHeight() / 2), -1.f, 1.f);

		Util::VECTOR3D CamPosition;
		Util::VECTOR3D CamTarget;

		m_FPSPer = std::clamp(m_FPSPer + (this->m_Character->IsFPSView() ? 1.f : -1.f) / 60.f / 0.25f, 0.f, 1.f);

		Util::VECTOR3D CamPosition1;
		Util::VECTOR3D CamTarget1;
		Util::VECTOR3D CamPosition2;
		Util::VECTOR3D CamTarget2;
		if (m_FPSPer != 0.f) {
			BackGround::Instance()->SettingChange(3, 1);
			Util::Matrix4x4 EyeMat = this->m_Character->GetEyeMat();
			CamPosition1 = EyeMat.pos();
			CamTarget1 = CamPosition1 + EyeMat.zvec() * (-10.f * Scale3DRate);
		}
		if (m_FPSPer != 1.f) {
			BackGround::Instance()->SettingChange(1, 0);
			float Length = (Scale3DRate * 5.f);
			if (this->m_Character->IsFreeView()) {
				this->m_CamOffset = Util::Lerp(this->m_CamOffset, Util::VECTOR3D::vget(XPer * 3.f, 0.f, -YPer * 2.f), 1.f - 0.8f);
				Length = (Scale3DRate * 5.f);
			}
			else {
				this->m_CamOffset = Util::Lerp(this->m_CamOffset, Util::VECTOR3D::zero(), 1.f - 0.8f);
				Length = (Scale3DRate * 3.f);
			}
			Util::VECTOR3D CamPos = this->m_Character->GetMat().pos() + (Util::VECTOR3D::vget(0.f, 1.f, 0.f) + this->m_CamOffset) * Scale3DRate;
			Util::VECTOR3D CamVec = Util::VECTOR3D::vget(0, 5.f, -3.f).normalized() * Length;
			{
				Util::VECTOR3D Target = CamPos + CamVec.normalized() * (Scale3DRate * 5.f);
				Util::VECTOR3D Base = CamPos + CamVec.normalized() * (Scale3DRate * 3.75f);
				if (BackGround::Instance()->CheckLine(Base, &Target)) {
					m_CamCheckTimer = 0.f;
					m_CamCheckLen = std::clamp((Target - Base).magnitude() - 1.5f * Scale3DRate, 1.5f * Scale3DRate, Length);
				}
				else {
					m_CamCheckTimer = std::min(m_CamCheckTimer + 1.f / 60.f, 0.5f);
					if (m_CamCheckTimer >= 0.5f) {
						m_CamCheckLen = CamVec.magnitude();
					}
				}
				Target = Base + (Target - Base).normalized() * m_CamCheckLen;
				CamVec = Target - CamPos;
			}
			this->m_CamVec = Util::Lerp(this->m_CamVec, CamVec, 1.f - 0.9f);
			CamPosition2 = CamPos + this->m_CamVec;
			CamTarget2 = CamPos;
		}

		CamPosition = Util::Lerp(CamPosition2, CamPosition1, m_FPSPer);
		CamTarget = Util::Lerp(CamTarget2, CamTarget1, m_FPSPer);

		/*
		{
			Util::VECTOR3D EyeMat = this->m_Character->GetEyeMat().pos() + Util::VECTOR3D::up() * (-0.25f * Scale3DRate);
			CamPosition = EyeMat + Util::VECTOR3D::forward() * (-1.5f * Scale3DRate);
			CamTarget = EyeMat;
		}
		//*/

		auto* CameraParts = Camera::Camera3D::Instance();
		CameraParts->SetCamPos(CamPosition, CamTarget, Util::VECTOR3D::vget(0, 1.f, 0));

		if (this->m_Character->IsShotSwitch()) {
			m_ShotFov = 1.f;
		}
		else {
			m_ShotFov = Util::Lerp(m_ShotFov, 0.f, 1.f - 0.9f);
		}

		this->m_EarlyCharacter->SetTarget(this->m_Character->GetEyeMat().pos());

		DxLib::SetMouseDispFlag(!this->m_Character->IsFPSView());

		CameraParts->SetCamInfo(Util::Lerp(Util::deg2rad(45),
			CameraParts->GetCamera().GetCamFov() - m_ShotFov* Util::deg2rad(5),
			m_FPSPer), CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());
		this->m_Character->SetIsActive(!m_Exit);

		BackGround::Instance()->Update();

		m_Fade = std::clamp(m_Fade + (m_Exit ? 1.f : -1.f) / 60.f, 0.f, 1.f);
		if (!m_Exit) {
			for (auto& m : BackGround::Instance()->GetMapInfo()) {
				if (m.m_InfoType == InfoType::None || m.m_InfoType == InfoType::Max) { continue; }
				Util::VECTOR3D Vec = BackGround::Instance()->GetWorldPos(m.m_pos) - this->m_Character->GetMat().pos();
				float Len = 0.125f * Scale3DRate;
				if (std::fabsf(Vec.y) >= Len) { continue; }
				Vec.y = 0.f;
				if (Vec.sqrMagnitude() >= (Len + 0.35f * Scale3DRate) * (Len + 0.35f * Scale3DRate)) { continue; }
				switch (m.m_InfoType) {
				case InfoType::Exit1:
					m_EntrancePoint = InfoType::Entrance1;
					m_MapName = "Map1";
					m_Exit = true;
					break;
				case InfoType::Exit2:
					m_EntrancePoint = InfoType::Entrance2;
					m_MapName = "Map1";
					m_Exit = true;
					break;
				case InfoType::Exit3:
					break;
				case InfoType::Entrance1:
				case InfoType::Entrance2:
				case InfoType::Entrance3:
				case InfoType::None:
				case InfoType::WayPoint:
				case InfoType::WayPoint2:
				case InfoType::Max:
				default:
					break;
				}
			}
		}
		else {
			if (m_Fade >= 1.f) {
				SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
				Util::SceneBase::SetEndScene();
			}
		}
	}
	void BGDraw_Sub(void) noexcept override {
		BackGround::Instance()->BGDraw();
	}
	void SetShadowDrawRigid_Sub(void) noexcept override {
		BackGround::Instance()->SetShadowDrawRigid();
	}
	void SetShadowDraw_Sub(void) noexcept override {
		BackGround::Instance()->SetShadowDraw();
		ObjectManager::Instance()->Draw_SetShadow();
	}
	void Draw_Sub(void) noexcept override {
		SetVerticalFogEnable(true);
		SetVerticalFogMode(DX_FOGMODE_LINEAR);
		SetVerticalFogStartEnd(8.0f * Scale3DRate, 7.0f * Scale3DRate);
		SetVerticalFogColor(0, 0, 0);
		BackGround::Instance()->Draw();
		SetVerticalFogEnable(false);
		ObjectManager::Instance()->Draw();
	}
	void DepthDraw_Sub(void) noexcept override {
		ObjectManager::Instance()->Draw_Depth();
	}
	void ShadowDrawFar_Sub(void) noexcept override {
		BackGround::Instance()->ShadowDrawFar();
	}
	void ShadowDraw_Sub(void) noexcept override {
		BackGround::Instance()->ShadowDraw();
		ObjectManager::Instance()->Draw_Shadow();
	}
	void UIDraw_Sub(void) noexcept override {
		{
			int count = 0;
			{
				BG::Algorithm::Vector3Int Pos = BackGround::Instance()->GetVoxelPoint(this->m_Character->GetMat().pos());
				for (auto& m : BackGround::Instance()->GetMapGraph()) {
					if (m.m_ID <= Pos.y) {
						m.m_Per = std::clamp(m.m_Per + 1.f / 60.f, 0.f, 1.f);
						++count;
					}
					else {
						m.m_Per = std::clamp(m.m_Per - 1.f / 60.f, 0.f, 1.f);
					}
				}
			}
			if (count > 0) {
				int loop = 0;
				for (const auto& m : BackGround::Instance()->GetMapGraph()) {
					if (m.m_Per > 0.f) {
						++loop;
						int Bright = 255 * loop / count;
						DxLib::SetDrawBright(Bright, Bright, Bright);
						DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m.m_Per), 0, 255));
						m.m_Map.DrawRotaGraph(256, 256, 3.0f, 0.0f, true);
					}
					{
						BG::Algorithm::Vector3Int Pos = BackGround::Instance()->GetVoxelPoint(this->m_Character->GetMat().pos());
						if (m.m_ID <= Pos.y) {
							DxLib::SetDrawBright(255, 255, 255);
							DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

							DxLib::DrawCircle(
								256 + static_cast<int>(static_cast<float>(Pos.x * 128 / 256) * 3.f),
								256 + static_cast<int>(static_cast<float>(-Pos.z * 128 / 256) * 3.f),
								3, ColorPalette::Blue, TRUE);
						}
					}
					{
						BG::Algorithm::Vector3Int Pos = BackGround::Instance()->GetVoxelPoint(this->m_EarlyCharacter->GetMat().pos());
						if (m.m_ID <= Pos.y) {
							DxLib::SetDrawBright(255, 255, 255);
							DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

							DxLib::DrawCircle(
								256 + static_cast<int>(static_cast<float>(Pos.x * 128 / 256) * 3.f),
								256 + static_cast<int>(static_cast<float>(-Pos.z * 128 / 256) * 3.f),
								3, ColorPalette::Red, TRUE);
						}
					}
				}
				DxLib::SetDrawBright(255, 255, 255);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
		{
			auto* DrawerMngr = Draw::MainDraw::Instance();
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m_Fade), 0, 255));
			DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}

	void Dispose_Sub(void) noexcept override {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, EnviID)->StopAll();
		BackGround::Release();
		this->m_EarlyCharacter.reset();
		this->m_Character.reset();
		this->m_MainGun.reset();
		this->m_HandGun.reset();
		ObjectManager::Instance()->DeleteAll();
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
		ObjectManager::Release();
	}
};
