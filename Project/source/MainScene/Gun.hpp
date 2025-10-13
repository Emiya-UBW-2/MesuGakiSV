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
	ADSPos,
	EyePos,
	Max,
};
static const char* GunFrameName[static_cast<int>(GunFrame::Max)] = {
	"センター",
	"LeftHand",
	"LeftHandZVec",
	"LeftHandYVec",
	"ADSPos",
	"EyePos",
};

class Gun :public BaseObject {
	std::array<float, static_cast<int>(GunAnim::Max)>		m_AnimPer{};
	Sound::SoundUniqueID SlideOpenID{ InvalidID };
	Sound::SoundUniqueID SlideCloseID{ InvalidID };
	Sound::SoundUniqueID ShotID{ InvalidID };
	Sound::SoundUniqueID UnLoadMagID{ InvalidID };
	Sound::SoundUniqueID LoadMagID{ InvalidID };
	Sound::SoundUniqueID ShotSPID{ InvalidID };

	Sound::SoundUniqueID TriggerID{ InvalidID };
public:
	Gun(void) noexcept {}
	Gun(const Gun&) = delete;
	Gun(Gun&&) = delete;
	Gun& operator=(const Gun&) = delete;
	Gun& operator=(Gun&&) = delete;
	virtual ~Gun(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
	const char*		GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
public:
	const auto			GetBaseLeftHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandPos)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandYVec)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandZVec)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
public:
	void Load_Sub(void) noexcept override {
		SlideOpenID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/0.wav", true);
		SlideCloseID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/1.wav", true);
		ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/2.wav", true);
		UnLoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/3.wav", true);
		LoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/4.wav", true);
		ShotSPID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/5.wav", true);
		TriggerID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/trigger.wav", true);

		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(GunAnim::Max); ++loop) {
			SetAnim(loop).SetPer(m_AnimPer[loop]);
		}
		SetAnim(static_cast<int>(GunAnim::Shot)).Update(false, 1.f);
		ModelID.FlipAnimAll();
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void ShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		ModelID.Dispose();
	}
};
