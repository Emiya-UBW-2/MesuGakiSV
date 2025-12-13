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

#include "Character.hpp"

enum class EarlyCharaAnim {
	Stand,//立ち
	Walk,//歩き

	Upper,//左に向く

	DownTop,//仰向きに倒れる
	WakeTop,//仰向きから立つ

	DownBottom,//仰向きに倒れる
	WakeBottom,//仰向きから立つ

	Punch,

	Max,
};

class MarkDraw {
	const Draw::GraphHandle*	m_Graph{};
	float						m_Timer{};
	float						m_Per{};
public:
	MarkDraw(void) noexcept {}
	MarkDraw(const MarkDraw&) = delete;
	MarkDraw(MarkDraw&&) = delete;
	MarkDraw& operator=(const MarkDraw&) = delete;
	MarkDraw& operator=(MarkDraw&&) = delete;
	virtual ~MarkDraw(void) noexcept {}
public:
	bool IsActive() const noexcept { return this->m_Timer != 0.f; }
	void SetActive() noexcept { this->m_Timer = 3.f; }
	void SetDisActive() noexcept { this->m_Timer = 0.f; }
public:
	void Init(std::string_view FilePath)noexcept {
		this->m_Graph = Draw::GraphPool::Instance()->Get(FilePath)->Get();
		this->m_Timer = 0.f;
		this->m_Per = 0.f;
	}
	void Update() noexcept {
		this->m_Timer = std::max(this->m_Timer - DeltaTime, 0.f);
		Util::Easing(&this->m_Per, this->m_Timer, 0.9f);
	}
	void Draw(const  Util::VECTOR3D& Pos, int ColorR, int ColorG, int ColorB) const noexcept {
		if (this->m_Per > 0.f) {
			SetUseLighting(false);
			DxLib::SetDrawBright(ColorR, ColorG, ColorB);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(this->m_Per * 5.f * 255.f), 0, 255));
			DxLib::DrawBillboard3D(
				(Pos + Util::VECTOR3D::up() * (0.25f * Scale3DRate * std::clamp(this->m_Per * 5.f, 0.f, 1.f))).get(),
				0.5f,
				0.5f,
				0.25f * Scale3DRate,
				0.f,
				this->m_Graph->get(),
				true
			);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			SetUseLighting(true);
			DxLib::SetDrawBright(255, 255, 255);
		}
	}
};

class EarlyCharacter :public CharacterCommon {
	std::array<float, static_cast<int>(EarlyCharaAnim::Max)>		m_AnimPer{};

	Util::VECTOR3D		m_MyTarget = Util::VECTOR3D::zero();
	int					m_TargetPathPlanningIndex{ 0 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	float				m_PathUpdateTimer{ 0.f };

	SpecialAction		m_DownTop{};
	SpecialAction		m_DownBottom{};

	float				m_KeepPower{ 0.f };
	float				m_KeepTimer{ 0.f };

	Util::VECTOR3D		m_HitVec{};
	float				m_HitPower{ 0.f };
	float				m_HitBack{ 0.f };

	Util::VECTOR3D		m_DownVec{};
	float				m_DownPower{ 0.f };
	char		padding[4]{};

	Sound::SoundUniqueID	m_PunchID{ InvalidID };
	Sound::SoundUniqueID	m_HitHumanID{ InvalidID };
	Sound::SoundUniqueID	m_DownHumanID{ InvalidID };
	Sound::SoundUniqueID	m_ArmlockStartID{ InvalidID };
	Sound::SoundUniqueID	m_ArmlockID{ InvalidID };

	MarkDraw			m_Alert{};

	SpecialAction		m_Punch{};

	float				m_AttackCoolDown{ 0.f };
	bool				m_PunchAttack{ false };
	char		padding3[3]{};

	float				m_CanSeePer{};
	Util::VECTOR2D		m_UIPos{};
	bool				m_CanSeeUI{ false };
	char		padding4[3]{};

	float				m_AlertTimer{};
	float				m_AlertAnyTimer{};
	bool				m_IsTurn{ false };
	char		padding5[2]{};
public:
	EarlyCharacter(void) noexcept {}
	EarlyCharacter(const EarlyCharacter&) = delete;
	EarlyCharacter(EarlyCharacter&&) = delete;
	EarlyCharacter& operator=(const EarlyCharacter&) = delete;
	EarlyCharacter& operator=(EarlyCharacter&&) = delete;
	virtual ~EarlyCharacter(void) noexcept {}
public:
	float GetSpeedMax(void) const noexcept { return 2.f * Scale3DRate * DeltaTime; }
	auto GetCanSeeUI(void) const noexcept { return this->m_CanSeeUI; }
	auto GetUIPos(void) const noexcept { return this->m_UIPos; }
	auto GetCanSeePer(void) const noexcept { return this->m_CanSeePer; }
	auto IsDown(void) const noexcept { return (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End); }
	auto IsWatching(void) const noexcept { return this->m_AlertTimer != 0.f; }
public:
	void		SetHit(const Util::VECTOR3D& Vec, float Power) noexcept {
		this->m_KeepPower = std::clamp(this->m_KeepPower + Power, 0.f, 1.f);
		this->m_KeepTimer = 1.f;
		if (this->m_KeepPower >= 1.f) {
			//倒れる
			Util::VECTOR3D A = GetMat().zvec(); A.y = 0.f;
			Util::VECTOR3D B = Vec; B.y = 0.f;
			if (Util::VECTOR3D::Dot(A, B) > 0.f) {
				SetDownTop(B.normalized());
			}
			else {
				SetDownBottom(B.normalized());
			}
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_DownHumanID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
		}
		else {
			//のけぞり
			Util::VECTOR3D A = GetMat().zvec(); A.y = 0.f;
			Util::VECTOR3D B = Vec; B.y = 0.f;
			if (Util::VECTOR3D::Dot(A, B) > 0.f) {
				this->m_HitVec = (B.normalized());
				this->m_HitPower = 0.5f;
				this->m_HitBack = 1.f;
			}
			else {
				this->m_HitVec = (B.normalized()) * -1.f;
				this->m_HitPower = -0.5f;
				this->m_HitBack = -1.f;
			}
		}
	}
	//倒れる
	void		SetDownTop(const Util::VECTOR3D& Vec) noexcept {
		this->m_DownVec = Vec * -1.f;
		if (!this->m_DownTop.IsActive()) {
			SetAnim(static_cast<int>(EarlyCharaAnim::DownTop)).SetTime(0.f);
			this->m_DownTop.m_Time = 3.f;
			this->m_DownPower = -1.f;
		}
		this->m_DownTop.SetActive();
	}
	//倒れる
	void		SetDownBottom(const Util::VECTOR3D& Vec) noexcept {
		this->m_DownVec = Vec;
		if (!this->m_DownBottom.IsActive()) {
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).SetTime(0.f);
			this->m_DownBottom.m_Time = 3.f;
			this->m_DownPower = 1.f;
		}
		this->m_DownBottom.SetActive();
	}
public:
	void CheckDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();

		Util::VECTOR3D Pos1 = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Head)).pos();
		Util::VECTOR3D Pos2 = GetCameraPosition();

		//Pos2 = Pos1 + (Pos2 - Pos1) * 0.5f;

		if (BackGround::Instance()->CheckLine(Pos1, &Pos2) == 0) {
			auto Pos = ConvWorldPosToScreenPos(Pos1.get());
			if (0.0f < Pos.z && Pos.z < 1.0f) {
				this->m_UIPos.x = Pos.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				this->m_UIPos.y = Pos.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
				this->m_CanSeeUI |= true;
			}
		}
	}
public:
	bool IsPlayer(void) noexcept override { return false; }

	void Load_Chara(void) noexcept override {
		this->m_DownHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/DownHuman.wav", true);
		this->m_ArmlockID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Armlock.wav", true);
		this->m_HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);
		this->m_PunchID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Punch.wav", true);

		this->m_Alert.Init("data/Image/Alert.png");
	}
	void Init_Chara(void) noexcept override {
		this->m_PathUpdateTimer = 1.f;

		this->m_DownTop.Init();
		this->m_DownBottom.Init();
		this->m_Punch.Init();

		this->m_DownPower = 0.f;
	}
	void Update_Chara(void) noexcept override;
	void Draw_Chara(void) const noexcept override {
		auto Pos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Head)).pos();
		this->m_Alert.Draw(Pos, 255, 0, 0);
	}
	void Dispose_Chara(void) noexcept override {
	}
};
