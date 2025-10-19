#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();
	BackGround::Create();
	BackGround::Instance()->Load(m_MapName.c_str());
	ObjectManager::Instance()->LoadModel("data/Early/");
	ObjectManager::Instance()->LoadModel("data/Soldier/");
	ObjectManager::Instance()->LoadModel("data/Px4/");
	ObjectManager::Instance()->LoadModel("data/Cx4/");
}
void MainScene::Init_Sub(void) noexcept {
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

	cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
	OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);
	EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

	this->m_EquipUI.emplace_back();
	this->m_EquipUI.back().Init(&this->m_MainGun);
	this->m_EquipUI.emplace_back();
	this->m_EquipUI.back().Init(&this->m_HandGun);

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
void MainScene::Update_Sub(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();

	KeyGuideParts->ChangeGuide(
		[this]() {
			auto* Localize = Util::LocalizePool::Instance();
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			if (m_IsChangeEquip) {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::W), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), Localize->Get(332));
			}
			else if (!this->m_IsPauseActive) {
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
	//
	CameraParts->SetCamInfo(Util::Lerp(Util::deg2rad(45),
		CameraParts->GetCamera().GetCamFov() - m_ShotFov * Util::deg2rad(5),
		m_FPSPer), CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());
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
	if (!this->m_Character->GetIsReloading()) {
		if (KeyMngr->GetBattleKeyReleaseTrigger(Util::EnumBattle::E)) {
			KeyGuideParts->SetGuideFlip();
			if ((this->m_EquipUITimer >= 10.f * DeltaTime) || (this->m_Character->GetEquip() == InvalidID)) {
				this->m_Character->SetEquip(m_EquipID);
			}
			else {
				this->m_Character->SetEquip(InvalidID);
			}
		}
		bool IsChangeEquip = KeyMngr->GetBattleKeyPress(Util::EnumBattle::E);
		if (IsChangeEquip) {
			if (IsChangeEquip != m_IsChangeEquip) {
				KeyGuideParts->SetGuideFlip();
			}
			float Prev = this->m_EquipUITimer;
			this->m_EquipUITimer = std::clamp(this->m_EquipUITimer + DeltaTime, 0.f, 10.f * DeltaTime);
			if (this->m_EquipUITimer >= 10.f * DeltaTime) {
				if (Prev < 10.f * DeltaTime) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
				}
				Util::Easing(&m_EquipPer, 0.f, 0.9f);
				if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::W)) {
					--m_EquipID;
					if (m_EquipID < 0) { m_EquipID = static_cast<int>(this->m_EquipUI.size()) - 1; }
					m_EquipPer -= 1.f;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
				}
				if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::S)) {
					++m_EquipID;
					if (m_EquipID > static_cast<int>(this->m_EquipUI.size()) - 1) { m_EquipID = 0; }
					m_EquipPer += 1.f;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}
		else {
			this->m_EquipUITimer = 0;
			m_EquipPer = 0.f;
		}
		m_IsChangeEquip = IsChangeEquip;
	}
	else {
		this->m_EquipUITimer = 0;
		m_EquipPer = 0.f;
		m_IsChangeEquip = false;
	}
	if (this->m_EquipUITimer >= 10.f * DeltaTime) {
		m_EquipUIActivePer = std::clamp(m_EquipUIActivePer + DeltaTime / 0.1f, 0.f, 1.f);
		DxLib::SetMouseDispFlag(true);
		return;
	}
	else {
		m_EquipUIActivePer = std::clamp(m_EquipUIActivePer - DeltaTime / 0.1f, 0.f, 1.f);
	}

	ObjectManager::Instance()->UpdateObject();
	//更新
	auto* DrawerMngr = Draw::MainDraw::Instance();
	float XPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionX() - DrawerMngr->GetDispWidth() / 2) / static_cast<float>(DrawerMngr->GetDispWidth() / 2), -1.f, 1.f);
	float YPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionY() - DrawerMngr->GetDispHeight() / 2) / static_cast<float>(DrawerMngr->GetDispHeight() / 2), -1.f, 1.f);

	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;

	m_FPSPer = std::clamp(m_FPSPer + (this->m_Character->IsFPSView() ? 1.f : -1.f) * DeltaTime / 0.25f, 0.f, 1.f);

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
			Util::Easing(&this->m_CamOffset, Util::VECTOR3D::vget(XPer * 3.f, 0.f, -YPer * 2.f), 0.8f);
			Length = (Scale3DRate * 5.f);
		}
		else {
			Util::Easing(&this->m_CamOffset, Util::VECTOR3D::zero(), 0.8f);
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
				m_CamCheckTimer = std::min(m_CamCheckTimer + DeltaTime, 0.5f);
				if (m_CamCheckTimer >= 0.5f) {
					m_CamCheckLen = CamVec.magnitude();
				}
			}
			Target = Base + (Target - Base).normalized() * m_CamCheckLen;
			CamVec = Target - CamPos;
		}
		Util::Easing(&this->m_CamVec, CamVec, 0.9f);
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

	CameraParts->SetCamPos(CamPosition, CamTarget, Util::VECTOR3D::vget(0, 1.f, 0));

	if (this->m_Character->IsShotSwitch()) {
		m_ShotFov = 1.f;
	}
	else {
		Util::Easing(&m_ShotFov, 0.f, 0.9f);
	}

	this->m_EarlyCharacter->SetTarget(this->m_Character->GetEyeMat().pos());

	DxLib::SetMouseDispFlag(!this->m_Character->IsFPSView());

	this->m_Character->SetIsActive(!m_Exit);

	BackGround::Instance()->Update();

	m_Fade = std::clamp(m_Fade + (m_Exit ? 1.f : -1.f) * DeltaTime, 0.f, 1.f);
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
void MainScene::BGDraw_Sub(void) noexcept {
	BackGround::Instance()->BGDraw();
}
void MainScene::SetShadowDrawRigid_Sub(void) noexcept {
	BackGround::Instance()->SetShadowDrawRigid();
}
void MainScene::SetShadowDraw_Sub(void) noexcept {
	BackGround::Instance()->SetShadowDraw();
	ObjectManager::Instance()->Draw_SetShadow();
}
void MainScene::Draw_Sub(void) noexcept {
	SetVerticalFogEnable(true);
	SetVerticalFogMode(DX_FOGMODE_LINEAR);
	SetVerticalFogStartEnd(8.0f * Scale3DRate, 7.0f * Scale3DRate);
	SetVerticalFogColor(0, 0, 0);
	BackGround::Instance()->Draw();
	SetVerticalFogEnable(false);
	ObjectManager::Instance()->Draw();
}
void MainScene::DepthDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Depth();
}
void MainScene::ShadowDrawFar_Sub(void) noexcept {
	BackGround::Instance()->ShadowDrawFar();
}
void MainScene::ShadowDraw_Sub(void) noexcept {
	BackGround::Instance()->ShadowDraw();
	ObjectManager::Instance()->Draw_Shadow();
}
void MainScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	{
		int count = 0;
		{
			BG::Algorithm::Vector3Int Pos = BackGround::Instance()->GetVoxelPoint(this->m_Character->GetMat().pos());
			for (auto& m : BackGround::Instance()->GetMapGraph()) {
				if (m.GetID() <= Pos.y) {
					m.m_Per = std::clamp(m.m_Per + DeltaTime, 0.f, 1.f);
					++count;
				}
				else {
					m.m_Per = std::clamp(m.m_Per - DeltaTime, 0.f, 1.f);
				}
			}
		}
		if (count > 0) {
			int loop = 0;
			for (const auto& m : BackGround::Instance()->GetMapGraph()) {
				if (m.GetPer() > 0.f) {
					++loop;
					int Bright = 255 * loop / count;
					DxLib::SetDrawBright(Bright, Bright, Bright);
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m.GetPer()), 0, 255));
					m.GetMap()->DrawRotaGraph(256, 256, 3.0f, 0.0f, true);
				}
				{
					BG::Algorithm::Vector3Int Pos = BackGround::Instance()->GetVoxelPoint(this->m_Character->GetMat().pos());
					if (m.GetID() <= Pos.y) {
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
					if (m.GetID() <= Pos.y) {
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
	if ((this->m_Character->GetEquip() != InvalidID) || (m_EquipUIActivePer > 0.f)) {
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(64.f * m_EquipUIActivePer), 0, 255));
			DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}

		int xpos = DrawerMngr->GetDispWidth() - 256 - 64;
		int ypos = DrawerMngr->GetDispHeight() - 128 - 64;
		for (int loop = 1; loop <= 3; ++loop) {
			int ID = (loop + 4 + m_EquipID) % static_cast<int>(this->m_EquipUI.size());
			int Y = ypos + static_cast<int>(static_cast<float>(128 + 16) * m_EquipUIActivePer) * loop + static_cast<int>(static_cast<float>(128 + 16) * m_EquipPer);
			auto& d = this->m_EquipUI.at(static_cast<size_t>(ID));
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255 - (std::abs(Y - ypos))), 0, 255));
			d.Draw(xpos, Y);
		}
		for (int loop = -3; loop <= 0; ++loop) {
			int ID = (loop + 4 + m_EquipID) % static_cast<int>(this->m_EquipUI.size());
			int Y = ypos + static_cast<int>(static_cast<float>(128 + 16) * m_EquipUIActivePer) * loop + static_cast<int>(static_cast<float>(128 + 16) * m_EquipPer);
			auto& d = this->m_EquipUI.at(static_cast<size_t>(ID));
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255 - (std::abs(Y - ypos))), 0, 255));
			d.Draw(xpos, Y);
		}
		DxLib::DrawBox(xpos, ypos, xpos + 256, ypos + 128, GetColor(255, 255, 0), false, 3);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	this->m_PauseUI.Draw();
	this->m_OptionWindow.Draw();
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m_Fade), 0, 255));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}
void MainScene::Dispose_Sub(void) noexcept {
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
