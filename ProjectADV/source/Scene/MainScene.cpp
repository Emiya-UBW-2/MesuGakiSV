#pragma warning(disable:5259)

#include "MainScene.hpp"

void MainScene::Load_Sub(void) noexcept {
	m_SpeakScript.Load("data/message00.txt");
}
void MainScene::Init_Sub(void) noexcept {
	Draw::MV1Pool::Instance()->SetModelAll();
	this->m_Exit = false;
	this->m_Fade = 1.f;

	this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

	this->m_NormalBGMID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::BGM, 1, "data/Sound/BGM/Normal.wav", false);

	Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.3f, -0.5f, -0.9f).normalized();

	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(0.5f);
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

	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	KeyGuideParts->SetGuideFlip();

	Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NormalBGMID)->Play(DX_PLAYTYPE_LOOP, TRUE);

	m_IsResetMouse = true;
	m_SpeakScript.SetStoryStart();
	//セーブデータから目標の会話データ番号を探る//TODO
	int TargetPoint = 0;//1997;
	//目標地点まで最速スキップ
	for (int loop = 0; loop < TargetPoint; ++loop) {
		m_SpeakScript.Step();
	}
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
		this->m_PauseUI.SetActive(this->m_IsPauseActive && !this->m_OptionWindow.IsActive());
		if (!this->m_IsPauseActive) {
			this->m_OptionWindow.SetActive(false);
		}
		this->m_PauseUI.Update();
		if (this->m_IsSceneEnd && this->m_PauseUI.IsEnd()) {
			this->m_Exit = true;
		}
		this->m_OptionWindow.Update();
	}
	if (this->m_IsPauseActive) {
		DxLib::SetMouseDispFlag(true);
		m_IsResetMouse = true;
		return;
	}

	//更新
	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;

	CamPosition = Util::VECTOR3D::vget(0.f, 15.f, -20.f);
	CamTarget = Util::VECTOR3D::vget(0.f, 15.f, 0.f);

	CameraParts->SetCamPos(CamPosition, CamTarget, Util::VECTOR3D::vget(0, 1.f, 0));

	DxLib::SetMouseDispFlag(true);

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime, 0.f, 1.f);
	if (!m_Exit) {
	}
	else {
		if (this->m_Fade >= 1.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			Util::SceneBase::SetEndScene();
		}
	}

	if (m_SpeakScript.IsEnd()) {
		this->m_Exit = true;
	}
	m_SpeakScript.Update();
}
void MainScene::BGDraw_Sub(void) noexcept {
	m_SpeakScript.DrawBG();
}
void MainScene::Draw_Sub(void) noexcept {
	m_SpeakScript.Draw3D();
}
void MainScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	//
	m_SpeakScript.Draw();
	//
	this->m_PauseUI.Draw();
	this->m_OptionWindow.Draw();
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Fade), 0, 255));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}
void MainScene::Dispose_Sub(void) noexcept {
	Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NormalBGMID)->StopAll();

	Sound::SoundPool::Instance()->Delete(Sound::SoundType::BGM, this->m_NormalBGMID);

	this->m_PauseUI.Dispose();
	this->m_OptionWindow.Dispose();
}

