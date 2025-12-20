#pragma warning(disable:5259)

#include "MainScene.hpp"

const Object2DManager* Util::SingletonBase<Object2DManager>::m_Singleton = nullptr;

const AmmoImages* Util::SingletonBase<AmmoImages>::m_Singleton = nullptr;
const AmmoPool* Util::SingletonBase<AmmoPool>::m_Singleton = nullptr;
const EnemyPool* Util::SingletonBase<EnemyPool>::m_Singleton = nullptr;

void ObjectEnemy::Update_Sub(void) noexcept {
	{
		m_Anim++;
		if (m_Anim > m_EnemyData->m_PicTotalFrame) {
			m_Anim = 0;
		}
		if (m_EnemyData->m_Picture.size() > 0) {
			auto& f = m_EnemyData->m_Picture.at(m_Frame);
			SetPtr(f.m_picture);
			if (m_Anim >= f.m_EndAnim) {
				++m_Frame %= m_EnemyData->m_Picture.size();
			}
		}
	}
	{
		if (m_EnemyData->m_Move.size() - 1 > m_MoveFrame) {
			auto& Now = m_EnemyData->m_Move.at(m_MoveFrame);
			auto& Next = m_EnemyData->m_Move.at(m_MoveFrame + 1);
			auto MoveAnimPer = static_cast<float>(m_MoveAnim - Now.m_Frame) / static_cast<float>(Next.m_Frame - Now.m_Frame);
			if (Next.m_Frame <= m_MoveAnim) {
				++m_MoveFrame;
				//
				for (auto& e : Next.m_EnemyAmmo) {
					if (e.m_AmmoID != -1) {
						float Rad = 0.f;

						switch (e.m_AmmoType) {
						case AmmoType::Normal:
							break;
						case AmmoType::ToMine:
						{
							Util::VECTOR2D Vec = (*EnemyPool::Instance()->m_pMine)->SetPos().m_Pos - (SetPos().m_Pos + e.m_AmmoPos);
							Rad = std::atan2f(Vec.x, Vec.y);
						}
						break;
						default:
							break;
						}

						AmmoPool::Instance()->SetAmmo(
							SetPos().m_Pos + e.m_AmmoPos,
							e.m_AmmoScale,
							Util::VECTOR2D::vget(std::sin(Rad + Util::deg2rad(e.m_AmmoDeg)), std::cos(Rad + Util::deg2rad(e.m_AmmoDeg))) * e.m_AmmoSpeed,
							&AmmoImages::Instance()->m_Ammo.at(static_cast<size_t>(e.m_AmmoID)),
							e.m_AmmoID>=16, 
							this->GetUniqueID());
					}
				}
			}
			SetPos().m_Pos = Util::Lerp(Now.m_Pos, Next.m_Pos, MoveAnimPer);
			if (Now.m_IsEnd) {
				SetActive(false);
			}
			m_MoveAnim++;
		}
		else {
			SetActive(false);
		}
	}
}
void MainScene::Load_Sub(void) noexcept {
	Object2DManager::Create();
	AmmoPool::Create();
	EnemyPool::Create();
	AmmoImages::Create();

	m_BackScreen = Draw::GraphPool::Instance()->Get("data/Image/BackScreen.png")->Get();

}
void MainScene::Init_Sub(void) noexcept {
	this->m_Exit = false;
	this->m_Fade = 1.f;

	this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);
	this->m_EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

	this->m_NormalBGMID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::BGM, 1, "data/Sound/BGM/Normal.wav", false);
	this->m_BOSSBGMID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::BGM, 1, "data/Sound/BGM/Boss.wav", false);

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
	Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NormalBGMID)->Play(DX_PLAYTYPE_LOOP, TRUE);

	m_IsResetMouse = true;
	
	Mine = std::make_shared<ObjectMine>();
	Object2DManager::Instance()->AddObject(Mine);
	Mine->SetPos().m_Pos = Util::VECTOR2D::vget(1920.f / 2.f, 1080.f - 128.f);
	EnemyPool::Instance()->m_pMine = &Mine;

	m_StageScript.Load();

	m_SpeakScript.Load("data/message00.txt");
	m_ClearScript.Load("data/message01.txt");
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
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(306));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(307));
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

	Object2DManager::Instance()->UpdateObject();
	//更新
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

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime, 0.f, 1.f);
	if (!m_Exit) {
	}
	else {
		if (this->m_Fade >= 1.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			Util::SceneBase::SetEndScene();
		}
	}
	if (m_StageScript.IsStory()) {
		//todo::射撃できない
	}

	for (auto& e : EnemyPool::Instance()->m_EnemyPos) {
		if (e->IsActive()) {
			//
			for (auto& a : AmmoPool::Instance()->m_AmmoPos) {
				if (a->IsActive() && a->ShooterID() == Mine->GetUniqueID()) {
					auto Vec = e->SetPos().m_Pos - a->SetPos().m_Pos;
					if (Vec.magnitude() < 32.f) {
						e->m_HP--;
						if (e->m_HP <= 0) {
							e->SetActive(false);
						}
						a->SetActive(false);
						break;
					}
				}
			}
		}
	}

	m_StageScript.Update();
	if (m_StageScript.IsStory()) {
		m_SpeakScript.SetStoryStart();
		if (m_SpeakScript.IsEnd()) {
			m_StageScript.SetStoryEnd();
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NormalBGMID)->StopAll();
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_BOSSBGMID)->Play(DX_PLAYTYPE_LOOP, TRUE);
		}
	}
	if (m_StageScript.IsClear()) {
		m_ClearScript.SetStoryStart();
		if (m_ClearScript.IsEnd()) {
			this->m_Exit = true;
		}
	}
	m_SpeakScript.Update();
	m_ClearScript.Update();
}
void MainScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	Object2DManager::Instance()->Draw();
	/*
	DxLib::DrawBox(
		DrawerMngr->GetDispWidth() * (1920 / 2 - 864 / 2) / 1920, DrawerMngr->GetDispHeight() * 36 / 1080,
		DrawerMngr->GetDispWidth() * (1920 / 2 + 864 / 2) / 1920, DrawerMngr->GetDispHeight() * (1080 - 36) / 1080,
		ColorPalette::Black, true);
	//*/
	//
	m_SpeakScript.Draw();
	m_ClearScript.Draw();
	//
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
	Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NormalBGMID)->StopAll();
	Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_BOSSBGMID)->StopAll();

	Mine.reset();

	this->m_PauseUI.Dispose();
	this->m_OptionWindow.Dispose();
	AmmoPool::Release();
	EnemyPool::Release();
	AmmoImages::Release();
	Object2DManager::Instance()->DeleteAll();
	Object2DManager::Release();
}

