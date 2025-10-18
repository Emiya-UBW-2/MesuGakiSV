#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/Sound.hpp"

#include "BaseObject.hpp"

#include "BackGround.hpp"

enum class GunAnim {
	Slide,//スライド
	Trigger,//トリガー
	Shot,//射撃
	Max,
};

enum class GunFrame {
	Center,
	LeftHandPos,
	LeftHandZVec,
	LeftHandYVec,
	EyePosLeft,
	RightHandPos,
	RightHandZVec,
	RightHandYVec,
	EyePosRight,
	ADSPos,
	Muzzle,
	Case,
	CaseVec,
	MagWel,
	MagIn,
	Max,
};
static const char* GunFrameName[static_cast<int>(GunFrame::Max)] = {
	"センター",
	"LeftHand",
	"LeftHandZVec",
	"LeftHandYVec",
	"EyePosLeft",
	"RightHand",
	"RightHandZVec",
	"RightHandYVec",
	"EyePosRight",
	"ADSPos",
	"muzzle",
	"case",
	"casevec",
	"magwel",
	"magin",
};

class Gun :public BaseObject {
	std::array<float, static_cast<int>(GunAnim::Max)>		m_AnimPer{};
	Sound::SoundUniqueID SlideOpenID{ InvalidID };
	Sound::SoundUniqueID SlideCloseID{ InvalidID };
	Sound::SoundUniqueID ShotID{ InvalidID };
	Sound::SoundUniqueID UnLoadMagID{ InvalidID };
	Sound::SoundUniqueID LoadMagID{ InvalidID };
	Sound::SoundUniqueID ShotSPID{ InvalidID };

	Sound::SoundUniqueID HitGroundID{ InvalidID };

	Sound::SoundUniqueID FallCaseID{ InvalidID };

	bool canshot{ true };
	char		padding[3]{};

	Draw::MV1			m_Case{};
	Draw::MV1			m_Mag{};

	Draw::MV1			m_FireEffect{};
	float				m_FireOpticalPer{};
	Util::Matrix4x4		m_FireMat{};
	char		padding2[4]{};
	const Draw::GraphHandle*	m_SmokeGraph{};
	std::array<std::pair<Util::Matrix4x4, float>, 5>	m_SmokePer{};
	int					m_SmokeID{};

	class Case {
	public:
		Case(void) noexcept {}
		Case(const Case&) = delete;
		Case(Case&&) = delete;
		Case& operator=(const Case&) = delete;
		Case& operator=(Case&&) = delete;
		virtual ~Case(void) noexcept {}
	public:
		Draw::MV1			m_Case{};
		Util::Matrix4x4 Mat{};
		Util::VECTOR3D Vector{};
		float YVecAdd{};
		float Timer{};
		bool m_IsPlaySound{};
		char		padding[3]{};
	};

	std::array<Case, 5>	m_CasePer{};
	int					m_CaseID{};
	char		padding3[4]{};


	class Ammo {
	public:
		Ammo(void) noexcept {}
		Ammo(const Ammo&) = delete;
		Ammo(Ammo&&) = delete;
		Ammo& operator=(const Ammo&) = delete;
		Ammo& operator=(Ammo&&) = delete;
		virtual ~Ammo(void) noexcept {}
	public:
		Util::Matrix4x4 Mat{};
		Util::VECTOR3D Vector{};
		float YVecAdd{};
		float Timer{};
		float DrawTimer{};
	};

	std::array<Ammo, 5>	m_AmmoPer{};
	int					m_AmmoID{};
	char		padding4[4]{};

	class AmmoEffect {
	public:
		AmmoEffect(void) noexcept {}
		AmmoEffect(const AmmoEffect&) = delete;
		AmmoEffect(AmmoEffect&&) = delete;
		AmmoEffect& operator=(const AmmoEffect&) = delete;
		AmmoEffect& operator=(AmmoEffect&&) = delete;
		virtual ~AmmoEffect(void) noexcept {}
	public:
		Util::Matrix4x4 Mat{};
		Util::VECTOR3D Vector{};
		float YVecAdd{};
		float Timer{};
		char		padding[4]{};
	};

	std::array<AmmoEffect, 50>	m_AmmoEffectPer{};
	int					m_AmmoEffectID{};
	char		padding5[4]{};

	float				m_MagInPer{};
	float				m_MagLoadPer{};

	bool				m_IsMagUnloadSound{};
	bool				m_IsMagLoadSound{};

	Util::Matrix4x4		m_MagLoad{};
public:
	Gun(void) noexcept {}
	Gun(const Gun&) = delete;
	Gun(Gun&&) = delete;
	Gun& operator=(const Gun&) = delete;
	Gun& operator=(Gun&&) = delete;
	virtual ~Gun(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
	const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
public:
	auto			GetBaseLeftHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandPos)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandYVec)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandZVec)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetBaseRightHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::RightHandPos)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::RightHandYVec)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::RightHandZVec)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetMagLeftHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = m_Mag.GetFrameLocalWorldMatrix(static_cast<int>(2)).pos();
		Util::VECTOR3D Handyvec = m_Mag.GetFrameLocalWorldMatrix(static_cast<int>(4)).pos() - HandPos;
		Util::VECTOR3D Handzvec = m_Mag.GetFrameLocalWorldMatrix(static_cast<int>(3)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
public:
	void ShotStart() {
		if (canshot) {
			canshot = false;
			m_AnimPer[static_cast<int>(GunAnim::Shot)] = 1.f;
			SetAnim(static_cast<int>(GunAnim::Shot)).SetTime(0.f);

			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, ShotID)->Play3D(MyMat.pos(), 50.f * Scale3DRate);

			m_FireMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(GetRand(90)));

			m_SmokePer.at(static_cast<size_t>(m_SmokeID)).first = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Muzzle));
			m_SmokePer.at(static_cast<size_t>(m_SmokeID)).second = 0.f;
			++m_SmokeID %= static_cast<int>(m_SmokePer.size());

			m_CasePer.at(static_cast<size_t>(m_CaseID)).Mat = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Case));
			m_CasePer.at(static_cast<size_t>(m_CaseID)).Vector = (
				GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::CaseVec)).pos() -
				GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Case)).pos()
				).normalized() * (static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate / 60.f);
			m_CasePer.at(static_cast<size_t>(m_CaseID)).YVecAdd = 0.f;
			m_CasePer.at(static_cast<size_t>(m_CaseID)).Timer = 1.f;
			m_CasePer.at(static_cast<size_t>(m_CaseID)).m_IsPlaySound = false;
			++m_CaseID %= static_cast<int>(m_CasePer.size());

			auto& s = m_AmmoPer.at(static_cast<size_t>(m_AmmoID));
			s.Mat = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Muzzle));
			s.Vector =
				GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Muzzle)).zvec() *
				(-100.f * Scale3DRate / 60.f);
			s.YVecAdd = 0.f;
			s.Timer = 1.f;
			s.DrawTimer = s.Timer + 1.f;
			++m_AmmoID %= static_cast<int>(m_AmmoPer.size());
		}
	}
	bool CanShot() const { return canshot; }
	void SetMagLoadMat(const Util::Matrix4x4& value) { m_MagLoad = value; }

	void SetMagLoad(float value) {
		m_MagInPer = std::clamp((value - 0.f) / (0.3f - 0.f), 0.f, 1.f);
		m_MagLoadPer = std::clamp((value - 0.3f) / (1.f - 0.3f), 0.f, 1.f);
	}

	void SetMagSound(float value) {
		if (value == 0.f) {
			m_IsMagUnloadSound = false;
			m_IsMagLoadSound = false;
		}
		if (value >= 0.1f) {
			if (!m_IsMagUnloadSound) {
				m_IsMagUnloadSound = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, UnLoadMagID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
			}
		}
		if (value >= 0.8f) {
			if (!m_IsMagLoadSound) {
				m_IsMagLoadSound = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, LoadMagID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
			}
		}
	}
public:
	void Load_Sub(void) noexcept override {
		SlideOpenID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/0.wav", true);
		SlideCloseID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/1.wav", true);
		ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/2.wav", true);
		UnLoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/3.wav", true);
		LoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/4.wav", true);
		ShotSPID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/5.wav", true);
		HitGroundID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitGround.wav", true);
		FallCaseID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/FallCase.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
		Draw::MV1::Load("data/FireEffect/model.mv1", &m_FireEffect, DX_LOADMODEL_PHYSICS_DISABLE);
		m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Smoke.png")->Get();

		Draw::MV1::Load("data/9x19/model.mv1", &m_Case, DX_LOADMODEL_PHYSICS_DISABLE);
		Draw::MV1::Load("data/Mag/model.mv1", &m_Mag, DX_LOADMODEL_PHYSICS_DISABLE);
	}
	void Init_Sub(void) noexcept override {
		for (auto& s : m_CasePer) {
			s.m_Case.Duplicate(m_Case);
		}
		SetMagLoad(0.f);
	}
	void Update_Sub(void) noexcept override {
		if (!canshot) {
			float AnimPer = SetAnim(static_cast<int>(GunAnim::Shot)).GetTimePer();
			if (AnimPer < 0.3f) {
				float Per = (AnimPer - 0.f) / (0.3f - 0.f);

				m_FireOpticalPer = Util::Lerp(0.f, 1.f, Per);
			}
			else if (AnimPer < 1.f) {
				float Per = (AnimPer - 0.3f) / (1.f - 0.3f);

				m_FireOpticalPer = Util::Lerp(1.f, 0.f, Per);
			}

			if (SetAnim(static_cast<int>(GunAnim::Shot)).GetTimePer() >= 1.f) {
				m_AnimPer[static_cast<int>(GunAnim::Shot)] = 0.f;
				canshot = true;
			}
		}

		for (auto& s : m_SmokePer) {
			s.second = std::clamp(s.second + 1.f / 60.f / 0.5f, 0.f, 1.f);
		}
		for (auto& s : m_AmmoPer) {
			if (s.Timer == 0.f && s.DrawTimer == 0.f) { continue; }
			s.DrawTimer = std::max(s.DrawTimer - 1.f / 60.f, 0.f);
			if (s.Timer == 0.f) { continue; }
			s.Timer = std::max(s.Timer - 1.f / 60.f, 0.f);
			s.YVecAdd -= 1.f / 2.f * (9.8f * Scale3DRate) / (60.f * 60.f);
			s.Vector.y += s.YVecAdd;
			Util::VECTOR3D Target = s.Mat.pos() + s.Vector;
			if (BackGround::Instance()->CheckLine(s.Mat.pos(), &Target)) {
				s.Vector = Target - s.Mat.pos();
				s.Timer = 0.f;
				s.DrawTimer = s.Timer + 1.f;
				for (int loop = 0; loop < 10; ++loop) {
					auto& ae = m_AmmoEffectPer.at(static_cast<size_t>(m_AmmoEffectID));
					ae.Mat = Util::Matrix4x4::Mtrans(Target);
					ae.Vector = s.Vector.normalized() * -1.f;
					ae.Vector =
						Util::Matrix4x4::Vtrans(ae.Vector,
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::forward(), ae.Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f))) *
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::up(), ae.Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f)))
						) *
						(static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate / 60.f);
					ae.YVecAdd = 0.f;
					ae.Timer = 1.f;
					++m_AmmoEffectID %= static_cast<int>(m_AmmoEffectPer.size());
				}
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(Target, 10.f * Scale3DRate);
			}

			s.Mat = s.Mat.rotation() *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(s.Vector, s.Mat.zvec()).normalized(), Util::deg2rad(1800.f) / 60.f) *
				Util::Matrix4x4::Mtrans(Target);
		}

		for (auto& ae : m_AmmoEffectPer) {
			if (ae.Timer == 0.f) { continue; }
			ae.Timer = std::max(ae.Timer - 1.f / 60.f, 0.f);
			ae.YVecAdd -= 1.f / 2.f * (9.8f * Scale3DRate) / (60.f * 60.f);
			ae.Vector.y += ae.YVecAdd;
			Util::VECTOR3D Target = ae.Mat.pos() + ae.Vector;
			Util::VECTOR3D Normal;
			if (BackGround::Instance()->CheckLine(ae.Mat.pos(), &Target, &Normal)) {
				ae.YVecAdd = 0.f;
				ae.Vector = Util::VECTOR3D::Reflect(ae.Vector, Normal) * 0.8f;
			}
			ae.Mat = ae.Mat.rotation() * Util::Matrix4x4::Mtrans(Target);
		}

		for (auto& s : m_CasePer) {
			if (s.Timer == 0.f) { continue; }
			s.Timer = std::max(s.Timer - 1.f / 60.f, 0.f);
			s.YVecAdd -= 1.f / 2.f * (9.8f * Scale3DRate) / (60.f * 60.f);
			s.Vector.y += s.YVecAdd;
			Util::VECTOR3D Target = s.Mat.pos() + s.Vector;
			Util::VECTOR3D Normal;
			if (BackGround::Instance()->CheckLine(s.Mat.pos(), &Target, &Normal)) {
				s.YVecAdd = 0.f;
				s.Vector = Util::VECTOR3D::Reflect(s.Vector, Normal) * 0.8f;
				if (!s.m_IsPlaySound) {
					s.m_IsPlaySound = true;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, FallCaseID)->Play3D(Target, 10.f * Scale3DRate);
				}
			}

			s.Mat = s.Mat.rotation() *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(s.Vector, s.Mat.zvec()).normalized(), Util::deg2rad(1800.f) / 60.f) *
				Util::Matrix4x4::Mtrans(Target);
			s.m_Case.SetMatrix(s.Mat);
		}
		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(GunAnim::Max); ++loop) {
			SetAnim(loop).SetPer(m_AnimPer[loop]);
		}
		SetAnim(static_cast<int>(GunAnim::Shot)).Update(false, 2.f);
		ModelID.FlipAnimAll();
		//
		auto MagWel = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::MagWel));
		auto MagIn = GetFrameBaseLocalMat(static_cast<int>(GunFrame::MagIn)).pos();
		auto Mag = Util::Lerp(GetMat(), Util::Matrix4x4::RotVec2(MagIn, Util::VECTOR3D::down()) * MagWel, m_MagInPer);

		Mag = Util::Lerp(Mag, m_MagLoad, m_MagLoadPer);
		m_Mag.SetMatrix(Mag);

		{
			float AnimPer = SetAnim(static_cast<int>(GunAnim::Shot)).GetTimePer();
			m_FireEffect.SetMatrix(
				Util::Matrix4x4::GetScale(AnimPer) *
				m_FireMat *
				GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Muzzle))
			);
			m_FireEffect.SetOpacityRate(m_FireOpticalPer);
		}
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
		m_Mag.DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		ModelID.DrawModel();
		m_Mag.DrawModel();
		//
		DxLib::SetUseLighting(FALSE);
		for (auto& ae : m_AmmoEffectPer) {
			if (ae.Timer == 0.f) { continue; }
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * ae.Timer), 0, 255));
			DxLib::DrawCapsule3D(
				(ae.Mat.pos() - ae.Vector * std::clamp(ae.Timer, 0.f, 1.f)).get(),
				ae.Mat.pos().get(),
				0.01f * Scale3DRate / 2.f,
				6,
				DxLib::GetColor(255, 255, 128),
				DxLib::GetColor(255, 255, 0),
				true
			);
		}
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		//
		DxLib::SetUseLighting(FALSE);
		for (auto& s : m_AmmoPer) {
			if (s.DrawTimer == 0.f) { continue; }
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * s.DrawTimer), 0, 255));
			DxLib::DrawCapsule3D(
				(s.Mat.pos() - s.Vector * std::clamp(s.DrawTimer, 0.f, 1.f)).get(),
				s.Mat.pos().get(),
				0.009f * Scale3DRate / 2.f,
				6,
				DxLib::GetColor(255, 255, 128),
				DxLib::GetColor(255, 255, 0),
				true
			);
		}
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		//
		DxLib::SetUseLighting(FALSE);
		for (auto& s : m_SmokePer) {
			float Per = std::sin(Util::deg2rad(180.f * s.second));
			if (Per > 0.f) {
				for (int loop = 0; loop < 3; ++loop) {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(64.f * Per));
					DxLib::DrawBillboard3D(
						(s.first.pos() - s.first.zvec() * (static_cast<float>(10 + (2 - loop) * 10) / 100.f * Scale3DRate * s.second)).get(),
						0.5f,
						0.5f,
						0.2f * Scale3DRate * s.second,
						Util::deg2rad(180.f * s.second) * ((loop % 2 == 0) ? 1.f : -1.f),
						m_SmokeGraph->get(),
						true
					);
				}
			}
		}
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		//
		if (!canshot) {
			DxLib::SetUseLighting(FALSE);
			m_FireEffect.DrawModel();
			DxLib::SetUseLighting(TRUE);
		}
		//
		for (auto& s : m_CasePer) {
			if (s.Timer == 0.f) { continue; }
			s.m_Case.DrawModel();
		}
	}
	void ShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
		m_Mag.DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		ModelID.Dispose();
		m_FireEffect.Dispose();
		for (auto& s : m_CasePer) {
			s.m_Case.Dispose();
		}
		m_Case.Dispose();
		m_Mag.Dispose();
	}
};
