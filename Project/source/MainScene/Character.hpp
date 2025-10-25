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


//ヒットボックス表示
#define DRAW_HITBOX (false)
namespace HB {
	//キャラのうち特定機能だけ抜き出したもの
	//
	enum class HitType {
		Head,
		Body,
		Arm,
		Leg,
		//ヒール限定
		Armor,
		Helmet,
	};
	//
	class HitBox {
		Util::VECTOR3D	m_pos;
		float		m_radius{ 0.0f };
		HitType		m_HitType{ HitType::Body };
	public:
		const auto& GetColType(void) const noexcept { return this->m_HitType; }
		const auto& GetPos(void) const noexcept { return this->m_pos; }
	public:
		void	Update(const Util::VECTOR3D& pos, float radius, HitType pHitType) {
			this->m_pos = pos;
			this->m_radius = radius;
			this->m_HitType = pHitType;
		}
#if DRAW_HITBOX
		void	Draw(void) const noexcept {
			unsigned int color{};
			switch (this->m_HitType) {
			case HitType::Head:
				color = ColorPalette::Red;
				break;
			case HitType::Body:
				color = ColorPalette::Green;
				break;
			case HitType::Arm:
				color = ColorPalette::Blue;
				break;
			case HitType::Leg:
				color = ColorPalette::Blue;
				break;
			default:
				break;
			}
			DrawSphere3D(this->m_pos.get(), this->m_radius, 6, color, color, true);
		}
#endif
		bool	Colcheck(const Util::VECTOR3D& StartPos, Util::VECTOR3D* pEndPos) const noexcept {
			VECTOR pos1 = StartPos.get();
			VECTOR pos2 = pEndPos->get();
			VECTOR posA = this->m_pos.get();
			SEGMENT_POINT_RESULT Res;
			Segment_Point_Analyse(&pos1, &pos2, &posA, &Res);
			if (Res.Seg_Point_MinDist_Square <= this->m_radius * this->m_radius) {
				*pEndPos = Res.Seg_MinDist_Pos;
				return true;
			}
			return false;
		}
	};
	class HitBoxControl {
	private:
		std::vector<HitBox>									m_HitBox;
	public:
		const HitBox* GetLineHit(const Util::VECTOR3D& StartPos, Util::VECTOR3D* pEndPos) const noexcept {
			for (auto& hitbox : this->m_HitBox) {
				if (hitbox.Colcheck(StartPos, pEndPos)) {
					return &hitbox;
				}
			}
			return nullptr;
		}
	public:
		void		CheckLineHitNearest(const Util::VECTOR3D& StartPos, Util::VECTOR3D* pEndPos) const noexcept {
			for (auto& hitbox : this->m_HitBox) {
				hitbox.Colcheck(StartPos, pEndPos);
			}
		}
		const auto& GetHitBoxPointList(void) const noexcept { return this->m_HitBox; }
	public:
		HitBoxControl(void) noexcept {}
		virtual ~HitBoxControl(void) noexcept {}
	public:
		void Init(void) noexcept {
			this->m_HitBox.resize(27);
		}
		void Update(const BaseObject* ptr, float SizeRate) noexcept;
#if DRAW_HITBOX
		void Draw(void) const noexcept {
			SetUseLighting(false);
			//SetUseZBuffer3D(false);

			for (auto& hitbox : this->m_HitBox) {
				hitbox.Draw();
			}

			//SetUseZBuffer3D(true);
			SetUseLighting(true);
		}
#endif
	};
}

enum class CharaStyle {
	Stand,//立ち
	Run,//走り
	Squat,//しゃがみ
	Prone,//伏せ
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

	FlipLeft,//左に向く
	Stay,//待機

	Prone,//伏せ
	ProneWalk,//伏せ移動

	FlipRight,//右に向く

	ProneAim,//伏せエイム

	Combo,//3段コンボ

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

class GunParam {
public:
	int					m_UniqueID{};
	float				m_GunPer{};
	float				m_Per{};
	float				m_PullPer{};
	int					m_EquipPhase{};
	bool				m_IsEquip{ false };
	bool				m_IsGunLoad{ false };
	bool				m_IsCocking{ false };
	char		padding[1]{};
	float				m_GunReadyPer{};
	float				m_GunPullPer{};
	float				m_GunADSPer{};
	float				m_GunLoadHandPer{};
	float				m_GunLoadPer{};
	float				m_GunLoadTimer{};
	const float			m_GunLoadTimerMax{ 2.f };

	float				m_CockingTimer{};
	const float			m_CockingTimerMax{ 0.5f };
	char		padding2[4]{};
public:
	GunParam(void) noexcept {}
	GunParam(const GunParam&) = delete;
	GunParam(GunParam&&) = delete;
	GunParam& operator=(const GunParam&) = delete;
	GunParam& operator=(GunParam&&) = delete;
	virtual ~GunParam(void) noexcept {}
public:
	auto GetPer(void) const noexcept { return this->m_Per; }
	auto GetPullPer(void) const noexcept { return this->m_PullPer; }
	auto GetGunPer(void) const noexcept { return this->m_GunPer; }
	auto GetGunPullPer(void) const noexcept { return this->m_GunPullPer; }
	auto GetReadyPer(void) const noexcept { return this->m_GunReadyPer; }
	auto GetADSPer(void) const noexcept { return this->m_GunADSPer; }
	auto GetLoadHandPer(void) const noexcept { return this->m_GunLoadHandPer; }
	auto GetUniqueID(void) const noexcept { return this->m_UniqueID; }
	bool CanShot(void) const noexcept { return this->m_IsEquip && !m_IsGunLoad; }
	bool GetIsEquip(void) const noexcept { return this->m_IsEquip; }
	bool GetIsReload(void) const noexcept { return this->m_IsGunLoad; }
	bool GetIsCocking(void) const noexcept { return this->m_IsCocking; }
	bool GetInHolster(void) const noexcept { return this->m_EquipPhase <= 1 && !GetIsEquip(); }

	bool GetIsReady(void) const noexcept { return GetIsEquip() && GetReadyPer() > 0.95f; }
	
	void SetIsEquip(bool value) {
		bool IsChange = this->m_IsEquip != value;
		this->m_IsEquip = value;
		if (IsChange) {
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
	}
	bool GetCanReload(void) const noexcept { return this->m_IsEquip && !m_IsGunLoad; }
	float GetReloadPer(void) const noexcept { return this->m_GunLoadTimer / this->m_GunLoadTimerMax; }
	float GetCockingPer(void) const noexcept { return this->m_CockingTimer / this->m_CockingTimerMax; }
	void ReloadStart(void) noexcept {
		this->m_IsGunLoad = true;
		this->m_GunLoadTimer = 0.f;
	}
	void Update(void) noexcept;
};

class CharacterCommon :public BaseObject {
	HB::HitBoxControl	m_HitBoxControl;
	Sound::SoundUniqueID	m_runfootID{ InvalidID };
	Sound::SoundUniqueID	m_standupID{ InvalidID };
protected:
	Util::VECTOR3D		m_MyPosTarget = Util::VECTOR3D::zero();
	Util::VECTOR3D		m_Rad = Util::VECTOR3D::zero();
	Util::VECTOR3D		m_RadAdd = Util::VECTOR3D::zero();
	Util::VECTOR3D		m_Vector = Util::VECTOR3D::zero();
	Util::VECTOR2D		m_InputVec = Util::VECTOR2D::zero();
	float				m_Speed = 0.f;
	float				m_MovePer = 0.f;
	float				m_YradDif{};
	int					m_FootSoundID{};
	//char		padding2[3]{};

	Util::Matrix3x3		m_Rot;
	char		padding3[4]{};
protected:
	auto GetRotMat() const { return Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), this->m_Rad.z) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), this->m_Rad.y); }
	float CalcYradDiff(float TargetYRad) {
		this->m_YradDif = TargetYRad - this->m_Rad.y;
		if (this->m_YradDif > 0.f) {
			while (true) {
				if (this->m_YradDif < DX_PI_F) { break; }
				this->m_YradDif -= DX_PI_F * 2.f;
			}
		}
		if (this->m_YradDif < 0.f) {
			while (true) {
				if (this->m_YradDif > -DX_PI_F) { break; }
				this->m_YradDif += DX_PI_F * 2.f;
			}
		}
		return std::clamp(this->m_YradDif / Util::deg2rad(15.f), -1.f, 1.f);
	}
	auto GetMovePer01() const { return m_MovePer; }
	auto GetTargetPos() const { return m_MyPosTarget; }

	void CheckWall(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& PosAdd, const Util::VECTOR3D& AddCapsuleMin, const Util::VECTOR3D& AddCapsuleMax, float Radius) {
		Util::VECTOR3D SPos = StartPos + PosAdd;
		Util::VECTOR3D EPos = *EndPos + PosAdd;
		std::vector<const Draw::MV1*> addonColObj;
		BackGround::Instance()->CheckWall(SPos, &EPos, AddCapsuleMin, AddCapsuleMax, Radius, addonColObj);// 現在地から仮座標に進んだ場合
		*EndPos = EPos - PosAdd;
	}
	bool CheckGround(Util::VECTOR3D* pPos, const Util::VECTOR3D& PosAdd, float UpLimit, float DownLimit) {
		Util::VECTOR3D Pos = *pPos + PosAdd - Util::VECTOR3D::up() * DownLimit * Scale3DRate;
		if (BackGround::Instance()->CheckLine(*pPos + PosAdd + Util::VECTOR3D::up() * UpLimit * Scale3DRate, &Pos) != 0) {
			*pPos = Pos - PosAdd;
			return true;
		}
		else {
			return false;
		}
	}
protected:
	float GetSpeed() const { return m_Speed; }
public:
	CharacterCommon(void) noexcept {}
	CharacterCommon(const CharacterCommon&) = delete;
	CharacterCommon(CharacterCommon&&) = delete;
	CharacterCommon& operator=(const CharacterCommon&) = delete;
	CharacterCommon& operator=(CharacterCommon&&) = delete;
	virtual ~CharacterCommon(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
	const char* GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }
protected:
	void PlayFootSound(int vol) noexcept {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_runfootID)->SetLocalVolume(vol);
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_runfootID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
	}
	void PlayMoveSound(void) noexcept {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_standupID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
	}
public:
	const HB::HitBox* CheckHit(const Util::VECTOR3D& StartPos, Util::VECTOR3D* pEndPos) noexcept {
		//if (!IsAlive()) { return nullptr; }
		if (!(Util::GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMat().pos()) <= 2.0f * Scale3DRate)) { return nullptr; }
		return this->m_HitBoxControl.GetLineHit(StartPos, pEndPos);
	}
	void SetPos(Util::VECTOR3D MyPos) noexcept {
		this->m_MyPosTarget = MyPos - Util::VECTOR3D::up() * Scale3DRate;
		if (!BackGround::Instance()->CheckLine(MyPos + Util::VECTOR3D::up() * Scale3DRate, &m_MyPosTarget)) {
			this->m_MyPosTarget = MyPos;
		}
		MyMat = Util::Matrix4x4::Mtrans(GetTargetPos());
		m_Rot = Util::Matrix3x3::Get33DX(MyMat);
	}
public:
	void Load_Sub(void) noexcept override {
		this->m_runfootID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/runfoot.wav", true);
		this->m_standupID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/standup.wav", true);
		Load_Chara();
	}
	void Init_Sub(void) noexcept override {
		this->m_HitBoxControl.Init();
		this->m_Speed = 0.f;
		Init_Chara();
	}
	void Update_Sub(void) noexcept override {
		//ヒットボックス
		this->m_HitBoxControl.Update(this, 1.0f);
		Update_Chara();
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Draw_Sub(void) const noexcept override {
		//hitbox描画
		GetModel().DrawModel();
#if DRAW_HITBOX
		this->m_HitBoxControl.Draw();
#endif
		/*
		auto Pos = (GetTargetPos() + Util::VECTOR3D::up() * (1.f * Scale3DRate));

		int X = this->m_TargetPathPlanningIndex;
		Util::VECTOR3D Vec3D = this->m_PathChecker.GetNextPoint(Pos, &X);

		DxLib::DrawLine3D(Vec3D.get(), Pos.get(), DxLib::GetColor(255, 0, 0));

		for (const auto& m : BackGround::Instance()->GetWayPoint()->GetWayPoints()) {
			Util::VECTOR3D Vec1 = m.GetPos();
			//if (!((Vec1 - Pos).magnitude() < 10.f * Scale3DRate)) { continue; }
			for (int K = 0; K < 8; K++) {
				int LinkIndex = m.GetLinkPolyIndex(K);
				if (LinkIndex == -1) { continue; }
				Util::VECTOR3D Vec2 = BackGround::Instance()->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(LinkIndex)).GetPos();

				DxLib::DrawLine3D(Vec1.get(), Vec2.get(), DxLib::GetColor(255, 255, 0));
			}
			DrawCube3D(
				m.GetMinPos().get(),
				m.GetMaxPos().get(),
				DxLib::GetColor(255, 0, 0),
				DxLib::GetColor(255, 0, 0),
				false
			);
		}
		this->m_PathChecker.Draw();
		//*/
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
	}
public:
	virtual void Load_Chara(void) noexcept = 0;
	virtual void Init_Chara(void) noexcept = 0;
	virtual void Update_Chara(void) noexcept = 0;
};

class Character :public CharacterCommon {
	Util::VECTOR3D		m_HandRad = Util::VECTOR3D::zero();
	Util::VECTOR3D		m_HandRad2 = Util::VECTOR3D::zero();
	Util::VECTOR3D		m_Normal = Util::VECTOR3D::up();
	Util::VECTOR3D		m_NormalR = Util::VECTOR3D::up();
	std::array<float, static_cast<int>(CharaAnim::Max)>		m_AnimPer{};
	std::array<float, static_cast<int>(CharaStyle::Max)>	m_StylePer{};
	CharaStyle												m_CharaStyle{ CharaStyle::Stand };
	Util::VECTOR3D											m_AimPoint;
	float				m_YradR{};
	float				m_AnimChangePer{};
	float				m_SwitchPer{};
	float				m_WalkEyeRad{};
	float				m_YradProne{};
	float				m_RadLimit{};
	float				m_PunchTimer{};
	float				m_PunchPower{};
	bool				m_PrevIsFPSView{};
	bool				m_IsFPS{};
	bool				m_IsActive{};
	bool				m_AnimMoving{ false };
	bool				m_ShotSwitch{ false };
	bool				m_PunchSwitch{ false };
	bool				m_PunchAttack{ false };
	char		padding[1]{};
	Sound::SoundUniqueID	m_heartID{ InvalidID };
	Sound::SoundUniqueID	m_PunchID{ InvalidID };
	Sound::SoundUniqueID	m_KickID{ InvalidID };
	Sound::SoundUniqueID HitHumanID{ InvalidID };
	int					m_StandAnimIndex{};
	int					m_WalkAnimIndex{};
	int					m_RunAnimIndex{};
	int					m_HaveHandgunAnimIndex{};
	int					m_HaveRifleAnimIndex{};
	int					m_ReloadHandgunAnimIndex{};
	int					m_ReloadRifleAnimIndex{};
	int					m_Prev{};
	int					m_Now{};
	int					m_Equip{ InvalidID };
	int					m_PrevEquip{ InvalidID };
	char		padding2[4]{};
	GunParam			m_Handgun{};
	GunParam			m_Maingun{};
public:
	Character(void) noexcept {}
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;
	virtual ~Character(void) noexcept {}
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
	bool IsFPSView(void) const noexcept { return this->m_IsFPS; }
	bool IsShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
	bool IsFreeView(void) const noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		return KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim) && !IsFPSView();
	}
	float GetSpeedMax(void) const noexcept {
		switch (this->m_CharaStyle) {
		case CharaStyle::Run:
			return 4.5f * Scale3DRate * DeltaTime;
			break;
		case CharaStyle::Squat:
			return 1.0f * Scale3DRate * DeltaTime;
			break;
		case CharaStyle::Prone:
			return 0.5f * Scale3DRate * DeltaTime;
			break;
		case CharaStyle::Stand:
		case CharaStyle::Max:
		default:
			return 2.5f * Scale3DRate * DeltaTime;
			break;
		}
	}
	void SetArmAnim(int Index) noexcept {
		this->m_Now = Index;
		if (this->m_Prev != this->m_Now && !m_AnimMoving) {
			Util::HandAnimPool::Instance()->GoTimeStart(this->m_Now);
			this->m_AnimChangePer = 0.f;
			this->m_AnimMoving = true;
		}
	}
	void SetIsActive(bool value) noexcept { this->m_IsActive = value; }
	void SetSubGunUniqueID(int value) noexcept { this->m_Handgun.m_UniqueID = value; }
	void SetMainGunUniqueID(int value) noexcept { this->m_Maingun.m_UniqueID = value; }
	bool GetIsReloading(void) const noexcept { return (this->m_Handgun.GetIsReload() || this->m_Handgun.GetIsCocking() || this->m_Maingun.GetIsReload() || this->m_Maingun.GetIsCocking()); }
	int GetEquip(void) const noexcept { return this->m_Equip; }
	void SetEquip(int value) noexcept { this->m_Equip = value; }

	bool ChanChangeWeapon() const noexcept {
		return  (!this->GetIsReloading() && !this->m_PunchSwitch);
	}
public:
	void CheckDraw_Sub(void) noexcept override {
		if (!IsFPSView()) {
			if (IsFreeView()) {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				Util::VECTOR3D Near = ConvScreenPosToWorldPos(VGet(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()), 0.f));
				Util::VECTOR3D Far = ConvScreenPosToWorldPos(VGet(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()), 1.f));
				Util::VECTOR3D Now = GetMat().pos();
				this->m_AimPoint = Util::Lerp(Near, Far, (Now.y - Near.y) / (Far.y - Near.y));
			}
		}
	}
public:
	void Load_Chara(void) noexcept override {
		this->m_heartID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/heart.wav", true);
		this->HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);

		this->m_PunchID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Punch.wav", true);
		this->m_KickID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Kick.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
	}
	void Init_Chara(void) noexcept override {
		this->m_StandAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Stand.anh");
		this->m_WalkAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Walk.anh");
		this->m_RunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/Run.anh");

		this->m_HaveHandgunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/HaveHandgun.anh");
		this->m_HaveRifleAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/HaveRifle.anh");

		this->m_ReloadHandgunAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/ReloadHandgun.anh");
		this->m_ReloadRifleAnimIndex = Util::HandAnimPool::Instance()->Add("data/CharaAnim/ReloadRifle.anh");

		//Util::HandAnimPool::Instance()->SetAnimSpeed(this->m_WalkAnimIndex, 2.5f);

		this->m_Prev = this->m_StandAnimIndex;
		SetArmAnim(this->m_StandAnimIndex);
		this->m_AnimChangePer = 1.f;
		this->m_AnimMoving = false;
	}
	void Update_Chara(void) noexcept override;
};
