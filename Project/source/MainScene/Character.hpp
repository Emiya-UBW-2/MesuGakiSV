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
#include "../Draw/MV1.hpp"

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
	Max,
};
static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
	"センター",
	"上半身",
	"上半身2",
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
};

struct Frames {
	int first{};
	Util::Matrix4x4 second{};
	Util::Matrix4x4 third{};
};

class BaseObject {
protected:
	std::vector<Frames>							m_Frames;
public:
	BaseObject(void) noexcept {}
	BaseObject(const BaseObject&) = delete;
	BaseObject(BaseObject&&) = delete;
	BaseObject& operator=(const BaseObject&) = delete;
	BaseObject& operator=(BaseObject&&) = delete;
	virtual ~BaseObject(void) noexcept {}
public:
	virtual int	GetFrameNum(void) noexcept { return 0; }
	virtual const char* GetFrameStr(int) noexcept { return nullptr; }
public:
	bool			HaveFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first != InvalidID; }
	const auto& GetFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first; }
	const auto& GetFrameBaseLocalMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].second; }
	const auto& GetFrameBaseLocalWorldMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].third; }
};

class Character :public BaseObject {
	Draw::MV1 ModelID{};
	Util::Matrix4x4 MyMat;
	Util::VECTOR3D MyPosTarget = Util::VECTOR3D::zero();
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
	char		padding[4]{};
	Util::Matrix4x4 m_RightPos;
	Util::Matrix4x4 m_LeftPos;
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
	const Util::Matrix4x4& GetMat(void) const noexcept { return MyMat; }
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
public:
	void Load(void) noexcept {
		Draw::MV1::Load("data/Soldier/model.mv1", &ModelID);
	}
	void Init(void) noexcept {
		Draw::MV1::SetAnime(&ModelID, ModelID);
		Speed = 0.f;

		// フレーム
		{
			this->m_Frames.clear();
			if (GetFrameNum() > 0) {
				this->m_Frames.resize(static_cast<size_t>(GetFrameNum()));
				for (auto& f : this->m_Frames) {
					f.first = InvalidID;
				}
				size_t count = 0;
				for (int frameNum = 0, Max = this->ModelID.GetFrameNum(); frameNum < Max; ++frameNum) {
					if (Util::UTF8toSjis(this->ModelID.GetFrameName(frameNum)) == GetFrameStr(static_cast<int>(count))) {
						// そのフレームを登録
						this->m_Frames[count].first = frameNum;
						this->m_Frames[count].second = Util::Matrix4x4::Mtrans(this->ModelID.GetFrameLocalMatrix(this->m_Frames[count].first).pos());
						this->m_Frames[count].third = this->ModelID.GetFrameLocalWorldMatrix(this->m_Frames[count].first);
					}
					else if (frameNum < Max - 1) {
						continue;// 飛ばす
					}
					++count;
					frameNum = 0;
					if (count >= this->m_Frames.size()) {
						break;
					}
				}
			}
		}
	}
	void Update(bool isActive) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
		bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);
		bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
		bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
		//
		if (isActive) {
			if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Squat)) {
				if (m_CharaStyle != CharaStyle::Squat) {
					m_CharaStyle = CharaStyle::Squat;
				}
				else {
					m_CharaStyle = CharaStyle::Stand;
				}
			}
			if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run)) {
				m_CharaStyle = CharaStyle::Run;
			}
			if (KeyMngr->GetBattleKeyReleaseTrigger(Util::EnumBattle::Run)) {
				m_CharaStyle = CharaStyle::Stand;
			}
		}
		// 左右回転
		{
			Util::VECTOR2D Vec = Util::VECTOR2D::zero();
			if (isActive) {
				if (LeftKey) {
					Vec += Util::VECTOR2D::left();
				}
				if (RightKey) {
					Vec += Util::VECTOR2D::right();
				}
				if (UpKey) {
					Vec += Util::VECTOR2D::up();
				}
				if (DownKey) {
					Vec += Util::VECTOR2D::down();
				}
			}
			VecR = Util::Lerp(VecR, Vec, 1.f - 0.9f);

			if (VecR.sqrMagnitude() > 0.f) {
				float dif = std::atan2f(-VecR.x, -VecR.y) - Yrad;
				if (dif > 0.f) {
					while (true) {
						if (dif < DX_PI_F) { break; }
						dif -= DX_PI_F * 2.f;
					}
				}
				if (dif < 0.f) {
					while (true) {
						if (dif > -DX_PI_F) { break; }
						dif += DX_PI_F * 2.f;
					}
				}
				float Per = std::clamp(dif / Util::deg2rad(15.f),-1.f,1.f);

				float YradAdd = 0.f;
				if (std::fabsf(Per) > 0.01f) {
					float Power = 1.f;
					switch (m_CharaStyle) {
					case CharaStyle::Run:
						Power = 1.5f;
						break;
					case CharaStyle::Squat:
						Power = 0.5f;
						break;
					case CharaStyle::Stand:
						Power = 1.f;
						break;
					case CharaStyle::Max:
					default:
						break;
					}
					YradAdd = Per * Util::deg2rad(720.f) * Power / 60.f;
				}
				Yrad += YradAdd;
				{
					float Power = 1.f;
					switch (m_CharaStyle) {
					case CharaStyle::Run:
						Power = 1.f;
						break;
					case CharaStyle::Squat:
						Power = 0.1f;
						break;
					case CharaStyle::Stand:
						Power = 0.25f;
						break;
					case CharaStyle::Max:
					default:
						break;
					}
					Zrad = Util::Lerp(Zrad, YradAdd * Power, 1.f - 0.9f);
				}
				if (Yrad > 0.f) {
					while (true) {
						if (Yrad < DX_PI_F * 2.f) { break; }
						Yrad -= DX_PI_F * 2.f;
					}
				}
				if (Yrad < 0.f) {
					while (true) {
						if (Yrad > -DX_PI_F * 2.f) { break; }
						Yrad += DX_PI_F * 2.f;
					}
				}
			}
		}
		// 進行方向に前進
		Speed = Util::Lerp(Speed, (LeftKey || RightKey || UpKey || DownKey || !isActive) ? GetSpeedMax() : 0.f, 1.f - 0.9f);

		// 移動ベクトルを加算した仮座標を作成
		Util::VECTOR3D PosBuffer = MyPosTarget + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * -Speed, MyMat.rotation());
		// 壁判定
		std::vector<const Draw::MV1*> addonColObj;
		BackGround::Instance()->CheckWall(MyPosTarget, &PosBuffer, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate, addonColObj);// 現在地から仮座標に進んだ場合
		// 地面判定
		PosBuffer.y = PosBuffer.y - 0.1f * Scale3DRate;
		if (!BackGround::Instance()->CheckLine(PosBuffer + Util::VECTOR3D::up() * Scale3DRate, &PosBuffer)) {
			// ヒットしていない際は落下させる
			m_YVec -= (9.8f * Scale3DRate) / (60.f * 60.f);
			PosBuffer.y += m_YVec;
		}
		else {
			m_YVec = 0.f;
		}
		// 仮座標を反映
		Speed = std::clamp((MyPosTarget - PosBuffer).magnitude(), 0.f, Speed);
		MyPosTarget = PosBuffer;
		Util::VECTOR3D MyPos = MyMat.pos();
		MyPos = Util::Lerp(MyPos, MyPosTarget, 1.f - 0.9f);

		MyMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Zrad) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Yrad) * Util::Matrix4x4::Mtrans(MyPos);
		ModelID.SetMatrix(MyMat);

		//移動割合
		MovePer = Util::Lerp(MovePer, GetSpeed() / GetSpeedMax(), 1.f - 0.9f);
		for (size_t loop = 0; loop < static_cast<size_t>(CharaStyle::Max); ++loop) {
			m_StylePer.at(loop) = std::clamp(m_StylePer.at(loop) + ((m_CharaStyle == static_cast<CharaStyle>(loop)) ? 1.f : -1.f) / 60.f / 0.3f, 0.f, 1.f);
		}

		//停止
		m_AnimPer[static_cast<size_t>(CharaAnim::Squat)] = (1.f - MovePer) * m_StylePer.at(static_cast<size_t>(CharaStyle::Squat));
		m_AnimPer[static_cast<size_t>(CharaAnim::Stand)] = (1.f - MovePer) * std::max(m_StylePer.at(static_cast<size_t>(CharaStyle::Stand)), m_StylePer.at(static_cast<size_t>(CharaStyle::Run)));
		//移動
		m_AnimPer[static_cast<size_t>(CharaAnim::SquatWalk)] = MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Squat));
		m_AnimPer[static_cast<size_t>(CharaAnim::Walk)] = MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Stand));
		m_AnimPer[static_cast<size_t>(CharaAnim::Run)] = MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Run));
		//回転
		{
			float Per = 0.f;
			if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim)) {
				Per = -Util::VECTOR3D::SignedAngle(MyMat.zvec() * -1.f, m_AimPoint - MyMat.pos(), Util::VECTOR3D::up()) / Util::deg2rad(90);
				Per = std::clamp(Per, -1.f, 1.f);
			}
			m_AnimPer[static_cast<size_t>(CharaAnim::Flip)] = Util::Lerp(m_AnimPer[static_cast<size_t>(CharaAnim::Flip)], Per, 1.f - 0.9f);
		}
		//
		m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_1)] = 1.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_2)] = 0.3f;
		m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_3)] = 1.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_4)] = 1.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_5)] = 1.f;

		m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_1)] = 1.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_2)] = 0.3f;
		m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_3)] = 1.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_4)] = 1.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_5)] = 1.f;

		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
			ModelID.SetAnim(loop).SetPer(m_AnimPer[loop]);
		}
		ModelID.SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
		ModelID.SetAnim(static_cast<int>(CharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
		ModelID.SetAnim(static_cast<int>(CharaAnim::Run)).Update(true, GetSpeed() * 0.75f);
		ModelID.SetAnim(static_cast<int>(CharaAnim::Squat)).Update(true, GetSpeed() * 2.75f);
		ModelID.SetAnim(static_cast<int>(CharaAnim::SquatWalk)).Update(true, GetSpeed() * 2.75f);
		ModelID.FlipAnimAll();

		Util::Matrix4x4 HandBaseMat = ModelID.GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)));

		m_RightPos = 
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-90)) *
			HandBaseMat.rotation() * 
			Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(Util::VECTOR3D::vget(-0.5f, -0.7f, 0.f) * Scale3DRate, HandBaseMat.rotation()) + HandBaseMat.pos());
		m_LeftPos =
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-90)) *
			HandBaseMat.rotation() *
			Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(Util::VECTOR3D::vget(0.5f, -0.7f, 0.f) * Scale3DRate, HandBaseMat.rotation()) + HandBaseMat.pos());

		{
			Draw::IK_RightArm(
				&ModelID,
				GetFrame(static_cast<int>(CharaFrame::RightArm)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
				GetFrame(static_cast<int>(CharaFrame::RightArm2)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
				GetFrame(static_cast<int>(CharaFrame::RightWrist)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
				m_RightPos);
		}
		{
			Draw::IK_LeftArm(
				&ModelID,
				GetFrame(static_cast<int>(CharaFrame::LeftArm)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
				GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
				GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
				m_LeftPos);
		}
	}
	void Dispose(void) noexcept {
		ModelID.Dispose();
	}

	void SetShadowDraw(void) const noexcept {
		ModelID.DrawModel();
	}
	void CheckDraw(void) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		Util::VECTOR3D Near = ConvScreenPosToWorldPos(VGet(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()), 0.f));
		Util::VECTOR3D Far = ConvScreenPosToWorldPos(VGet(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()), 1.f));
		Util::VECTOR3D Now = MyMat.pos();
		m_AimPoint = Util::Lerp(Near, Far, (Now.y - Near.y) / (Far.y - Near.y));
	}
	void Draw(void) const noexcept {
		ModelID.DrawModel();
	}
	void ShadowDraw(void) const noexcept {
		ModelID.DrawModel();
	}

};
