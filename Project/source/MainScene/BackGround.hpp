#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/MV1.hpp"
#include "../Draw/Voxel.hpp"

class BackGround : public Util::SingletonBase<BackGround> {
private:
	friend class Util::SingletonBase<BackGround>;
private:
	BG::VoxelControl Voxel;
	Draw::MV1 SkyBoxID{};
private:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
	int				CheckLine(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, Util::VECTOR3D* Normal = nullptr) const noexcept {
		return Voxel.CheckLine(StartPos, EndPos, Normal);
	}
public:
	void Load(void) noexcept {
		Voxel.Load();												// 事前読み込み
		Draw::MV1::Load("data/SkyBox/model.mqoz", &SkyBoxID);
	}
	void Init(void) noexcept {
		Voxel.InitStart();											// 初期化開始時処理
		Voxel.LoadCellsFile("data/Map.txt");						// ボクセルデータの読み込み
		Voxel.InitEnd();											// 初期化終了時処理
	}
	void Update(void) noexcept {
		auto* CameraParts = Camera::Camera3D::Instance();
		auto* PostPassParts = Draw::PostPassEffect::Instance();
		// ボクセル処理
		Voxel.SetDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(),
			(CameraParts->GetCameraForDraw().GetCamVec() - CameraParts->GetCameraForDraw().GetCamPos()).normalized());// 描画する際の描画中心座標と描画する向きを指定
		Voxel.SetShadowDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(), PostPassParts->GetAmbientLightVec());// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		Voxel.Update();
	}
	void Dispose(void) noexcept {
		Voxel.Dispose();
		Voxel.Dispose_Load();
		SkyBoxID.Dispose();
	}

	void BGDraw(void) const noexcept {
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid(void) const noexcept {
		Voxel.DrawByShader();
	}
	void SetShadowDraw(void) const noexcept {
	}
	void Draw(void) const noexcept {
		Voxel.Draw();
	}
	void ShadowDrawFar(void) const noexcept {
		Voxel.DrawShadow();
	}
	void ShadowDraw(void) const noexcept {
	}

};
