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

class MainScene : public Util::SceneBase {
	DXLibRef::MV1 ModelID{};
	DXLibRef::MV1 MapID{};
	DXLibRef::MV1 SkyBoxID{};

	float m_XRad = 0.f;
	float m_YRad = 0.f;
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		DXLibRef::MV1::Load("data/Soldier/model_DISABLE.mv1", &ModelID);
		DXLibRef::MV1::Load("data/Map/model.mqoz", &MapID);
		DXLibRef::MV1::Load("data/SkyBox/model.mqoz", &SkyBoxID);

		Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-1.f, -1.f, 1.f).normalized();

		auto* PostPassParts = Draw::PostPassEffect::Instance();
		PostPassParts->SetShadowScale(1.f);
		PostPassParts->SetAmbientLight(LightVec);

		SetLightEnable(FALSE);
		auto* LightParts = DXLibRef::LightPool::Instance();
		auto& FirstLight = LightParts->Put(DXLibRef::LightType::DIRECTIONAL, LightVec);
		SetLightAmbColorHandle(FirstLight.get(), GetColorF(0.25f, 0.25f, 0.25f, 1.0f));
		SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

		//DoF
		PostPassParts->Set_DoFNearFar(
			(Scale3DRate * 0.15f), Scale3DRate * 5.0f,
			(Scale3DRate * 0.05f), Scale3DRate * 30.0f);
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::A)) {
			m_YRad -= Util::deg2rad(60.f) / 60.f;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::D)) {
			m_YRad += Util::deg2rad(60.f) / 60.f;
		}

		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::W)) {
			m_XRad += Util::deg2rad(60.f) / 60.f;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::S)) {
			m_XRad -= Util::deg2rad(60.f) / 60.f;
		}

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
		MapID.Draw(false);
	}
	void Draw_Sub(void) noexcept override {
		ModelID.Draw(true);
	}
	void ShadowDrawFar_Sub(void) noexcept override {
		MapID.Draw(false);
	}
	void ShadowDraw_Sub(void) noexcept override {
		ModelID.Draw(true);
	}
	void UIDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, DxLib::GetColor(255, 0, 0));
	}

	void Dispose_Sub(void) noexcept override {
		SkyBoxID.Dispose();
		MapID.Dispose();
		ModelID.Dispose();
	}
};
