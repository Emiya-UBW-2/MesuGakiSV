#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/MV1.hpp"
#include "../Draw/Voxel.hpp"

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"

class MainScene : public Util::SceneBase {
	Draw::MV1 ModelID{};
	BackGround::VoxelControl Voxel;
	Draw::MV1 SkyBoxID{};

	float m_XRad = 0.f;
	float m_YRad = 0.f;

	OptionWindow m_OptionWindow;
	PauseUI m_PauseUI;

	bool m_IsSceneEnd{ false };
	bool m_IsPauseActive{ false };
	char		padding[6]{};
	Util::VECTOR3D LightVec;

	Util::VECTOR3D MyPos;
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		Voxel.Load();												// 事前読み込み

		Voxel.InitStart();											// 初期化開始時処理
		Voxel.LoadCellsFile("data/Map.txt");						// ボクセルデータの読み込み
		Voxel.InitEnd();											// 初期化終了時処理

		Draw::MV1::Load("data/Soldier/model_DISABLE.mv1", &ModelID);
		Draw::MV1::Load("data/SkyBox/model.mqoz", &SkyBoxID);

		LightVec = Util::VECTOR3D::vget(-0.1f, -1.f, 0.1f).normalized();

		auto* PostPassParts = Draw::PostPassEffect::Instance();
		PostPassParts->SetShadowScale(0.5f);
		PostPassParts->SetAmbientLight(LightVec);

		SetLightEnable(FALSE);
		auto* LightParts = Draw::LightPool::Instance();
		auto& FirstLight = LightParts->Put(Draw::LightType::DIRECTIONAL, LightVec);
		SetLightAmbColorHandle(FirstLight.get(), GetColorF(0.25f, 0.25f, 0.25f, 1.0f));
		SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

		//DoF
		PostPassParts->Set_DoFNearFar(
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

		MyPos = Util::VECTOR3D::vget(MyPos.x, -50.f * Scale3DRate, MyPos.z);
		Voxel.CheckLine(Util::VECTOR3D::vget(MyPos.x, 0.f * Scale3DRate, MyPos.z), &MyPos);
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
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

		//*
		ModelID.SetMatrix(
			Util::Matrix4x4::Mtrans(MyPos)
		);
		//*/

		Util::VECTOR3D CamPos = Util::VECTOR3D::vget(0, 1.5f, -5.f) * Scale3DRate;
		CamPos = Util::Matrix3x3::Vtrans(CamPos,
			Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), m_XRad) *
			Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), m_YRad)
		);

		auto* CameraParts = Camera::Camera3D::Instance();
		CameraParts->SetCamPos(
			MyPos + CamPos,
			MyPos,
			Util::VECTOR3D::vget(0, 1.f, 0));
		CameraParts->SetCamInfo(Util::deg2rad(45), 0.5f, Scale3DRate * 30.0f);

		// ボクセル処理
		Voxel.SetDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(),
			(CameraParts->GetCameraForDraw().GetCamVec() - CameraParts->GetCameraForDraw().GetCamPos()).normalized());// 描画する際の描画中心座標と描画する向きを指定
		Voxel.SetShadowDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(), LightVec);// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		Voxel.Update();
	}
	void BGDraw_Sub(void) noexcept override {
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid_Sub(void) noexcept override {
		Voxel.DrawByShader();
	}
	void SetShadowDraw_Sub(void) noexcept override {
		ModelID.DrawModel();
	}
	void Draw_Sub(void) noexcept override {
		Voxel.Draw();
		ModelID.DrawModel();
	}
	void ShadowDrawFar_Sub(void) noexcept override {
		Voxel.DrawShadow();
	}
	void ShadowDraw_Sub(void) noexcept override {
		ModelID.DrawModel();
	}
	void UIDraw_Sub(void) noexcept override {
		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
	}

	void Dispose_Sub(void) noexcept override {
		Voxel.Dispose();
		Voxel.Dispose_Load();
		SkyBoxID.Dispose();
		ModelID.Dispose();
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
