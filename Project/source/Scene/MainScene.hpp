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

class MainScene : public Util::SceneBase {
	int ModelID = -1;
	char		padding[4]{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		ModelID = MV1LoadModel("data/Soldier/model_DISABLE.mv1");
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
		auto* CameraParts = Camera::Camera3D::Instance();
		CameraParts->SetMainCamera().SetCamPos(VGet(0, 15.f, -40.f), VGet(0, 15.f, 0.f), VGet(0, 1.f, 0));
		CameraParts->SetMainCamera().SetCamInfo(Util::deg2rad(45), 0.5f, 80.f);

	}
	void BGDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), GetColor(100, 150, 255), TRUE);

	}
	void Draw_Sub(void) noexcept override {
		MV1DrawModel(ModelID);
	}
	void UIDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		DxLib::DrawCircle(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), 5, GetColor(255, 0, 0));
	}

	void Dispose_Sub(void) noexcept override {
		MV1DeleteModel(ModelID);
		ModelID = -1;
	}
};
