#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/Sound.hpp"

#include "BaseObject.hpp"

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
};

class Gun :public BaseObject {
	std::array<float, static_cast<int>(GunAnim::Max)>		m_AnimPer{};
	Sound::SoundUniqueID SlideOpenID{ InvalidID };
	Sound::SoundUniqueID SlideCloseID{ InvalidID };
	Sound::SoundUniqueID ShotID{ InvalidID };
	Sound::SoundUniqueID UnLoadMagID{ InvalidID };
	Sound::SoundUniqueID LoadMagID{ InvalidID };
	Sound::SoundUniqueID ShotSPID{ InvalidID };

	bool canshot{ true };
	char		padding[3]{};

	Draw::MV1			m_FireEffect{};
	float				m_FireOpticalPer{};
	Util::Matrix4x4		m_FireMat{};
	char		padding2[4]{};
	const Draw::GraphHandle*	m_SmokeGraph{};
	std::array<std::pair<Util::Matrix4x4, float>, 5>	m_SmokePer{};
	int					m_SmokeID{};
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
		}
	}
	bool CanShot() const { return canshot; }
public:
	void Load_Sub(void) noexcept override {
		SlideOpenID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/0.wav", true);
		SlideCloseID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/1.wav", true);
		ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/2.wav", true);
		UnLoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/3.wav", true);
		LoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/4.wav", true);
		ShotSPID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/5.wav", true);

		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
		Draw::MV1::Load("data/FireEffect/model.mv1", &m_FireEffect, DX_LOADMODEL_PHYSICS_DISABLE);
		m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Smoke.png")->Get();
	}
	void Init_Sub(void) noexcept override {
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
		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(GunAnim::Max); ++loop) {
			SetAnim(loop).SetPer(m_AnimPer[loop]);
		}
		SetAnim(static_cast<int>(GunAnim::Shot)).Update(false, 2.f);
		ModelID.FlipAnimAll();

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
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		ModelID.DrawModel();
		//
		for (auto& s : m_SmokePer) {
			float Per = std::sin(Util::deg2rad(180.f * s.second));
			if (Per > 0.f) {
				for (int loop = 0; loop < 3; ++loop) {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(64.f * Per));
					DxLib::DrawBillboard3D(
						(s.first.pos() - s.first.zvec() * (static_cast<float>(20 + (2 - loop)) / 100.f * Scale3DRate * s.second)).get(),
						0.5f,
						0.5f,
						0.2f * Scale3DRate * s.second,
						Util::deg2rad(180.f * s.second) * ((loop % 2 == 0) ? 1.f : -1.f),
						m_SmokeGraph->get(),
						true
					);
				}
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
		//
		if (!canshot) {
			SetUseLighting(FALSE);
			m_FireEffect.DrawModel();
			SetUseLighting(TRUE);
		}
	}
	void ShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		ModelID.Dispose();
		m_FireEffect.Dispose();
	}
};
