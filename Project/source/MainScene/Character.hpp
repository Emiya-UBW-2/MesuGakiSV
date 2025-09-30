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

	float Yrad = 0.f;
	float Speed = 0.f;
public:
	Character(void) noexcept {}
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;
	virtual ~Character(void) noexcept {}
public:
	const Util::Matrix4x4& GetMat(void) const noexcept { return MyMat; }
public:
	void Load(void) noexcept {
		Draw::MV1::Load("data/Soldier/model_DISABLE.mv1", &ModelID);
	}
	void Init(void) noexcept {
		Util::VECTOR3D MyPos = Util::VECTOR3D::vget(0.f, -50.f, 0.f) * Scale3DRate;
		BackGround::Instance()->CheckLine(Util::VECTOR3D::vget(MyPos.x, 0.f * Scale3DRate, MyPos.z), &MyPos);
		MyMat = Util::Matrix4x4::Mtrans(MyPos);
	}
	void Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		// 左右回転
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::A)) {
			Yrad -= 1.f / 60.f;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::D)) {
			Yrad += 1.f / 60.f;
		}
		// 進行方向に前進後退
		float Spd = 0.f;
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::W)) {
			Spd = 2.f * Scale3DRate / 60.f;
		}
		else if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::S)) {
			Spd = -2.f * Scale3DRate / 60.f;
		}
		else {
			Spd = 0.f;
		}
		Speed = Util::Lerp(Speed, Spd, 1.f - 0.9f);

		Util::VECTOR3D MyPos = MyMat.pos();
		// 移動ベクトルを加算した仮座標を作成
		Util::VECTOR3D PosBuffer = MyPos + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * -Speed, MyMat.rotation());
		// 壁判定
		std::vector<const Draw::MV1*> addonColObj;
		BackGround::Instance()->CheckWall(MyPos, &PosBuffer, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.7f * Scale3DRate, addonColObj);// 現在地から仮座標に進んだ場合
		// 地面判定
		PosBuffer.y = PosBuffer.y - 0.1f * Scale3DRate;
		if (!BackGround::Instance()->CheckLine(PosBuffer + Util::VECTOR3D::up() * Scale3DRate, &PosBuffer)) {
			// ヒットしていない際は落下させる(加速度は無視)
			PosBuffer.y -= (9.8f * Scale3DRate) / (60.f * 60.f);
		}
		// 仮座標を反映
		MyPos = PosBuffer;

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
