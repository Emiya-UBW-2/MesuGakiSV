#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"

#include "../Util/SceneManager.hpp"

struct Move {
	Util::VECTOR2D					m_Pos{};
	float							m_Rad{};
};

class Object2DBase {
	Move					m_Pos{};
	std::array<Move, 3>	m_PosRe{};
public:
	Move& SetPos() noexcept { return m_Pos; }
public:
	void Update() noexcept {
		for (size_t loop = 1; loop < m_PosRe.size(); ++loop) {
			m_PosRe.at(loop - 1) = m_PosRe.at(loop);
		}
		m_PosRe.back() = m_Pos;
	}
	void Draw(const Draw::GraphHandle* m_Pic) noexcept {
		for (size_t loop = 0; loop < m_PosRe.size(); ++loop) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255 * (loop + 1) / m_PosRe.size()), 0, 255));
			m_Pic->DrawRotaGraph(static_cast<int>(m_PosRe.at(loop).m_Pos.x), static_cast<int>(m_PosRe.at(loop).m_Pos.y), 1.f, m_PosRe.at(loop).m_Rad, true);
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
};
class ObjectAmmo : public Object2DBase {
public:
	Util::VECTOR2D					m_Vec{};
public:
	bool IsActive() noexcept {
		return (SetPos().m_Pos.y >= -100.f);
	}
	void UpdateAmmo() noexcept {
		SetPos().m_Pos += m_Vec * DeltaTime;
		SetPos().m_Rad -= Util::deg2rad(60) * DeltaTime;
	}
};

class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_Exit{ false };
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsResetMouse{ false };
	float							m_Fade{ 1.f };
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	Sound::SoundUniqueID			m_EnviID{ InvalidID };
	const Draw::GraphHandle*		m_BackScreen{};

	const Draw::GraphHandle* m_ReimuStay{};
	const Draw::GraphHandle* m_ReimuLeft{};
	const Draw::GraphHandle* m_ReimuRight{};
	const Draw::GraphHandle* m_Reimu{};

	const Draw::GraphHandle* m_ReimuOption{};

	const Draw::GraphHandle* m_Ammo00{};
	const Draw::GraphHandle* m_Ammo01{};

	Object2DBase					m_ReimuOpt0{};
	Object2DBase					m_ReimuOpt1{};
	Object2DBase					m_ReimuPos{};
	float							m_ReimuSpeed{};
	float							m_ReimuSlowPer{};

	std::vector<ObjectAmmo>			m_Ammo00Pos{};
	float							m_Ammo00ShotTimer{};

	std::vector<ObjectAmmo>			m_Ammo01Pos{};
	float							m_Ammo01ShotTimer{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
private:
	void SetAmmo00(const Util::VECTOR2D& pos, const Util::VECTOR2D& vec) noexcept {
		for (auto& a : m_Ammo00Pos) {
			if (!a.IsActive()) {
				a.SetPos().m_Pos = pos;
				a.SetPos().m_Rad = Util::deg2rad(GetRand(90));
				a.m_Vec = vec;
				return;
			}
		}
		m_Ammo00Pos.emplace_back();
		m_Ammo00Pos.back().SetPos().m_Pos = pos;
		m_Ammo00Pos.back().SetPos().m_Rad = Util::deg2rad(GetRand(90));
		m_Ammo00Pos.back().m_Vec = vec;
	}
	void SetAmmo01(const Util::VECTOR2D& pos, const Util::VECTOR2D& vec) noexcept {
		for (auto& a : m_Ammo01Pos) {
			if (!a.IsActive()) {
				a.SetPos().m_Pos = pos;
				a.SetPos().m_Rad = Util::deg2rad(GetRand(90));
				a.m_Vec = vec;
				return;
			}
		}
		m_Ammo01Pos.emplace_back();
		m_Ammo01Pos.back().SetPos().m_Pos = pos;
		m_Ammo01Pos.back().SetPos().m_Rad = Util::deg2rad(GetRand(90));
		m_Ammo01Pos.back().m_Vec = vec;
	}
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override;
	void SetShadowDrawRigid_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) noexcept override;
	void Draw_Sub(void) noexcept override;
	void DepthDraw_Sub(void) noexcept override;
	void ShadowDrawFar_Sub(void) noexcept override;
	void ShadowDraw_Sub(void) noexcept override;
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
