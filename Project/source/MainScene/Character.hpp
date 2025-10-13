#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BackGround.hpp"
#include "../Util/Enum.hpp"
#include "../Util/Algorithm.hpp"
#include "../Util/Sound.hpp"
#include "../Draw/MV1.hpp"
#include "../File/FileStream.hpp"

#include "../Util/CharaAnim.hpp"

#include "BaseObject.hpp"

enum class CharaStyle {
	Stand,//立ち
	Run,//走り
	Squat,//しゃがみ
	Max,
};
enum class CharaAnim {
	Stand,//立ち
	Walk,//歩き
	Run,//走り
	Squat,//しゃがみ
	SquatWalk,//しゃがみ歩き
	ReftHand_1,//右親指
	ReftHand_2,//右一指指
	ReftHand_3,//右中指
	ReftHand_4,//右薬指
	ReftHand_5,//右子指
	LeftHand_1,//左親指
	LeftHand_2,//左一指指
	LeftHand_3,//左中指
	LeftHand_4,//左薬指
	LeftHand_5,//左子指

	Flip,//左に向く
	Max,
};

enum class CharaFrame {
	Center,
	Upper,
	Upper2,
	Head,
	Eye,
	LeftFoot1,
	LeftFoot2,
	LeftFoot,
	RightFoot1,
	RightFoot2,
	RightFoot,
	RightArm,
	RightArm2,
	RightWrist,
	RightHandJoint,
	LeftArm,
	LeftArm2,
	LeftWrist,
	LeftHandJoint,
	Holster,
	HolsterY,
	HolsterZ,
	HolsterPull,
	HolsterYPull,
	HolsterZPull,
	Max,
};
static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
	"センター",
	"上半身",
	"上半身2",
	"頭",
	"両目",
	"左足",
	"左ひざ",
	"左足首",
	"右足",
	"右ひざ",
	"右足首",
	"右腕",
	"右ひじ",
	"右手首",
	"右ダミー",
	"左腕",
	"左ひじ",
	"左手首",
	"左ダミー",
	"holster",
	"holsterY",
	"holsterZ",
	"holsterPull",
	"holsterYPull",
	"holsterZPull",
};

class Character :public BaseObject {
	Util::VECTOR3D MyPosTarget = Util::VECTOR3D::zero();
	float Xrad = 0.f;
	float Yrad = 0.f;
	float Zrad = 0.f;
	float Speed = 0.f;
	float MovePer = 0.f;
	Util::VECTOR2D VecR = Util::VECTOR2D::zero();
	float													m_YVec{};
	std::array<float, static_cast<int>(CharaAnim::Max)>		m_AnimPer{};
	std::array<float, static_cast<int>(CharaStyle::Max)>	m_StylePer{};
	CharaStyle												m_CharaStyle{ CharaStyle::Stand };
	Util::VECTOR3D											m_AimPoint;
	float				m_YradDif{};
	uint8_t				m_MoveKey{};
	bool				m_PrevIsFPSView{};
	char		padding[2]{};
	Sound::SoundUniqueID heartID{ InvalidID };
	Sound::SoundUniqueID runfootID{ InvalidID };
	Sound::SoundUniqueID standupID{ InvalidID };
	int					m_FootSoundID{};
	bool				m_IsFPS{};
	bool				m_IsActive{};
	char		padding2[2]{};
	int					m_StandAnimIndex{};
	int					m_WalkAnimIndex{};
	int					m_RunAnimIndex{};

	int					m_Prev{};
	int					m_Now{};
	float				m_AnimChangePer{};
	bool				m_AnimMoving{ false };
	bool				m_IsEquipHandgun{ false };
	char		padding3[6]{};
	int					m_GunUniqueID{};
	float				m_GunReadyPer{};
	float				m_HolsterGunPer{};
	float				m_HolsterPer{};
	float				m_HolsterPullPer{};
	int					m_EquipHandgunPhase{};
	float				m_GunADSPer{};
	//char		padding4[4]{};
public:
	Character(void) noexcept {}
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;
	virtual ~Character(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
	const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }
public:
	auto			GetHolsterMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Holster)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::HolsterY)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::HolsterZ)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetHolsterPullMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::HolsterPull)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::HolsterYPull)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::HolsterZPull)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	const Util::Matrix4x4 GetEyeMat(void) const noexcept;
	bool IsFPSView(void) const noexcept { return m_IsFPS; }
	bool IsFreeView(void) const noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		return KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim) && !IsFPSView();
	}
	float GetSpeed(void) const noexcept { return Speed; }
	float GetSpeedMax(void) const noexcept {
		switch (m_CharaStyle) {
		case CharaStyle::Run:
			return 4.5f * Scale3DRate / 60.f;
			break;
		case CharaStyle::Squat:
			return 1.0f * Scale3DRate / 60.f;
			break;
		case CharaStyle::Stand:
		case CharaStyle::Max:
		default:
			return 2.5f * Scale3DRate / 60.f;
			break;
		}
	}
	void SetPos(Util::VECTOR3D MyPos) noexcept {
		MyPosTarget = MyPos - Util::VECTOR3D::up() * Scale3DRate;
		if (!BackGround::Instance()->CheckLine(MyPos + Util::VECTOR3D::up() * Scale3DRate, &MyPosTarget)) {
			MyPosTarget = MyPos;
		}
		MyMat = Util::Matrix4x4::Mtrans(MyPosTarget);
	}
	void SetArmAnim(int Index) noexcept {
		m_Now = Index;
		if (m_Prev != m_Now && !m_AnimMoving) {
			Util::HandAnimPool::Instance()->GoTimeStart(m_Now);
			m_AnimChangePer = 0.f;
			m_AnimMoving = true;
		}
	}
	void SetIsActive(bool value) noexcept { m_IsActive = value; }

	void SetGunUniqueID(int value) noexcept {
		m_GunUniqueID = value;
	}
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
		Speed = 0.f;

		heartID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/heart.wav", true);
		runfootID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/runfoot.wav", true);
		standupID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/standup.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);

		m_StandAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Stand.txt");
		m_WalkAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Walk.txt");
		m_RunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Run.txt");

		//Util::HandAnimPool::Instance()->SetAnimSpeed(m_WalkAnimIndex, 2.5f);

		m_Prev = m_StandAnimIndex;
		SetArmAnim(m_StandAnimIndex);
		m_AnimChangePer = 1.f;
		m_AnimMoving = false;
	}
	void Update_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
		if (!IsFPSView()) {
			if (IsFreeView()) {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				Util::VECTOR3D Near = ConvScreenPosToWorldPos(VGet(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()), 0.f));
				Util::VECTOR3D Far = ConvScreenPosToWorldPos(VGet(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()), 1.f));
				Util::VECTOR3D Now = MyMat.pos();
				m_AimPoint = Util::Lerp(Near, Far, (Now.y - Near.y) / (Far.y - Near.y));
			}
		}
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
