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

class MainScene : public Util::SceneBase {
	int ModelID{ InvalidID };
	int MapID{ InvalidID };
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		ModelID = DxLib::MV1LoadModel("data/Soldier/model_DISABLE.mv1");
		MapID = DxLib::MV1LoadModel("data/Map/model.mv1");

		Util::VECTOR3D LightVec = Util::VECTOR3D::vget(1.f, -1.f, 1.f).normalized();

		auto* PostPassParts = Draw::PostPassEffect::Instance();
		PostPassParts->SetShadowScale(1.f);
		PostPassParts->SetAmbientLight(LightVec);

		SetLightEnable(FALSE);
		auto* LightParts = DXLibRef::LightPool::Instance();
		auto& FirstLight = LightParts->Put(DXLibRef::LightType::DIRECTIONAL, LightVec);
		SetLightAmbColorHandle(FirstLight.get(), GetColorF(0.25f, 0.25f, 0.25f, 1.0f));
		SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
		auto* CameraParts = Camera::Camera3D::Instance();
		CameraParts->SetCamPos(Util::VECTOR3D::vget(0, 1.5f * Scale3DRate, -5.f * Scale3DRate), Util::VECTOR3D::vget(0, 0.5f * Scale3DRate, 0.f), Util::VECTOR3D::vget(0, 1.f, 0));
		CameraParts->SetCamInfo(Util::deg2rad(45), 0.5f, 80.f);

	}
	void BGDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), DxLib::GetColor(100, 150, 255), TRUE);

	}
	void Draw_Sub(void) noexcept override {
		DxLib::MV1DrawModel(MapID);
		DxLib::MV1DrawModel(ModelID);
	}
	void UIDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, DxLib::GetColor(255, 0, 0));
	}

	void Dispose_Sub(void) noexcept override {
		DxLib::MV1DeleteModel(MapID);
		MapID = InvalidID;
		DxLib::MV1DeleteModel(ModelID);
		ModelID = InvalidID;
	}
};
