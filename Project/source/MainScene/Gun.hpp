#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BaseObject.hpp"
#include "BackGround.hpp"

#include "../Util/Util.hpp"
#include "../Util/Sound.hpp"

#include "../MainScene/PlayerManager.hpp"

class Case : public BaseObject {
public:
	Case(void) noexcept {}
	Case(const Case&) = delete;
	Case(Case&&) = delete;
	Case& operator=(const Case&) = delete;
	Case& operator=(Case&&) = delete;
	virtual ~Case(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char*		GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	bool m_IsPlaySound{};
	char		padding[3]{};
	Sound::SoundUniqueID FallCaseID{ InvalidID };
	char		padding2[4]{};
public:
	void Set(const Util::Matrix4x4& Case, const Util::Matrix4x4& CaseVec) noexcept {
		SetMatrix(Case);
		this->Vector = (CaseVec.pos() - Case.pos()).normalized() *
			(static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate * DeltaTime);
		this->YVecAdd = 0.f;
		this->Timer = 1.f;
		this->m_IsPlaySound = false;
	}
public:
	void Load_Sub(void) noexcept override {
		FallCaseID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/FallCase.wav", true);
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DeltaTime, 0.f);
		this->YVecAdd -= GravAccel;
		this->Vector.y += this->YVecAdd;
		Util::VECTOR3D Target = this->GetMat().pos() + this->Vector;
		Util::VECTOR3D Normal;
		if (BackGround::Instance()->CheckLine(this->GetMat().pos(), &Target, &Normal)) {
			this->YVecAdd = 0.f;
			this->Vector = Util::VECTOR3D::Reflect(this->Vector, Normal) * 0.8f;
			if (!this->m_IsPlaySound) {
				this->m_IsPlaySound = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, FallCaseID)->Play3D(Target, 10.f * Scale3DRate);
			}
		}

		this->SetMatrix(
			this->GetMat().rotation() *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(this->Vector, this->GetMat().zvec()).normalized(), Util::deg2rad(1800.f) * DeltaTime) *
			Util::Matrix4x4::Mtrans(Target)
		);
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		ModelID.DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		ModelID.DrawModel();
	}
	void ShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		ModelID.DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		ModelID.Dispose();
	}
};
class AmmoHitEffect : public BaseObject {
public:
	AmmoHitEffect(void) noexcept {}
	AmmoHitEffect(const AmmoHitEffect&) = delete;
	AmmoHitEffect(AmmoHitEffect&&) = delete;
	AmmoHitEffect& operator=(const AmmoHitEffect&) = delete;
	AmmoHitEffect& operator=(AmmoHitEffect&&) = delete;
	virtual ~AmmoHitEffect(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	char		padding[4]{};
public:
	void Set(const Util::VECTOR3D& Pos, const Util::VECTOR3D& Normal) noexcept {
		SetMatrix(Util::Matrix4x4::Mtrans(Pos));
		this->Vector = Normal;
		this->Vector =
			Util::Matrix4x4::Vtrans(this->Vector,
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::forward(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f))) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::up(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f)))
			) *
			(static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate * DeltaTime);
		this->YVecAdd = 0.f;
		this->Timer = 1.f;

	}
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DeltaTime, 0.f);
		this->YVecAdd -= GravAccel;
		this->Vector.y += this->YVecAdd;
		Util::VECTOR3D Target = GetMat().pos() + this->Vector;
		Util::VECTOR3D Normal;
		if (BackGround::Instance()->CheckLine(GetMat().pos(), &Target, &Normal)) {
			this->YVecAdd = 0.f;
			this->Vector = Util::VECTOR3D::Reflect(this->Vector, Normal) * 0.8f;
		}
		SetMatrix(GetMat().rotation() * Util::Matrix4x4::Mtrans(Target));
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		ModelID.DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		DxLib::SetUseLighting(FALSE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->Timer), 0, 255));
		DxLib::DrawCapsule3D(
			(GetMat().pos() - this->Vector * std::clamp(this->Timer, 0.f, 1.f)).get(),
			GetMat().pos().get(),
			0.01f * Scale3DRate / 2.f,
			6,
			DxLib::GetColor(255, 255, 128),
			DxLib::GetColor(255, 255, 0),
			true
		);
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};
class ShotEffect : public BaseObject {
public:
	ShotEffect(void) noexcept {}
	ShotEffect(const ShotEffect&) = delete;
	ShotEffect(ShotEffect&&) = delete;
	ShotEffect& operator=(const ShotEffect&) = delete;
	ShotEffect& operator=(ShotEffect&&) = delete;
	virtual ~ShotEffect(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::Matrix4x4				m_FireMat{};
	Util::Matrix4x4				m_SmokeMat{};
	const Draw::GraphHandle*	m_SmokeGraph{};
	float						m_FireOpticalPer{};
	float						m_SmokePer{};
	float						AnimPer = 0.f;
	char		padding[4]{};
public:
	void Set(const Util::Matrix4x4& Muzzle) noexcept {
		m_FireMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(GetRand(90)));
		m_SmokeMat = Muzzle;
		m_SmokePer = 0.f;
		AnimPer = 0.f;
	}
public:
	void Load_Sub(void) noexcept override {
		m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Smoke.png")->Get();
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		this->m_SmokePer = std::clamp(this->m_SmokePer + DeltaTime / 0.5f, 0.f, 1.f);
		AnimPer = std::clamp(AnimPer + DeltaTime / 0.1f, 0.f, 1.f);
		if (0.0f <= AnimPer && AnimPer <= 0.3f) {
			m_FireOpticalPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.f, 0.3f, AnimPer));
		}
		if (0.3f <= AnimPer && AnimPer <= 1.f) {
			m_FireOpticalPer = Util::Lerp(1.f, 0.f, Util::GetPer01(0.3f, 1.f, AnimPer));
		}
		SetMatrix(
			Util::Matrix4x4::GetScale(AnimPer) *
			m_FireMat *
			m_SmokeMat
		);
		ModelID.SetOpacityRate(m_FireOpticalPer);
	}
	void SetShadowDraw_Sub(void) const noexcept override {
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		DxLib::SetUseLighting(FALSE);
		float Per = std::sin(Util::deg2rad(180.f * m_SmokePer));
		if (Per > 0.f) {
			for (int loop = 0; loop < 3; ++loop) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(64.f * Per));
				DxLib::DrawBillboard3D(
					(m_SmokeMat.pos() - m_SmokeMat.zvec() * (static_cast<float>(10 + (2 - loop) * 10) / 100.f * Scale3DRate * m_SmokePer)).get(),
					0.5f,
					0.5f,
					0.2f * Scale3DRate * m_SmokePer,
					Util::deg2rad(180.f * m_SmokePer) * ((loop % 2 == 0) ? 1.f : -1.f),
					m_SmokeGraph->get(),
					true
				);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		if (m_FireOpticalPer > 0.f) {
			ModelID.DrawModel();
		}
		DxLib::SetUseLighting(TRUE);
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
		ModelID.Dispose();
	}
};
class Ammo : public BaseObject {
public:
	Ammo(void) noexcept {}
	Ammo(const Ammo&) = delete;
	Ammo(Ammo&&) = delete;
	Ammo& operator=(const Ammo&) = delete;
	Ammo& operator=(Ammo&&) = delete;
	virtual ~Ammo(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	float DrawTimer{};
	Sound::SoundUniqueID HitGroundID{ InvalidID };
	char		padding[4]{};
	std::array<std::shared_ptr<AmmoHitEffect>, 10>	m_AmmoEffectPer{};
public:
	void Set(const Util::Matrix4x4& Muzzle) noexcept {
		SetMatrix(Muzzle);
		this->Vector = Muzzle.zvec() * (-100.f * Scale3DRate * DeltaTime);
		this->YVecAdd = 0.f;
		this->Timer = 1.f;
		this->DrawTimer = this->Timer + 1.f;
	}
private:
	void SetAmmo(const Util::VECTOR3D& pos) noexcept {
		this->Vector = pos - GetMat().pos();
		this->Timer = 0.f;
		this->DrawTimer = this->Timer + 1.f;
		for (auto& ae : m_AmmoEffectPer) {
			ae->Set(
				pos,
				this->Vector.normalized() * -1.f
			);
		}
	}
public:
	void Load_Sub(void) noexcept override {
		HitGroundID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitGround.wav", true);
	}
	void Init_Sub(void) noexcept override {
		for (auto& ae : m_AmmoEffectPer) {
			ae = std::make_shared<AmmoHitEffect>();
			ObjectManager::Instance()->InitObject(ae);
		}
	}
	void Update_Sub(void) noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		this->DrawTimer = std::max(this->DrawTimer - DeltaTime, 0.f);
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DeltaTime, 0.f);
		this->YVecAdd -= GravAccel;
		this->Vector.y += this->YVecAdd;
		Util::VECTOR3D Target = GetMat().pos() + this->Vector;
		if (BackGround::Instance()->CheckLine(GetMat().pos(), &Target)) {
			SetAmmo(Target);
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(Target, 10.f * Scale3DRate);
		}
		for (auto& c : PlayerManager::Instance()->SetCharacter()) {
			if (c->CheckHit(GetMat().pos(), &Target)) {
				c->SetDownTop();
				if (this->Timer != 0.f) {
					SetAmmo(Target);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(Target, 10.f * Scale3DRate);
				}
			}
		}
		SetMatrix(GetMat().rotation() *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(this->Vector, GetMat().zvec()).normalized(), Util::deg2rad(1800.f) * DeltaTime) *
			Util::Matrix4x4::Mtrans(Target));
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		DxLib::SetUseLighting(FALSE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->DrawTimer), 0, 255));
		DxLib::DrawCapsule3D(
			(GetMat().pos() - this->Vector * std::clamp(this->DrawTimer, 0.f, 1.f)).get(),
			GetMat().pos().get(),
			0.009f * Scale3DRate / 2.f,
			6,
			DxLib::GetColor(255, 255, 128),
			DxLib::GetColor(255, 255, 0),
			true
		);
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
		for (auto& ae : m_AmmoEffectPer) {
			ae.reset();
		}
	}
};

enum class MagFrame {
	Center,
	LeftHandPos,
	LeftHandZVec,
	LeftHandYVec,
	Max,
};
static const char* MagFrameName[static_cast<int>(MagFrame::Max)] = {
	"センター",
	"LeftHand",
	"LeftHandZVec",
	"LeftHandYVec",
};

class Magazine : public BaseObject {
public:
	Magazine(void) noexcept {}
	Magazine(const Magazine&) = delete;
	Magazine(Magazine&&) = delete;
	Magazine& operator=(const Magazine&) = delete;
	Magazine& operator=(Magazine&&) = delete;
	virtual ~Magazine(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(MagFrame::Max); }
	const char* GetFrameStr(int id) noexcept override { return MagFrameName[id]; }
private:
	Sound::SoundUniqueID UnLoadMagID{ InvalidID };
	Sound::SoundUniqueID LoadMagID{ InvalidID };

	float				m_MagInPer{};
	float				m_MagLoadPer{};
	Util::Matrix4x4		m_MagLoad{};
	bool				m_IsMagUnloadSound{};
	bool				m_IsMagLoadSound{};
	char		padding[2]{};

	int					m_AmmoNum{};
	int					m_AmmoTotal{ 17 };
	char		padding2[4]{};
public:
	auto			GetMagLeftHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(MagFrame::LeftHandPos)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(MagFrame::LeftHandYVec)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(MagFrame::LeftHandZVec)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	void SetMagMat(const Util::Matrix4x4& MagMat, const Util::Matrix4x4& MagWelMat) noexcept {
		Util::Matrix4x4 Mag = MagMat;
		Mag = Util::Lerp(Mag, MagWelMat, m_MagInPer);
		Mag = Util::Lerp(Mag, m_MagLoad, m_MagLoadPer);
		SetMatrix(Mag);
	}
	void SetMagLoadMat(const Util::Matrix4x4& value) { m_MagLoad = value; }
	void SetMagPer(float LoadPer, float ReloadPer) {
		m_MagInPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.f, 0.3f, LoadPer));
		m_MagLoadPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.3f, 1.f, LoadPer));

		if (ReloadPer == 0.f) {
			m_IsMagUnloadSound = false;
			m_IsMagLoadSound = false;
		}
		if (ReloadPer >= 0.1f) {
			if (!m_IsMagUnloadSound) {
				m_IsMagUnloadSound = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, UnLoadMagID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.2f * Scale3DRate);
				m_AmmoNum = 0;
			}
		}
		if (ReloadPer >= 0.8f) {
			if (!m_IsMagLoadSound) {
				m_IsMagLoadSound = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, LoadMagID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.2f * Scale3DRate);
				m_AmmoNum = m_AmmoTotal;
			}
		}
	}
	int GetAmmoNum() const { return m_AmmoNum; }
	int GetAmmoTotal() const { return m_AmmoTotal; }

	void SetMag(int value) { m_AmmoNum = value; }
	bool SubAmmo() {
		if (m_AmmoNum > 0) {
			--m_AmmoNum;
			return true;
		}
		return false;
	}
public:
	void Load_Sub(void) noexcept override {
		UnLoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/3.wav", true);
		LoadMagID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/4.wav", true);
	}
	void Init_Sub(void) noexcept override {
		SetMag(GetAmmoTotal());
	}
	void Update_Sub(void) noexcept override {
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
	LeftHandPosPull,
	LeftHandZVecPull,
	LeftHandYVecPull,
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
	"LeftHandPull",
	"LeftHandZVecPull",
	"LeftHandYVecPull",
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
	std::array<std::shared_ptr<ShotEffect>, 3>	m_ShotEffect{};
	int					m_ShotEffectID{};
	char		padding[4]{};

	std::array<std::shared_ptr<Case>, 3>	m_CasePer{};
	int					m_CaseID{};
	char		padding2[4]{};

	std::array<std::shared_ptr<Ammo>, 5>	m_AmmoPer{};
	int					m_AmmoID{};
	char		padding3[4]{};

	std::shared_ptr<Magazine>				m_Magazine{};

	std::array<float, static_cast<int>(GunAnim::Max)>		m_AnimPer{};
	Sound::SoundUniqueID SlideCloseID{ InvalidID };
	Sound::SoundUniqueID ShotID{ InvalidID };
	Sound::SoundUniqueID ShotSPID{ InvalidID };
	//char		padding[4]{};

	const Draw::GraphHandle* m_Pic{};
	bool				m_IsSlideCloseSound{};
	bool				m_Trigger{};
	bool				m_ChamberIn{};
	bool				m_CanShot{ true };
	char		padding4[4]{};
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
	auto			GetBaseLeftHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandPos)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandYVec)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandZVec)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetPullLeftHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandPosPull)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandYVecPull)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::LeftHandZVecPull)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetBaseRightHandMat(void) const noexcept {
		Util::VECTOR3D HandPos = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::RightHandPos)).pos();
		Util::VECTOR3D Handyvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::RightHandYVec)).pos() - HandPos;
		Util::VECTOR3D Handzvec = GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::RightHandZVec)).pos() - HandPos;
		return Util::Matrix4x4::Axis1(Handyvec.normalized(), Handzvec.normalized() * -1.f, HandPos);
	}
	auto			GetMagLeftHandMat(void) const noexcept { return m_Magazine->GetMagLeftHandMat(); }
	auto			GetAmmoNum() const { return m_Magazine->GetAmmoNum() + (m_ChamberIn ? 1 : 0); }
	auto			GetAmmoTotal() const { return m_Magazine->GetAmmoTotal() + 1; }
	auto			CanReload() const { return GetAmmoNum() != GetAmmoTotal(); }
	auto			CanShot() const { return m_CanShot && m_ChamberIn; }
	const auto*		GetPicPtr(void) const noexcept { return m_Pic; }
public:
	void ShotStart() {
		if (m_CanShot) {
			m_CanShot = false;
			m_ChamberIn = false;
			m_AnimPer[static_cast<int>(GunAnim::Shot)] = 1.f;
			SetAnim(static_cast<int>(GunAnim::Shot)).SetTime(0.f);

			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, ShotID)->Play3D(GetMat().pos(), 50.f * Scale3DRate);

			m_ShotEffect.at(static_cast<size_t>(m_ShotEffectID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Muzzle)));
			++m_ShotEffectID %= static_cast<int>(m_ShotEffect.size());

			m_CasePer.at(static_cast<size_t>(m_CaseID))->Set(
				GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Case)),
				GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::CaseVec)));
			++m_CaseID %= static_cast<int>(m_CasePer.size());

			m_AmmoPer.at(static_cast<size_t>(m_AmmoID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::Muzzle)));
			++m_AmmoID %= static_cast<int>(m_AmmoPer.size());
		}
	}
	void SetMagLoadMat(const Util::Matrix4x4& value) { m_Magazine->SetMagLoadMat(value); }
	void SetMagPer(float LoadPer, float ReloadPer) { m_Magazine->SetMagPer(LoadPer, ReloadPer); }
	void SetCockingPer(float value) {
		if (value == 0.f) {
			m_IsSlideCloseSound = false;
		}
		if (value >= 0.5f) {
			if (!m_IsSlideCloseSound) {
				m_IsSlideCloseSound = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, SlideCloseID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.2f * Scale3DRate);
				if (m_Magazine->SubAmmo()) {
					m_ChamberIn = true;
				}
			}
		}
	}
	void SetTrigger(bool value) { m_Trigger = value; }
public:
	void Load_Sub(void) noexcept override {
		SlideCloseID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/1.wav", true);
		ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/2.wav", true);
		ShotSPID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/gun/auto1911/5.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);

		m_Pic = Draw::GraphPool::Instance()->Get(this->GetFilePath() + "pic.png")->Get();

		ObjectManager::Instance()->LoadModel("data/9x19/");
		ObjectManager::Instance()->LoadModel("data/FireEffect/");
		ObjectManager::Instance()->LoadModel("data/Mag/");
	}
	void Init_Sub(void) noexcept override {
		for (auto& s : m_CasePer) {
			s = std::make_shared<Case>();
			ObjectManager::Instance()->InitObject(s, s, "data/9x19/");
		}
		for (auto& s : m_ShotEffect) {
			s = std::make_shared<ShotEffect>();
			ObjectManager::Instance()->InitObject(s, s, "data/FireEffect/");
		}
		for (auto& s : m_AmmoPer) {
			s = std::make_shared<Ammo>();
			ObjectManager::Instance()->InitObject(s);
		}
		
		m_Magazine = std::make_shared<Magazine>();
		ObjectManager::Instance()->InitObject(m_Magazine, m_Magazine,"data/Mag/");

		SetMagPer(0.f, 0.f);
		m_ChamberIn = true;
	}
	void Update_Sub(void) noexcept override {
		if (!m_CanShot) {
			if (SetAnim(static_cast<int>(GunAnim::Shot)).GetTimePer() >= 1.f) {
				m_AnimPer[static_cast<int>(GunAnim::Shot)] = 0.f;
				m_CanShot = true;
				if (m_Magazine->SubAmmo()) {
					m_ChamberIn = true;
				}
			}
		}

		Util::Easing(&m_AnimPer[static_cast<size_t>(GunAnim::Slide)], (!m_ChamberIn && m_CanShot) ? 1.f : 0.f, 0.8f);

		Util::Easing(&m_AnimPer[static_cast<size_t>(GunAnim::Trigger)], m_Trigger ? 1.f : 0.f, 0.8f);
		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(GunAnim::Max); ++loop) {
			SetAnim(loop).SetPer(m_AnimPer[loop]);
		}
		SetAnim(static_cast<int>(GunAnim::Shot)).Update(false, 2.f);
		ModelID.FlipAnimAll();
		//
		m_Magazine->SetMagMat(GetMat(),
			Util::Matrix4x4::RotVec2(GetFrameBaseLocalMat(static_cast<int>(GunFrame::MagIn)).pos(), Util::VECTOR3D::down()) *
			GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::MagWel))
		);
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

		for (auto& s : m_CasePer) {
			s.reset();
		}
		for (auto& s : m_ShotEffect) {
			s.reset();
		}
		for (auto& s : m_AmmoPer) {
			s.reset();
		}
		m_Magazine.reset();

	}
};
