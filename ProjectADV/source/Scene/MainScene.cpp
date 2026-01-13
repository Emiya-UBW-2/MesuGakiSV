#pragma warning(disable:5259)

#include "MainScene.hpp"

#include "../MainScene/Others.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();

	if (m_IsSeek) {
		Seek();
	}
	m_IsSeek = true;
	this->m_SpeakScript.Load(("data/message" + std::to_string(m_NowPhase) + ".txt").c_str());
}
void MainScene::Init_Sub(void) noexcept {
	this->m_Exit = false;
	this->m_Fade = 1.f;

	this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

	Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.9f, -0.3f, 0.1f).normalized();

	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(1.f);
	PostPassParts->SetAmbientLight(LightVec);

	SetLightEnable(false);
	auto* LightParts = Draw::LightPool::Instance();
	auto& FirstLight = LightParts->Put(Draw::LightType::DIRECTIONAL, LightVec);
	SetLightAmbColorHandle(FirstLight.get(), GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
	SetLightDifColorHandle(FirstLight.get(), GetColorF(0.5f, 0.5f, 0.5f, 1.0f));

	//DoF
	PostPassParts->SetDoFNearFar(
		(Scale3DRate * 0.15f), Scale3DRate * 5.0f,
		(Scale3DRate * 0.05f), Scale3DRate * 30.0f);

	PostPassParts->SetGodRayPer(0.25f);

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
	this->m_PauseUI.SetEvent(3, [this]() {
		this->m_SaveUI.SetActive(true);
		});
	this->m_PauseUI.SetEvent(4, [this]() {
		this->m_ContinueUI.SetActive(true);
		});

	this->m_SaveUI.Init();
	this->m_SaveUI.SetEvent([this]() {
		//決定時の
		Param::CommonParam::Instance()->m_IsLoad = this->m_SaveUI.GetSelect();
		Save(Param::CommonParam::Instance()->m_IsLoad);
		this->m_SaveUI.SetActive(false);
		});
	this->m_SaveUI.SetActive(false);

	this->m_ContinueUI.Init();
	this->m_ContinueUI.SetEvent([this]() {
		//決定時の
		this->m_ContinueUI.SetActive(false);
		Param::CommonParam::Instance()->m_IsLoad = this->m_ContinueUI.GetSelect();

		SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
		Util::SceneBase::SetEndScene();
		this->m_IsPauseActive = false;
		});
	this->m_ContinueUI.SetActive(false);

	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	KeyGuideParts->SetGuideFlip();

	Start();
}
void MainScene::Update_Sub(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();

	KeyGuideParts->ChangeGuide(
		[this]() {
			auto* Localize = Util::LocalizePool::Instance();
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			if (!this->m_IsPauseActive) {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(302));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(303));
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
		this->m_PauseUI.SetActive(
			this->m_IsPauseActive
			&& !this->m_OptionWindow.IsActive()
			&& !this->m_SaveUI.IsActive()
			&& !this->m_ContinueUI.IsActive()
		);
		if (!this->m_IsPauseActive) {
			this->m_OptionWindow.SetActive(false);
			this->m_SaveUI.SetActive(false);
			this->m_ContinueUI.SetActive(false);
		}
		this->m_PauseUI.Update();
		if (this->m_IsSceneEnd && this->m_PauseUI.IsEnd()) {
			this->m_Exit = true;
		}
		this->m_OptionWindow.Update();
		this->m_SaveUI.Update();
		this->m_ContinueUI.Update();
	}
	if (this->m_IsPauseActive) {
		DxLib::SetMouseDispFlag(true);
		return;
	}

	//更新
	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;

	int mouseX = -std::clamp(m_PrevMouseX - DrawerMngr->GetMousePositionX(), -30, 30);
	Util::Easing(&m_MouseXR, static_cast<float>(mouseX), 0.995f);

	m_PrevMouseX = DrawerMngr->GetMousePositionX();

	CamPosition =
		Util::Matrix4x4::Vtrans(
			Util::VECTOR3D::vget(0.f, 30.f, -47.f),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(-135 + m_MouseXR))
		);
	CamTarget = Util::VECTOR3D::vget(0.f, 15.f, 0.f);

	CameraParts->SetCamPos(CamPosition, CamTarget, Util::VECTOR3D::vget(0, 1.f, 0));

	DxLib::SetMouseDispFlag(true);

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime, 0.f, 1.f);
	if (!this->m_Exit) {
	}
	else {
		if (this->m_Fade >= 1.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			Util::SceneBase::SetEndScene();
		}
	}

	if (this->m_SpeakScript.IsEnd()) {
		if (this->m_SpeakScript.GetNext() != InvalidID) {
			m_NowPhase = this->m_SpeakScript.GetNext();
			m_IsSeek = false;
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			Util::SceneBase::SetEndScene();
		}
		else {
			this->m_Exit = true;
		}
	}
	this->m_SpeakScript.Update();
	ObjectManager::Instance()->UpdateObject();

	/*
	int x = 0, y = 0;
	for (auto& t : m_Table) {
		auto Str = std::to_string(1.25f * (static_cast<float>(x) - 5.f / 2.f) + 1.f / 2.f) + ",0," + std::to_string(1.5f * (static_cast<float>(y) - 4.f / 2.f));
		OutputDebugString(Str.c_str());
		++x;
		if (x >= 5) {
			x = 0;
			y++;
		}
		OutputDebugString("\n");
	}
	OutputDebugString("\n");
	//*/
}
void MainScene::BGDraw_Sub(void) noexcept {
	this->m_SpeakScript.DrawBG();
	DrawBox(0, 0, 1920, 1080, ColorPalette::Gray25, true);
}
void MainScene::Draw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw();
	this->m_SpeakScript.Draw3D();
}
void MainScene::SetShadowDrawRigid_Sub(void) noexcept {
}
void MainScene::SetShadowDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_SetShadow();
	this->m_SpeakScript.Draw3D();
}
void MainScene::ShadowDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Shadow();
	this->m_SpeakScript.Draw3D();
}
void MainScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	//
	this->m_SpeakScript.Draw();
	//
	this->m_PauseUI.Draw();
	this->m_OptionWindow.Draw();
	this->m_SaveUI.Draw();
	this->m_ContinueUI.Draw();
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Fade), 0, 255));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}
void MainScene::Dispose_Sub(void) noexcept {
	ObjectManager::Instance()->DeleteAll();


	ObjectManager::Release();

	this->m_SpeakScript.Dispose();

	this->m_PauseUI.Dispose();
	this->m_OptionWindow.Dispose();
	this->m_SaveUI.Dispose();
	this->m_ContinueUI.Dispose();
}

