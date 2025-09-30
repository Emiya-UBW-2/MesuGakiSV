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
