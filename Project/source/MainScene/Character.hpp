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

class Character {
	Draw::MV1 ModelID{};
	Util::Matrix4x4 MyMat;

	Util::VECTOR3D MyPosTarget = Util::VECTOR3D::zero();
	float Yrad = 0.f;
	float Speed = 0.f;

	Util::VECTOR2D VecR = Util::VECTOR2D::zero();
	char		padding[4]{};
public:
	Character(void) noexcept {}
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;
	virtual ~Character(void) noexcept {}
public:
	const Util::Matrix4x4& GetMat(void) const noexcept { return MyMat; }
	float GetSpeed(void) const noexcept { return Speed; }
	float GetSpeedMax(void) const noexcept { return 4.f * Scale3DRate / 60.f; }
public:
	void Load(void) noexcept {
		Draw::MV1::Load("data/Soldier/model_DISABLE.mv1", &ModelID);
	}
	void Init(void) noexcept {
		MyPosTarget = Util::VECTOR3D::vget(0.f, -50.f, 0.f) * Scale3DRate;
		BackGround::Instance()->CheckLine(Util::VECTOR3D::vget(MyPosTarget.x, 0.f * Scale3DRate, MyPosTarget.z), &MyPosTarget);
		MyMat = Util::Matrix4x4::Mtrans(MyPosTarget);
	}
	void Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
		bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);
		bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
		bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
		// 左右回転
		{
			Util::VECTOR2D Vec = Util::VECTOR2D::zero();
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
				if (std::fabsf(Per) > 0.01f) {
					Yrad += Per * Util::deg2rad(720.f) / 60.f;
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
		Speed = Util::Lerp(Speed, (LeftKey || RightKey || UpKey || DownKey) ? GetSpeedMax() : 0.f, 1.f - 0.9f);

		// 移動ベクトルを加算した仮座標を作成
		Util::VECTOR3D PosBuffer = MyPosTarget + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * -Speed, MyMat.rotation());
		// 壁判定
		std::vector<const Draw::MV1*> addonColObj;
		BackGround::Instance()->CheckWall(MyPosTarget, &PosBuffer, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.7f * Scale3DRate, addonColObj);// 現在地から仮座標に進んだ場合
		// 地面判定
		PosBuffer.y = PosBuffer.y - 0.1f * Scale3DRate;
		if (!BackGround::Instance()->CheckLine(PosBuffer + Util::VECTOR3D::up() * Scale3DRate, &PosBuffer)) {
			// ヒットしていない際は落下させる(加速度は無視)
			PosBuffer.y -= (9.8f * Scale3DRate) / (60.f * 60.f);
		}
		// 仮座標を反映
		MyPosTarget = PosBuffer;
		Util::VECTOR3D MyPos = MyMat.pos();
		MyPos = Util::Lerp(MyPos, MyPosTarget, 1.f - 0.9f);

		MyMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Yrad) * Util::Matrix4x4::Mtrans(MyPos);
		ModelID.SetMatrix(MyMat);
	}
	void Dispose(void) noexcept {
		ModelID.Dispose();
	}

	void SetShadowDraw(void) const noexcept {
		ModelID.DrawModel();
	}
	void Draw(void) const noexcept {
		ModelID.DrawModel();
	}
	void ShadowDraw(void) const noexcept {
		ModelID.DrawModel();
	}

};
