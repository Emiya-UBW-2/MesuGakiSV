#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"
#include "../MainScene/AmmoBox.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();
	PlayerManager::Create();
	BackGround::Create();
	BackGround::Instance()->Load(this->m_MapName.c_str());

	PlayerManager::Instance()->Load();
	ObjectManager::Instance()->LoadModel("data/model/Px4/");
	ObjectManager::Instance()->LoadModel("data/model/Cx4/");

	ObjectManager::Instance()->LoadModel("data/model/Cx4Scope/");
	ObjectManager::Instance()->LoadModel("data/model/Px4Sup/");

	m_StandGraph = Draw::GraphPool::Instance()->Get("data/Image/Body.png")->Get();
	m_SquatGraph = Draw::GraphPool::Instance()->Get("data/Image/BodyC.png")->Get();
	m_ProneGraph = Draw::GraphPool::Instance()->Get("data/Image/BodyP.png")->Get();
	m_Watch = Draw::GraphPool::Instance()->Get("data/Image/Watch.png")->Get();
	m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
	m_Lock = Draw::GraphPool::Instance()->Get("data/Image/Lock.png")->Get();

	AmmoBoxPool::Create();
}
void MainScene::Init_Sub(void) noexcept {
	BackGround::Instance()->Init();

	PlayerManager::Instance()->Init();

	this->m_MainGun = std::make_shared<Gun>();
	this->m_HandGun = std::make_shared<Gun>();
	ObjectManager::Instance()->InitObject(this->m_MainGun, this->m_MainGun, "data/model/Cx4/");
	ObjectManager::Instance()->InitObject(this->m_HandGun, this->m_HandGun, "data/model/Px4/");

	this->m_MainGunAttach = std::make_shared<Scope>();
	ObjectManager::Instance()->InitObject(this->m_MainGunAttach, this->m_MainGunAttach, "data/model/Cx4Scope/");

	this->m_HandGunAttach = std::make_shared<Suppressor>();
	ObjectManager::Instance()->InitObject(this->m_HandGunAttach, this->m_HandGunAttach, "data/model/Px4Sup/");

	this->m_MainGun->SetAttachScopeID(this->m_MainGunAttach->GetObjectID());
	this->m_HandGun->SetAttachSuppressorID(this->m_HandGunAttach->GetObjectID());

	auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));

	Player->SetMainGunUniqueID(this->m_MainGun->GetObjectID());
	Player->SetSubGunUniqueID(this->m_HandGun->GetObjectID());

	BackGround::Instance()->m_Offset = Util::VECTOR3D::vget(0.f, 0.f, 0.f) * Scale3DRate;
	
	for (auto& m : BackGround::Instance()->GetMapInfo()) {
		if (m.m_InfoType == InfoType::Entrance1) {
			Player->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
		}
	}
	//
	AmmoBoxPool::Instance()->Init();
	for (auto& m : BackGround::Instance()->GetMapInfo()) {
		if (m.m_InfoType == InfoType::AmmoBox) {
			AmmoBoxPool::Instance()->AddOne(BackGround::Instance()->GetWorldPos(m.m_pos));
		}
	}
	this->m_Exit = false;
	this->m_Fade = 1.f;

	this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);
	this->m_EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

	Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.9f, -0.5f, -0.3f).normalized();
	//Util::VECTOR3D LightVec = Util::VECTOR3D::vget(0.02f, -1.f, 0.02f).normalized();

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

	PostPassParts->SetGodRayPer(0.5f);

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

	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);
	m_IsResetMouse = true;
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
			if (!this->m_IsPauseActive) {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::W), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::A), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::D), Localize->Get(334));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Run), Localize->Get(308));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Jump), Localize->Get(312));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(336));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(338));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Q), "");
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::E), Localize->Get(335));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Walk), Localize->Get(309));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Squat), Localize->Get(310));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Prone), Localize->Get(311));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::ChangeWeapon), Localize->Get(315));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Reload), Localize->Get(316));
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
	auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));
	//
	CameraParts->SetCamInfo(Util::Lerp(Util::deg2rad(45),
		CameraParts->GetCamera().GetCamFov() - this->m_ShotFov * Util::Lerp(Util::deg2rad(5), Util::deg2rad(15), Player->GetADSPer()),
		this->m_FPSPer), CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());
	// 影をセット
	PostPassParts->SetShadowFarChange();
	//ポーズメニュー
	{
		if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Tab)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
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
		m_IsResetMouse = true;
		return;
	}

	if (m_IsResetMouse) {
		m_IsResetMouse = false;
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
	}

	AmmoBoxPool::Instance()->Update();

	ObjectManager::Instance()->UpdateObject();
	//更新
	auto* DrawerMngr = Draw::MainDraw::Instance();
	float XPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionX() - DrawerMngr->GetDispWidth() / 2) / static_cast<float>(DrawerMngr->GetDispWidth() / 2), -1.f, 1.f);
	float YPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionY() - DrawerMngr->GetDispHeight() / 2) / static_cast<float>(DrawerMngr->GetDispHeight() / 2), -1.f, 1.f);

	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;

	this->m_FPSPer = std::clamp(this->m_FPSPer + (Player->IsFPSView() ? 1.f : -1.f) * DeltaTime / 0.25f, 0.f, 1.f);

	Util::VECTOR3D CamPosition1;
	Util::VECTOR3D CamTarget1;
	Util::VECTOR3D CamPosition2;
	Util::VECTOR3D CamTarget2;
	if (this->m_FPSPer != 0.f) {
		BackGround::Instance()->SettingChange(1, 0);
		Util::Matrix4x4 EyeMat = Player->GetPlayerEyeMat();
		CamPosition1 = EyeMat.pos();
		CamTarget1 = CamPosition1 + EyeMat.zvec() * (-10.f * Scale3DRate);
	}
	if (this->m_FPSPer != 1.f) {
		BackGround::Instance()->SettingChange(1, 0);
		float Length = (Scale3DRate * 5.f);
		if (Player->IsFreeView()) {
			Util::Easing(&this->m_CamOffset, Util::VECTOR3D::vget(XPer * 3.f, 0.f, -YPer * 2.f), 0.8f);
			Length = (Scale3DRate * 5.f);
		}
		else {
			Util::Easing(&this->m_CamOffset, Util::VECTOR3D::zero(), 0.8f);
			Length = (Scale3DRate * 3.f);
		}
		Util::VECTOR3D CamPos = Player->GetMat().pos() + (Util::VECTOR3D::vget(0.f, 1.f, 0.f) + this->m_CamOffset) * Scale3DRate;
		Util::VECTOR3D CamVec = Util::VECTOR3D::vget(0, 5.f, -3.f).normalized() * Length;
		{
			Util::VECTOR3D Target = CamPos + CamVec.normalized() * (Scale3DRate * 5.f);
			Util::VECTOR3D Base = CamPos + CamVec.normalized() * (Scale3DRate * 3.75f);
			if (BackGround::Instance()->CheckLine(Base, &Target)) {
				this->m_CamCheckTimer = 0.f;
				this->m_CamCheckLen = std::clamp((Target - Base).magnitude() - 1.5f * Scale3DRate, 1.5f * Scale3DRate, Length);
			}
			else {
				this->m_CamCheckTimer = std::min(this->m_CamCheckTimer + DeltaTime, 0.5f);
				if (this->m_CamCheckTimer >= 0.5f) {
					this->m_CamCheckLen = CamVec.magnitude();
				}
			}
			Target = Base + (Target - Base).normalized() * this->m_CamCheckLen;
			CamVec = Target - CamPos;
		}
		Util::Easing(&this->m_CamVec, CamVec, 0.9f);
		CamPosition2 = CamPos + this->m_CamVec;
		CamTarget2 = CamPos;
	}

	CamPosition = Util::Lerp(CamPosition2, CamPosition1, this->m_FPSPer);
	CamTarget = Util::Lerp(CamTarget2, CamTarget1, this->m_FPSPer);

	/*
	{
		Util::VECTOR3D EyeMat = Player->GetEyeMatrix().pos() + Util::VECTOR3D::up() * (-0.25f * Scale3DRate);
		CamPosition = EyeMat + Util::VECTOR3D::forward() * (-1.5f * Scale3DRate);
		CamTarget = EyeMat;
	}
	//*/

	CameraParts->SetCamPos(CamPosition, CamTarget, Util::VECTOR3D::vget(0, 1.f, 0));

	if (Player->IsShotSwitch()) {
		this->m_ShotFov = 1.f;
	}
	else {
		Util::Easing(&m_ShotFov, 0.f, 0.8f);
	}

	DxLib::SetMouseDispFlag(!Player->IsFPSView() && !Player->IsFreeView());

	Player->SetIsActive(!m_Exit);

	BackGround::Instance()->Update();

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime / 0.1f, 0.f, 1.f);
	if (!m_Exit) {
		for (auto& m : BackGround::Instance()->GetMapInfo()) {
			if (m.m_InfoType == InfoType::None || m.m_InfoType == InfoType::Max) { continue; }
			Util::VECTOR3D Vec = BackGround::Instance()->GetWorldPos(m.m_pos) + BackGround::Instance()->m_Offset - Player->GetMat().pos();
			float Len = 0.125f * Scale3DRate;
			if (std::fabsf(Vec.y) >= Len) { continue; }
			Vec.y = 0.f;
			if (Vec.sqrMagnitude() >= (Len + 0.35f * Scale3DRate) * (Len + 0.35f * Scale3DRate)) { continue; }
			switch (m.m_InfoType) {
			case InfoType::Exit1:
				BackGround::Instance()->ChangeOffset();
				//this->m_EntrancePoint = InfoType::Entrance1;
				//this->m_MapName = "Map1";
				//this->m_Exit = true;
				break;
			case InfoType::Exit2:
				break;
			case InfoType::Exit3:
				break;
			case InfoType::Entrance1:
			case InfoType::Entrance2:
			case InfoType::Entrance3:
			case InfoType::None:
			case InfoType::WayPoint:
			case InfoType::WayPoint2:
			case InfoType::AmmoBox:
			case InfoType::Max:
			default:
				break;
			}
		}
	}
	else {
		if (this->m_Fade >= 1.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			Util::SceneBase::SetEndScene();
		}
	}

	{
		this->m_CharaStyleChange = std::max(this->m_CharaStyleChange - DeltaTime, 0.f);
		if (this->m_CharaStyle != Player->GetStyle()) {
			this->m_CharaStyleChange = 3.f;
		}
		Util::Easing(&this->m_CharaStyleChangeR, std::clamp(this->m_CharaStyleChange, 0.f, 1.f), 0.9f);
		this->m_CharaStyle = Player->GetStyle();
	}
	//
	{
		PostPassParts->SetScopeParam().m_IsActive = this->m_UseLens;
		PostPassParts->SetScopeParam().m_Radius = (this->m_LensSize - this->m_LensPos).magnitude();
		PostPassParts->SetScopeParam().m_Zoom = 4.f;
		PostPassParts->SetScopeParam().m_Xpos = this->m_LensPos.x;
		PostPassParts->SetScopeParam().m_Ypos = static_cast<float>(DrawerMngr->GetDispHeight()) - this->m_LensPos.y;

		PostPassParts->SetScopeParam().m_Radius = PostPassParts->SetScopeParam().m_Radius / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth()));
		PostPassParts->SetScopeParam().m_Xpos = PostPassParts->SetScopeParam().m_Xpos / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth()));
		PostPassParts->SetScopeParam().m_Ypos = PostPassParts->SetScopeParam().m_Ypos / (static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight()));
	}
	this->m_UseLens = false;
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
	AmmoBoxPool::Instance()->SetPos2D();
	{
		auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));
		if (Player->HasLens()) {
			auto* DrawerMngr = Draw::MainDraw::Instance();

			auto Pos = ConvWorldPosToScreenPos(Player->GetLensPos().pos().get());
			if (0.0f < Pos.z && Pos.z < 1.0f) {
				this->m_LensPos.x = Pos.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				this->m_LensPos.y = Pos.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
				this->m_UseLens |= true;
			}
			auto Size = ConvWorldPosToScreenPos(Player->GetLensSize().pos().get());
			if (0.0f < Size.z && Size.z < 1.0f) {
				this->m_LensSize.x = Size.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				this->m_LensSize.y = Size.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
				this->m_UseLens |= true;
			}
			/*
			SetUseLighting(false);
			DxLib::DrawBillboard3D(
				Player->GetLensPos().pos().get(),
				0.5f,
				0.5f,
				0.01f * Scale3DRate,
				0.f,
				Player->GetReticlePtr()->get(),
				true
			);
			SetUseLighting(true);
			//*/
		}
	}
	SetFogEnable(true);
	SetFogMode(DX_FOGMODE_LINEAR);
	SetFogStartEnd(20.0f * Scale3DRate, 50.0f * Scale3DRate);
	SetFogColor(0, 0, 0);

	SetVerticalFogEnable(true);
	SetVerticalFogMode(DX_FOGMODE_LINEAR);
	SetVerticalFogStartEnd(8.0f * Scale3DRate, 7.0f * Scale3DRate);
	SetVerticalFogColor(0, 0, 0);
	BackGround::Instance()->Draw();
	SetVerticalFogEnable(false);

	SetFogEnable(false);
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
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* Localize = Util::LocalizePool::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();

	auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));
	if (this->m_UseLens) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * Player->GetADSPer()), 0, 255));
		Player->GetReticlePtr()->DrawRotaGraph(static_cast<int>(this->m_LensPos.x), static_cast<int>(this->m_LensPos.y), 512.f / 256.f * Util::deg2rad(120) / CameraParts->GetCamera().GetCamFov(), 0.f, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	AmmoBoxPool::Instance()->DrawUI();
	{
		for (auto& c : PlayerManager::Instance()->GetCharacter()) {
			if (c->IsPlayer()) { continue; }
			auto& ec = ((std::shared_ptr<EarlyCharacter>&)c);
			if (ec->GetCanSeePer() == 0.f) { continue; }
			if (ec->GetDrugPer() == ec->GetDrugPerMax() * 2.f) { continue; }
			auto Pos = ec->GetUIPos();

			/*
			auto Len = std::clamp(
				(Pos - Util::VECTOR2D::vget(DrawerMngr->GetDispWidth() / 2, DrawerMngr->GetDispHeight() / 2)).magnitude() - 100.f,
				0.f, 500.f);
			//*/
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA,
				std::clamp(static_cast<int>(255.f *
					//(1.f - Len / 500.f)*
					ec->GetCanSeePer()
					), 0, 255));

			DrawLine(
				static_cast<int>(Pos.x), static_cast<int>(Pos.y),
				static_cast<int>(Pos.x) + 24, static_cast<int>(Pos.y) - 24,
				ColorPalette::Green, 3);

			DrawBox(
				static_cast<int>(Pos.x) + 24, static_cast<int>(Pos.y) - 24 - 10,
				static_cast<int>(Pos.x) + 24 + static_cast<int>(64.f * std::clamp(ec->GetDrugPer(), 0.f, ec->GetDrugPerMax()) / ec->GetDrugPerMax()), static_cast<int>(Pos.y) - 24,
				ColorPalette::Yellow, true);

			DrawBox(
				static_cast<int>(Pos.x) + 24, static_cast<int>(Pos.y) - 24 - 10,
				static_cast<int>(Pos.x) + 24 + static_cast<int>(64.f * std::clamp(ec->GetDrugPer() - ec->GetDrugPerMax(), 0.f, ec->GetDrugPerMax()) / ec->GetDrugPerMax()), static_cast<int>(Pos.y) - 24,
				ColorPalette::Red, true);

			DrawBox(
				static_cast<int>(Pos.x) + 24, static_cast<int>(Pos.y) - 24 - 10,
				static_cast<int>(Pos.x) + 24 + 64, static_cast<int>(Pos.y) - 24,
				ColorPalette::Green, false, 3);
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	{
		int xpos = DrawerMngr->GetDispWidth() / 2;
		int ypos = DrawerMngr->GetDispHeight() * 3 / 4;

		if (Player->CanArmlock()) {
			KeyGuideParts->DrawButton(xpos - 24 / 2, ypos - 24 / 2, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::MIDDLE, Draw::FontYCenter::TOP,
				xpos, ypos + 18,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(341)));
			ypos += 52;
		}

		if (Player->CanArmlockInjector()) {
			KeyGuideParts->DrawButton(xpos - 24 / 2, ypos - 24 / 2, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::MIDDLE, Draw::FontYCenter::TOP,
				xpos, ypos + 18,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(342)));
			ypos += 52;
		}

		if (Player->NeedReload()) {
			KeyGuideParts->DrawButton(xpos - 24 / 2, ypos - 24 / 2, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Reload));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::MIDDLE, Draw::FontYCenter::TOP,
				xpos, ypos + 18,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(316)));
			ypos += 52;
		}
	}
	{
		int xpos = DrawerMngr->GetDispWidth() / 2;
		int ypos = DrawerMngr->GetDispHeight() * 7 / 8;

		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_CharaStyleChangeR), 64, 255));
		{
			DxLib::SetDrawBright(0, 255, 0);
			switch (Player->GetStyle()) {
			case CharaStyle::Run:
			case CharaStyle::Stand:
				m_StandGraph->DrawRotaGraph(xpos, ypos, 128.f / 500.f, 0.f, true);
				break;
			case CharaStyle::Squat:
				m_SquatGraph->DrawRotaGraph(xpos, ypos, 128.f / 500.f, 0.f, true);
				break;
			case CharaStyle::Prone:
				m_ProneGraph->DrawRotaGraph(xpos, ypos, 128.f / 500.f, 0.f, true);
				break;
			case CharaStyle::Max:
			default:
				break;
			}
			DxLib::SetDrawBright(255, 255, 255);
		}
		{
			xpos = DrawerMngr->GetDispWidth() / 2 - 64;
			ypos = DrawerMngr->GetDispHeight() * 7 / 8 - 32 + 24 / 2;

			KeyGuideParts->DrawButton(xpos - 24, ypos - 24, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Squat));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::RIGHT, Draw::FontYCenter::BOTTOM,
				xpos - 24, ypos,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(310)));

			xpos = DrawerMngr->GetDispWidth() / 2 - 64;
			ypos = DrawerMngr->GetDispHeight() * 7 / 8 + 32 + 24 / 2;

			KeyGuideParts->DrawButton(xpos - 24, ypos - 24, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Prone));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::RIGHT, Draw::FontYCenter::BOTTOM,
				xpos - 24, ypos,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(311)));
		}

		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	this->m_PauseUI.Draw();
	this->m_OptionWindow.Draw();
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Fade), 0, 255));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}
void MainScene::Dispose_Sub(void) noexcept {
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->StopAll();
	BackGround::Release();
	PlayerManager::Release();
	AmmoBoxPool::Release();
	this->m_MainGun.reset();
	this->m_HandGun.reset();
	this->m_HandGunAttach.reset();
	this->m_MainGunAttach.reset();
	ObjectManager::Instance()->DeleteAll();
	this->m_PauseUI.Dispose();
	this->m_OptionWindow.Dispose();
	ObjectManager::Release();
}
