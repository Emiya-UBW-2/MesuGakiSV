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

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"

class MainScene : public Util::SceneBase {
	Draw::MV1 ModelID{};
	Draw::MV1 MapID{};
	Draw::MV1 SkyBoxID{};

	float m_XRad = 0.f;
	float m_YRad = 0.f;

	OptionWindow m_OptionWindow;
	PauseUI m_PauseUI;

	bool m_IsSceneEnd{ false };
	bool m_IsPauseActive{ false };
	char		padding[6]{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		Draw::MV1::Load("data/Soldier/model_DISABLE.mv1", &ModelID);
		Draw::MV1::Load("data/Map/model.mqoz", &MapID);
		Draw::MV1::Load("data/SkyBox/model.mqoz", &SkyBoxID);

		Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-1.f, -1.f, 1.f).normalized();

		auto* PostPassParts = Draw::PostPassEffect::Instance();
		PostPassParts->SetShadowScale(1.f);
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
			});
		this->m_PauseUI.SetEvent(1, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_PauseUI.SetEvent(2, [this]() {
			this->m_IsPauseActive = false;
			});
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		//ポーズメニュー
		{
			if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Tab)) {
				this->m_IsPauseActive ^= 1;
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
		if (m_IsPauseActive) {
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

		/*
		ModelID.SetMatrix(
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -m_XRad) *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), -m_YRad)
		);
		//*/

		Util::VECTOR3D CamPos = Util::VECTOR3D::vget(0, 1.5f, -5.f) * Scale3DRate;
		CamPos = Util::Matrix3x3::Vtrans(CamPos,
			Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), m_XRad) *
			Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), m_YRad)
		);

		auto* CameraParts = Camera::Camera3D::Instance();
		CameraParts->SetCamPos(
			CamPos,
			Util::VECTOR3D::vget(0, 0.5f, 0.f) * Scale3DRate,
			Util::VECTOR3D::vget(0, 1.f, 0));
		CameraParts->SetCamInfo(Util::deg2rad(45), 0.5f, Scale3DRate * 30.0f);
	}
	void BGDraw_Sub(void) noexcept override {
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void DrawRigid_Sub(void) noexcept override {
		MapID.DrawModel();
	}
	void Draw_Sub(void) noexcept override {
		ModelID.DrawModel();
	}
	void ShadowDrawFar_Sub(void) noexcept override {
		MapID.DrawModel();
	}
	void ShadowDraw_Sub(void) noexcept override {
		ModelID.DrawModel();
	}
	void UIDraw_Sub(void) noexcept override {
		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
	}

	void Dispose_Sub(void) noexcept override {
		SkyBoxID.Dispose();
		MapID.Dispose();
		ModelID.Dispose();
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
