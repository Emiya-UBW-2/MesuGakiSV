#include "MainScene.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();

	m_BackScreen = Draw::GraphPool::Instance()->Get("data/Image/BackScreen.png")->Get();
	m_ReimuStay = Draw::GraphPool::Instance()->Get("data/Image/stay00.png")->Get();
	m_ReimuLeft = Draw::GraphPool::Instance()->Get("data/Image/left00.png")->Get();
	m_ReimuRight = Draw::GraphPool::Instance()->Get("data/Image/right00.png")->Get();
	m_Ammo00 = Draw::GraphPool::Instance()->Get("data/Image/Ammo00.png")->Get();
	m_Ammo01 = Draw::GraphPool::Instance()->Get("data/Image/Ammo01.png")->Get();
	m_ReimuOption = Draw::GraphPool::Instance()->Get("data/Image/Option.png")->Get();
}
void MainScene::Init_Sub(void) noexcept {
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

	m_ReimuPos.SetPos().m_Pos = Util::VECTOR2D::vget(1920.f / 2.f, 1080.f - 128.f);

	m_StageScript.Load();
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
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(336));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(338));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Run), Localize->Get(308));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Walk), Localize->Get(309));
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
	CameraParts->SetCamInfo(CameraParts->GetCamera().GetCamFov(), CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());
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

	ObjectManager::Instance()->UpdateObject();
	//更新
	auto* DrawerMngr = Draw::MainDraw::Instance();

	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;

	Util::VECTOR3D CamPosition1;
	Util::VECTOR3D CamTarget1;
	Util::VECTOR3D CamPosition2;
	Util::VECTOR3D CamTarget2;
	Util::Matrix4x4 EyeMat;
	CamPosition1 = EyeMat.pos();
	CamTarget1 = CamPosition1 + EyeMat.zvec() * (-10.f * Scale3DRate);

	CamPosition = Util::Lerp(CamPosition2, CamPosition1, 1.f);
	CamTarget = Util::Lerp(CamTarget2, CamTarget1, 1.f);

	CameraParts->SetCamPos(CamPosition, CamTarget, Util::VECTOR3D::vget(0, 1.f, 0));

	DxLib::SetMouseDispFlag(true);

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime / 0.1f, 0.f, 1.f);
	if (!m_Exit) {
	}
	else {
		if (this->m_Fade >= 1.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			Util::SceneBase::SetEndScene();
		}
	}
	//
	m_Reimu = m_ReimuStay;

	bool IsSlow = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run);
	m_ReimuSlowPer = std::clamp(m_ReimuSlowPer + (IsSlow ? DeltaTime : -DeltaTime), 0.f, 1.f);
	if (IsSlow) {
		m_ReimuSpeed = 300.f * DeltaTime;
	}
	else {
		m_ReimuSpeed = 600.f*DeltaTime;
	}
	if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::A)) {
		m_Reimu = m_ReimuLeft;
		m_ReimuPos.SetPos().m_Pos.x -= m_ReimuSpeed;
	}
	if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::D)) {
		m_Reimu = m_ReimuRight;
		m_ReimuPos.SetPos().m_Pos.x += m_ReimuSpeed;
	}
	if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::W)) {
		m_ReimuPos.SetPos().m_Pos.y -= m_ReimuSpeed;
	}
	if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::S)) {
		m_ReimuPos.SetPos().m_Pos.y += m_ReimuSpeed;
	}

	m_ReimuPos.SetPos().m_Pos.x = std::clamp(m_ReimuPos.SetPos().m_Pos.x, static_cast<float>(1920 / 2 - 864 / 2 + 50), static_cast<float>(1920 / 2 + 864 / 2 - 50));
	m_ReimuPos.SetPos().m_Pos.y = std::clamp(m_ReimuPos.SetPos().m_Pos.y, static_cast<float>(36 + 50), static_cast<float>(1080 - 36 - 50));

	m_ReimuOpt0.SetPos().m_Pos = m_ReimuPos.SetPos().m_Pos + Util::Lerp(Util::VECTOR2D::vget(-50.f, 0.f), Util::VECTOR2D::vget(-20.f, -50.f), m_ReimuSlowPer);
	m_ReimuOpt1.SetPos().m_Pos = m_ReimuPos.SetPos().m_Pos + Util::Lerp(Util::VECTOR2D::vget(50.f, -0.f), Util::VECTOR2D::vget(20.f, -50.f), m_ReimuSlowPer);
	m_ReimuOpt0.SetPos().m_Rad += Util::deg2rad(360) * DeltaTime;
	m_ReimuOpt1.SetPos().m_Rad -= Util::deg2rad(360) * DeltaTime;

	{
		if (m_Ammo00ShotTimer <= 0.f) {
			if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
				m_Ammo00ShotTimer = 0.1f;
				if (IsSlow) {
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(-18.f, -10.f), Util::VECTOR2D::vget(-100.f, -1200.f));
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(-6.f, -10.f), Util::VECTOR2D::vget(-50.f, -1200.f));
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(6.f, -10.f), Util::VECTOR2D::vget(50.f, -1200.f));
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(18.f, -10.f), Util::VECTOR2D::vget(100.f, -1200.f));
				}
				else {
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(-18.f, -10.f), Util::VECTOR2D::vget(-200.f, -1200.f));
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(-6.f, -10.f), Util::VECTOR2D::vget(-100.f, -1200.f));
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(6.f, -10.f), Util::VECTOR2D::vget(100.f, -1200.f));
					SetAmmo00(m_ReimuPos.SetPos().m_Pos + Util::VECTOR2D::vget(18.f, -10.f), Util::VECTOR2D::vget(200.f, -1200.f));
				}
			}
		}
		else {
			m_Ammo00ShotTimer = std::max(m_Ammo00ShotTimer - DeltaTime, 0.f);
		}
		if (m_Ammo01ShotTimer <= 0.f) {
			if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
				m_Ammo01ShotTimer = 0.1f;
				if (IsSlow) {
					SetAmmo01(m_ReimuOpt0.SetPos().m_Pos, Util::VECTOR2D::vget(-1200.f, -1200.f));
					SetAmmo01(m_ReimuOpt1.SetPos().m_Pos, Util::VECTOR2D::vget(1200.f, -1200.f));
				}
				else {
					SetAmmo01(m_ReimuOpt0.SetPos().m_Pos, Util::VECTOR2D::vget(-1200.f, -1200.f));
					SetAmmo01(m_ReimuOpt1.SetPos().m_Pos, Util::VECTOR2D::vget(1200.f, -1200.f));
				}
			}
		}
		else {
			m_Ammo01ShotTimer = std::max(m_Ammo01ShotTimer - DeltaTime, 0.f);
		}
	}

	for (auto& a : m_Ammo00Pos) {
		if (!a.IsActive()) { continue; }
		a.UpdateAmmo();
		a.Update();
	}

	for (auto& a : m_Ammo01Pos) {
		if (!a.IsActive()) { continue; }
		a.UpdateAmmo();
		a.Update();
	}
	m_ReimuOpt0.Update();
	m_ReimuOpt1.Update();
	m_ReimuPos.Update();

	m_StageScript.Update();
}
void MainScene::BGDraw_Sub(void) noexcept {
}
void MainScene::SetShadowDrawRigid_Sub(void) noexcept {
}
void MainScene::SetShadowDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_SetShadow();
}
void MainScene::Draw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw();
}
void MainScene::DepthDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Depth();
}
void MainScene::ShadowDrawFar_Sub(void) noexcept {
}
void MainScene::ShadowDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Shadow();
}
void MainScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	/*
	DxLib::DrawBox(
		DrawerMngr->GetDispWidth() * (1920 / 2 - 864 / 2) / 1920, DrawerMngr->GetDispHeight() * 36 / 1080,
		DrawerMngr->GetDispWidth() * (1920 / 2 + 864 / 2) / 1920, DrawerMngr->GetDispHeight() * (1080 - 36) / 1080,
		ColorPalette::Black, true);
	//*/
	
	for (auto& a : m_Ammo00Pos) {
		a.Draw(m_Ammo00);
	}
	for (auto& a : m_Ammo01Pos) {
		a.Draw(m_Ammo01);
	}
	m_ReimuOpt0.Draw(m_ReimuOption);
	m_ReimuOpt1.Draw(m_ReimuOption);
	m_ReimuPos.Draw(m_Reimu);

	m_StageScript.Draw();

	m_BackScreen->DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), true);
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
	ObjectManager::Instance()->DeleteAll();
	this->m_PauseUI.Dispose();
	this->m_OptionWindow.Dispose();
	ObjectManager::Release();
}
