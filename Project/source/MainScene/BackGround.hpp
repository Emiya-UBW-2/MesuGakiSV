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
	struct mapGraph {
		Draw::GraphHandle	m_Map{};
		int					m_ID{};
		float				m_Per{};
	public:
		mapGraph(void) noexcept {}
		mapGraph(const mapGraph& o) = delete;
		mapGraph(mapGraph&& o) noexcept {
			this->m_Map = (Draw::GraphHandle&&)o.m_Map;
			this->m_ID = o.m_ID;
			this->m_Per = o.m_Per;
		}
		mapGraph& operator=(const mapGraph& o) = delete;
		mapGraph& operator=(mapGraph&& o) noexcept {
			this->m_Map = (Draw::GraphHandle&&)o.m_Map;
			this->m_ID = o.m_ID;
			this->m_Per = o.m_Per;
			return *this;
		}
		virtual ~mapGraph(void) noexcept {}
	};
private:
	BG::VoxelControl Voxel;
	Draw::MV1 SkyBoxID{};
	std::vector<mapGraph> m_map;
private:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
	const BG::Algorithm::Vector3Int	GetVoxelPoint(const Util::VECTOR3D& StartPos) const noexcept {
		BG::Algorithm::Vector3Int Pos = Voxel.GetReferenceCells().GetVoxelPoint(StartPos);
		Pos.x -= Voxel.GetReferenceCells().All / 2;
		Pos.z -= Voxel.GetReferenceCells().All / 2;
		return Pos;
	}
	int				CheckLine(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, Util::VECTOR3D* Normal = nullptr) const noexcept {
		return Voxel.CheckLine(StartPos, EndPos, Normal);
	}
	bool			CheckWall(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& AddCapsuleMin, const Util::VECTOR3D& AddCapsuleMax, float Radius, const std::vector<const Draw::MV1*>& addonColObj) const noexcept {
		return Voxel.CheckWall(StartPos, EndPos, AddCapsuleMin, AddCapsuleMax, Radius, addonColObj);
	}
	std::vector<mapGraph>& GetMapGraph(void) noexcept { return m_map; }
public:
	void Load(void) noexcept {
		Voxel.Load("data/Map1/tex.png");							// 事前読み込み
		Draw::MV1::Load("data/SkyBox/model.mqoz", &SkyBoxID);
		SetTransColor(0, 255, 0);
		for (int loop = 0; loop < Voxel.GetReferenceCells().All; ++loop) {
			std::string Path = "data/Map1/map";
			Path += std::to_string(loop);
			Path += ".png";
			if (Util::IsFileExist(Path.c_str())) {
				m_map.emplace_back();
				m_map.back().m_Map.Load(Path);
				m_map.back().m_ID = loop;
				m_map.back().m_Per = 0.f;
			}
		}
		SetTransColor(0, 0, 0);
	}
	void Init(void) noexcept {
		Voxel.InitStart();											// 初期化開始時処理
		Voxel.LoadCellsFile("data/Map1/Map.txt");					// ボクセルデータの読み込み
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
		for (auto& m : m_map) {
			m.m_Map.Dispose();
		}
		m_map.clear();
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
