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
	Stay,//待機
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
	Sling,
	SlingY,
	SlingZ,
	SlingPull,
	SlingYPull,
	SlingZPull,
	MagPouch,
	MagPouchY,
	MagPouchZ,
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
	"sling",
	"slingY",
	"slingZ",
	"slingPull",
	"slingYPull",
	"slingZPull",
	"magpouch",
	"magpouchY",
	"magpouchZ",
};

struct GunParam {
	int					m_UniqueID{};
	float				m_GunPer{};
	float				m_Per{};
	float				m_PullPer{};
	int					m_EquipPhase{};
	bool				m_IsEquip{ false };
	bool				m_IsGunLoad{ false };
	char		padding[2]{};
	float				m_GunReadyPer{};
	float				m_GunADSPer{};
	float				m_GunLoadHandPer{};
	float				m_GunLoadPer{};
	float				m_GunLoadTimer{};
	const float			m_GunLoadTimerMax{ 2.f };
public:
	bool CanShot() const noexcept { return m_IsEquip && !m_IsGunLoad; }
	bool GetIsEquip() const noexcept { return m_IsEquip; }
	bool GetIsReload() const noexcept { return m_IsGunLoad; }
	void SetIsEquip(bool value) {
		this->m_IsEquip = value;
		if (this->m_IsEquip) {
			this->m_EquipPhase = 0;
			this->m_GunPer = 0.f;
			this->m_Per = 0.f;
			this->m_PullPer = 0.f;
		}
		else {
			this->m_EquipPhase = 2;
			this->m_GunPer = 1.f;
			this->m_Per = 0.f;
			this->m_PullPer = 0.f;
		}
	}
	bool GetCanReload() const noexcept { return this->m_IsEquip && !m_IsGunLoad; }
	float GetReloadPer() const noexcept { return this->m_GunLoadTimer / this->m_GunLoadTimerMax; }
	void ReloadStart() noexcept {
		m_IsGunLoad = true;
		m_GunLoadTimer = 0.f;
	}
	void Update() noexcept {
		if (this->m_IsEquip) {
			switch (this->m_EquipPhase) {
			case 0:
				this->m_Per = std::clamp(this->m_Per + 1.f / 60.f / 0.1f, 0.f, 1.f);
				if (this->m_Per >= 1.f) {
					this->m_EquipPhase = 1;
				}
				break;
			case 1:
				this->m_GunPer = std::clamp(this->m_GunPer + 1.f / 60.f / 0.1f, 0.f, 1.f);
				this->m_Per = std::clamp(this->m_Per - 1.f / 60.f / 0.1f, 0.f, 1.f);
				this->m_PullPer = std::clamp(this->m_PullPer + 1.f / 60.f / 0.1f, 0.f, 1.f);
				if (this->m_PullPer >= 1.f) {
					this->m_EquipPhase = 2;
				}
				break;
			case 2:
				this->m_Per = 0.f;
				this->m_PullPer = std::clamp(this->m_PullPer - 1.f / 60.f / 0.1f, 0.f, 1.f);
				break;
			default:
				break;
			}
			if (m_IsGunLoad) {
				m_GunLoadTimer = std::clamp(m_GunLoadTimer + 1.f / 60.f, 0.f, m_GunLoadTimerMax);
				if (0.f <= GetReloadPer() && GetReloadPer() <= 0.1f) {
					m_GunLoadHandPer = std::clamp((GetReloadPer() - 0.f) / (0.1f - 0.f), 0.f, 1.f);
				}
				if (0.1f <= GetReloadPer() && GetReloadPer() <= 0.3f) {
					m_GunLoadPer = std::clamp((GetReloadPer() - 0.1f) / (0.3f - 0.1f), 0.f, 1.f);
				}
				if (0.6f <= GetReloadPer() && GetReloadPer() <= 0.85f) {
					m_GunLoadPer = 1.f - std::clamp((GetReloadPer() - 0.6f) / (0.85f - 0.6f), 0.f, 1.f);
				}
				if (0.9f <= GetReloadPer() && GetReloadPer() <= 1.f) {
					m_GunLoadHandPer = 1.f - std::clamp((GetReloadPer() - 0.9f) / (1.f - 0.9f), 0.f, 1.f);
				}
				if (GetReloadPer() == 1.f) {
					m_IsGunLoad = false;
				}
			}
			else {
				m_GunLoadTimer = 0.f;
				m_GunLoadPer = 0.f;
				m_GunLoadHandPer = 0.f;
			}
		}
		else {
			switch (this->m_EquipPhase) {
			case 2:
				this->m_PullPer = std::clamp(this->m_PullPer + 1.f / 60.f / 0.1f, 0.f, 1.f);
				if (this->m_PullPer >= 1.f) {
					this->m_EquipPhase = 1;
				}
				break;
			case 1:
				this->m_GunPer = std::clamp(this->m_GunPer - 1.f / 60.f / 0.1f, 0.f, 1.f);
				this->m_PullPer = std::clamp(this->m_PullPer - 1.f / 60.f / 0.1f, 0.f, 1.f);
				this->m_Per = std::clamp(this->m_Per + 1.f / 60.f / 0.1f, 0.f, 1.f);
				if (this->m_Per >= 1.f) {
					this->m_EquipPhase = 0;
				}
				break;
			case 0:
				this->m_PullPer = 0.f;
				this->m_Per = std::clamp(this->m_Per - 1.f / 60.f / 0.1f, 0.f, 1.f);
				break;
			default:
				break;
			}
			m_GunLoadPer = 0.f;
			m_GunLoadHandPer = 0.f;
		}
	}
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
	int					m_HaveHandgunAnimIndex{};
	int					m_HaveRifleAnimIndex{};

	int					m_ReloadHandgunAnimIndex{};
	int					m_ReloadRifleAnimIndex{};


	int					m_Prev{};
	int					m_Now{};
	float				m_AnimChangePer{};
	bool				m_AnimMoving{ false };
	bool				m_ShotSwitch{ false };
	char		padding3[6]{};
	float				m_SwitchPer{};

	GunParam			m_Handgun{};
	GunParam			m_Maingun{};

	float				m_WalkRad{};
	//char		paddin4[4]{};
	float				m_YradAdd{};
	float				m_XradAdd{};

	float				m_YradAddR{};
	float				m_XradAddR{};

	float				m_YradAddR2{};
	float				m_XradAddR2{};
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
	auto			GetSlingMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Sling)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::SlingY)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::SlingZ)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetSlingPullMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::SlingPull)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::SlingYPull)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::SlingZPull)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}

	auto			GetMagPouchMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::MagPouch)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::MagPouchY)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::MagPouchZ)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}

	const Util::Matrix4x4 GetEyeMat(void) const noexcept;
	bool IsFPSView(void) const noexcept { return m_IsFPS; }
	bool IsShotSwitch(void) const noexcept { return m_ShotSwitch; }
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

	void SetSubGunUniqueID(int value) noexcept {
		m_Handgun.m_UniqueID = value;
	}
	void SetMainGunUniqueID(int value) noexcept {
		m_Maingun.m_UniqueID = value;
	}
public:
	void Load_Sub(void) noexcept override {
		heartID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/heart.wav", true);
		runfootID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/runfoot.wav", true);
		standupID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/standup.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
	}
	void Init_Sub(void) noexcept override {
		Speed = 0.f;

		m_StandAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Stand.txt");
		m_WalkAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Walk.txt");
		m_RunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Run.txt");

		m_HaveHandgunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/HaveHandgun.txt");
		m_HaveRifleAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/HaveRifle.txt");

		m_ReloadHandgunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/ReloadHandgun.txt");
		m_ReloadRifleAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/ReloadRifle.txt");

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
