#include "EarlyCharacter.hpp"

const Util::Matrix4x4 EarlyCharacter::GetEyeMat(void) const noexcept {
	Util::Matrix4x4 Mat = GetFrameLocalWorldMatrix(static_cast<int>(EarlyCharaFrame::Eye));
	return Mat;
}

void EarlyCharacter::Update_Sub(void) noexcept {
	//前準備
	this->m_PathUpdateTimer = std::max(this->m_PathUpdateTimer - DeltaTime, 0.f);
	if (this->m_PathUpdateTimer <= 0.f) {
		this->m_PathUpdateTimer += 1.f;
		this->ChangePoint();
	}
	bool IsMove = false;
	//
	{
		bool IsMoving = false;
		if (SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).GetPer() > 0.5f) {
			IsMoving = true;
			float Time = SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).GetTime();

			//L
			if ((9.0f < Time && Time < 10.0f)) {
				if (this->m_FootSoundID != 4) {
					this->m_FootSoundID = 4;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(192);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				}
			}
			//R
			if ((27.0f < Time && Time < 28.0f)) {
				if (this->m_FootSoundID != 5) {
					this->m_FootSoundID = 5;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(192);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				}
			}
		}
		if (!IsMoving) {
			m_FootSoundID = -1;
		}
	}
	// 左右回転
	{
		Util::VECTOR2D Vec = Util::VECTOR2D::zero();
		{
			auto Pos = (m_MyPosTarget + Util::VECTOR3D::up() * (1.f * Scale3DRate));
			Util::VECTOR3D Vec3D = m_PathChecker.GetNextPoint(Pos, &this->m_TargetPathPlanningIndex) - Pos;
			Vec.x = Vec3D.x;
			Vec.y = Vec3D.z;
		}
		IsMove = true;// Vec.magnitude() >= 0.5f * Scale3DRate;

		//IsMove = Vec.magnitude() >= 0.1f * Scale3DRate;
		{
			Util::VECTOR3D Vec3D = m_MyTarget - m_MyPosTarget; Vec3D.y = 0.f;
			IsMove = Vec3D.magnitude() >= 0.5f * Scale3DRate;
		}
		if (Vec.magnitude() >= 1.f * Scale3DRate) {
			Vec = Vec.normalized();
		}
		//歩くより早く移動する場合
		if (m_Speed > (3.f * Scale3DRate * DeltaTime)) {
			Util::Easing(&m_VecR, Vec, 0.975f);
		}
		else {
			Util::Easing(&m_VecR, Vec, 0.9f);
		}

		if (m_VecR.sqrMagnitude() > 0.f) {
			m_YradDif = std::atan2f(-m_VecR.x, -m_VecR.y) - m_Rad.y;
			if (m_YradDif > 0.f) {
				while (true) {
					if (m_YradDif < DX_PI_F) { break; }
					m_YradDif -= DX_PI_F * 2.f;
				}
			}
			if (m_YradDif < 0.f) {
				while (true) {
					if (m_YradDif > -DX_PI_F) { break; }
					m_YradDif += DX_PI_F * 2.f;
				}
			}
			float Per = std::clamp(m_YradDif / Util::deg2rad(15.f), -1.f, 1.f);

			IsMove = std::fabsf(Per) < 0.3f;


			float YradAdd = 0.f;
			if (std::fabsf(Per) > 0.01f) {
				float Power = 1.f;
				YradAdd = Per * Power * Util::deg2rad(720.f)* DeltaTime;
			}
			m_Rad.y += YradAdd;
			{
				float Power = 1.f;
				Util::Easing(&m_Rad.z, YradAdd * Power, 0.9f);
			}
			m_Rad.y = Util::AngleRange360(m_Rad.y);
		}

		Util::Easing(&m_Rad.x, 0.f, 0.9f);
	}
	// 進行方向に前進
	Util::Easing(&m_Speed, IsMove ? GetSpeedMax() : 0.f, 0.9f);

	// 移動ベクトルを加算した仮座標を作成
	Util::VECTOR3D PosBuffer;
	{
		PosBuffer = m_MyPosTarget + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * -m_Speed, GetMat().rotation());
	}
	// 壁判定
	std::vector<const Draw::MV1*> addonColObj;
	BackGround::Instance()->CheckWall(m_MyPosTarget, &PosBuffer, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate, addonColObj);// 現在地から仮座標に進んだ場合
	// 地面判定
	PosBuffer.y = PosBuffer.y - 0.1f * Scale3DRate;
	if (!BackGround::Instance()->CheckLine(PosBuffer + Util::VECTOR3D::up() * Scale3DRate, &PosBuffer)) {
		// ヒットしていない際は落下させる
		m_Vector.y -= GravAccel;
		PosBuffer.y += m_Vector.y;
	}
	else {
		m_Vector.y = 0.f;
	}
	// 仮座標を反映
	m_Speed = std::clamp((m_MyPosTarget - PosBuffer).magnitude(), 0.f, m_Speed);
	m_MyPosTarget = PosBuffer;
	Util::VECTOR3D MyPos = GetMat().pos();
	Util::Easing(&MyPos, m_MyPosTarget, 0.9f);

	SetMatrix(
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), m_Rad.z) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_Rad.y) * Util::Matrix4x4::Mtrans(MyPos)
	);

	//移動割合
	Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);

	m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Upper)] = 1.f;
	//停止
	m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Stand)] = (1.f - m_MovePer);
	//移動
	m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Walk)] = m_MovePer;

	//アニメアップデート
	for (size_t loop = 0; loop < static_cast<size_t>(EarlyCharaAnim::Max); ++loop) {
		SetAnim(loop).SetPer(m_AnimPer[loop]);
	}
	SetAnim(static_cast<int>(EarlyCharaAnim::Upper)).Update(true, 1.f);
	SetAnim(static_cast<int>(EarlyCharaAnim::Stand)).Update(true, 1.f);
	SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
	ModelID.FlipAnimAll();
}