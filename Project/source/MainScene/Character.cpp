#include "Character.hpp"

#include "Gun.hpp"

const Util::Matrix4x4 Character::GetEyeMat(void) const noexcept {
	Util::Matrix4x4 Mat = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye));

	Mat = Mat.rotation() *
		Util::Matrix4x4::Mtrans(
			Util::Matrix4x4::Vtrans(
				Util::VECTOR3D::vget(
					std::cosf(m_WalkRad),
					std::fabsf(std::sinf(m_WalkRad)) * -0.25f,
					0.f
				) * (MovePer * (0.03f * Scale3DRate)),
				Mat.rotation()
			) +
			Mat.pos());
	{
		auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
		Mat = Util::Lerp(Mat, gun->GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::ADSPos)), m_Handgun.m_GunADSPer);
	}
	{
		auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
		Mat = Util::Lerp(Mat, gun->GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::ADSPos)), m_Maingun.m_GunADSPer);
	}
	return Mat;
}

void Character::Update_Sub(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
	bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);
	bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
	bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
	uint8_t				MoveKey = 0;
	MoveKey |= LeftKey ? (1 << 0) : 0;
	MoveKey |= RightKey ? (1 << 1) : 0;
	MoveKey |= UpKey ? (1 << 2) : 0;
	MoveKey |= DownKey ? (1 << 3) : 0;
	//
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Walk)) {
		m_IsFPS ^= 1;
	}
	//
	if (m_IsActive) {
		if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Squat)) {
			if (m_CharaStyle == CharaStyle::Stand || m_CharaStyle == CharaStyle::Squat) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
			}
			if (m_CharaStyle != CharaStyle::Squat) {
				m_CharaStyle = CharaStyle::Squat;
			}
			else {
				m_CharaStyle = CharaStyle::Stand;
			}
		}
		else {
			if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run)) {
				if (m_CharaStyle == CharaStyle::Squat) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
				}
				m_CharaStyle = CharaStyle::Run;
			}
			if (KeyMngr->GetBattleKeyReleaseTrigger(Util::EnumBattle::Run)) {
				m_CharaStyle = CharaStyle::Stand;
			}
		}
		if (m_Handgun.GetIsEquip()) {
			SetArmAnim(m_HaveHandgunAnimIndex);
		}
		else if (m_Maingun.GetIsEquip()) {
			SetArmAnim(m_HaveRifleAnimIndex);
		}
		else {
			if (MovePer > 0.5f) {
				if (m_CharaStyle == CharaStyle::Run) {
					SetArmAnim(m_RunAnimIndex);
				}
				else {
					SetArmAnim(m_WalkAnimIndex);
				}
			}
			else {
				SetArmAnim(m_StandAnimIndex);
			}
		}
	}
	//
	{
		bool IsMoving = false;
		switch (m_CharaStyle) {
		case CharaStyle::Run:
			if (SetAnim(static_cast<int>(CharaAnim::Run)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(CharaAnim::Run)).GetTime();

				//L
				if ((9.0f / 35.f * 16.f < Time && Time < 10.0f / 35.f * 16.f)) {
					if (this->m_FootSoundID != 0) {
						this->m_FootSoundID = 0;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->SetLocalVolume(255);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
				//R
				if ((27.0f / 35.f * 16.f < Time && Time < 28.0f / 35.f * 16.f)) {
					if (this->m_FootSoundID != 1) {
						this->m_FootSoundID = 1;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->SetLocalVolume(255);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
			}
			break;
		case CharaStyle::Squat:
			if (SetAnim(static_cast<int>(CharaAnim::SquatWalk)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(CharaAnim::SquatWalk)).GetTime();

				//L
				if ((9.0f < Time && Time < 10.0f)) {
					if (this->m_FootSoundID != 2) {
						this->m_FootSoundID = 2;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->SetLocalVolume(92);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 3) {
						this->m_FootSoundID = 3;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->SetLocalVolume(92);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
			}
			break;
		case CharaStyle::Stand:
			if (SetAnim(static_cast<int>(CharaAnim::Walk)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(CharaAnim::Walk)).GetTime();

				//L
				if ((9.0f < Time && Time < 10.0f)) {
					if (this->m_FootSoundID != 4) {
						this->m_FootSoundID = 4;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->SetLocalVolume(192);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 5) {
						this->m_FootSoundID = 5;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->SetLocalVolume(192);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
			}
			break;
		case CharaStyle::Max:
		default:
			break;
		}
		if (!IsMoving) {
			m_FootSoundID = -1;
		}
	}
	// 左右回転

	int LookX = 0;
	int LookY = 0;

	if (IsFPSView()) {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		if (m_PrevIsFPSView != IsFPSView()) {
			DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
		}
		int MX = DrawerMngr->GetMousePositionX();
		int MY = DrawerMngr->GetMousePositionY();
		DxLib::GetMousePoint(&MX, &MY);
		LookX = MX - DrawerMngr->GetWindowDrawWidth() / 2;
		LookY = MY - DrawerMngr->GetWindowDrawHeight() / 2;
		DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
		DxLib::SetMouseDispFlag(false);
	}
	m_PrevIsFPSView = IsFPSView();
	if (IsFPSView()) {
		float YradAdd = Util::deg2rad(static_cast<float>(LookX) / 30.f);
		float XradAdd = Util::deg2rad(static_cast<float>(LookY) / 30.f);

		Yrad += YradAdd;
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

		Xrad = std::clamp(Xrad + XradAdd, Util::deg2rad(-80), Util::deg2rad(80));
		VecR = Util::VECTOR2D::zero();
	}
	else {
		Util::VECTOR2D Vec = Util::VECTOR2D::zero();
		if (m_IsActive) {
			if ((MoveKey & (1 << 0)) != 0) {
				Vec += Util::VECTOR2D::left();
			}
			if ((MoveKey & (1 << 1)) != 0) {
				Vec += Util::VECTOR2D::right();
			}
			if ((MoveKey & (1 << 2)) != 0) {
				Vec += Util::VECTOR2D::up();
			}
			if ((MoveKey & (1 << 3)) != 0) {
				Vec += Util::VECTOR2D::down();
			}
		}
		//歩くより早く移動する場合
		if (Speed > (3.f * Scale3DRate / 60.f)) {
			VecR = Util::Lerp(VecR, Vec, 1.f - 0.975f);
		}
		else {
			VecR = Util::Lerp(VecR, Vec, 1.f - 0.9f);
		}

		if (VecR.sqrMagnitude() > 0.f) {
			m_YradDif = std::atan2f(-VecR.x, -VecR.y) - Yrad;
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
					if (Speed > (3.f * Scale3DRate / 60.f)) {
						Power = 1.f;
					}
					else {
						Power = 0.1f;
					}
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

		Xrad = Util::Lerp(Xrad, 0.f, 1.f - 0.9f);
	}

	bool IsAim = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim);
	if (m_CharaStyle == CharaStyle::Run) {
		IsAim = false;
	}

	if (IsFPSView()) {
		m_Handgun.m_GunReadyPer = Util::Lerp(m_Handgun.m_GunReadyPer,
			m_Handgun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) ? 1.f : 0.f,
			1.f - 0.9f);
		m_Handgun.m_GunADSPer = Util::Lerp(m_Handgun.m_GunADSPer,
			m_Handgun.GetIsEquip() && IsAim ? 1.f : 0.f,
			1.f - 0.9f);

		m_Maingun.m_GunReadyPer = Util::Lerp(m_Maingun.m_GunReadyPer,
			m_Maingun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) ? 1.f : 0.f,
			1.f - 0.9f);
		m_Maingun.m_GunADSPer = Util::Lerp(m_Maingun.m_GunADSPer,
			m_Maingun.GetIsEquip() && IsAim ? 1.f : 0.f,
			1.f - 0.9f);
	}
	else {
		m_Handgun.m_GunReadyPer = Util::Lerp(m_Handgun.m_GunReadyPer,
			m_Handgun.GetIsEquip() && IsAim ? 1.f : 0.f,
			1.f - 0.9f);
		m_Handgun.m_GunADSPer = 0.f;

		m_Maingun.m_GunReadyPer = Util::Lerp(m_Maingun.m_GunReadyPer,
			m_Maingun.GetIsEquip() && IsAim ? 1.f : 0.f,
			1.f - 0.9f);
		m_Maingun.m_GunADSPer = 0.f;
	}
	//
	m_ShotSwitch = false;
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack)) {
		if (m_Handgun.GetIsEquip() && m_Handgun.m_GunReadyPer > 0.95f) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
			if (gun->CanShot()) {
				gun->ShotStart();
				m_ShotSwitch = true;
			}
		}
		if (m_Maingun.GetIsEquip() && m_Maingun.m_GunReadyPer > 0.95f) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
			if (gun->CanShot()) {
				gun->ShotStart();
				m_ShotSwitch = true;
			}
		}
	}


	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::E)) {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
		m_Handgun.SetIsEquip(!m_Handgun.GetIsEquip());
	}
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Q)) {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
		m_Maingun.SetIsEquip(!m_Maingun.GetIsEquip());
	}

	m_Handgun.Update();
	m_Maingun.Update();

	// 進行方向に前進
	Speed = Util::Lerp(Speed, ((MoveKey != 0) || !m_IsActive) ? GetSpeedMax() : 0.f, 1.f - 0.9f);

	// 移動ベクトルを加算した仮座標を作成
	Util::VECTOR3D PosBuffer;
	if (IsFPSView()) {
		Util::VECTOR3D Vec = Util::VECTOR3D::zero();
		if (m_IsActive) {
			if ((MoveKey & (1 << 0)) != 0) {
				Vec += Util::VECTOR3D::left();
			}
			if ((MoveKey & (1 << 1)) != 0) {
				Vec += Util::VECTOR3D::right();
			}
			if ((MoveKey & (1 << 2)) != 0) {
				Vec += Util::VECTOR3D::forward();
			}
			if ((MoveKey & (1 << 3)) != 0) {
				Vec += Util::VECTOR3D::back();
			}
		}
		if (Vec.sqrMagnitude() > 0.f) {
			Vec = Vec.normalized();
		}
		PosBuffer = MyPosTarget + Util::Matrix4x4::Vtrans(Vec * -Speed, MyMat.rotation());
	}
	else {
		PosBuffer = MyPosTarget + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * -Speed, MyMat.rotation());
	}
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

	SetMatrix(
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Zrad) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Yrad) * Util::Matrix4x4::Mtrans(MyPos)
	);

	//移動割合
	MovePer = Util::Lerp(MovePer, GetSpeed() / GetSpeedMax(), 1.f - 0.9f);

	if (MovePer > 0.01f) {
		m_WalkRad += Util::deg2rad(360) / 60.f;
	}
	else {
		m_WalkRad = 0.f;
	}

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
		if (!IsFPSView()) {
			if (IsFreeView() && (m_CharaStyle != CharaStyle::Run)) {
				Per = -Util::VECTOR3D::SignedAngle(MyMat.zvec() * -1.f, m_AimPoint - MyMat.pos(), Util::VECTOR3D::up()) / Util::deg2rad(90);
			}
			else {
				Per = m_YradDif / Util::deg2rad(90);
			}
		}
		m_SwitchPer = Util::Lerp(m_SwitchPer, (Per >= -0.5f) ? 1.f : 0.f, 1.f - 0.9f);
		m_AnimPer[static_cast<size_t>(CharaAnim::Flip)] = Util::Lerp(m_AnimPer[static_cast<size_t>(CharaAnim::Flip)], std::clamp(Per, -1.f, 1.f), 1.f - 0.9f);
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
		SetAnim(loop).SetPer(m_AnimPer[loop]);
	}
	SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
	SetAnim(static_cast<int>(CharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::Run)).Update(true, GetSpeed() * 0.75f);
	SetAnim(static_cast<int>(CharaAnim::Squat)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::SquatWalk)).Update(true, GetSpeed() * 2.75f);
	ModelID.FlipAnimAll();

	Util::Matrix4x4 HandBaseMat = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye));

	{
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper));
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper2));
		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -Xrad * 60.f / 100.f) *
			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper))
		);
		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper2),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -Xrad * 40.f / 100.f) *
			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper2))
		);
	}
	{
		Util::HandAnimPool::Instance()->Update();
		if (m_AnimMoving) {
			m_AnimChangePer = std::clamp(m_AnimChangePer + 1.f / 60.f / 0.25f, 0.f, 1.f);
			if (m_AnimChangePer >= 1.f) {
				m_AnimMoving = false;
				m_Prev = m_Now;
			}
		}
		Util::VRAnim	m_VRAnim = Util::VRAnim::LerpAnim(
			Util::HandAnimPool::Instance()->GetAnim(m_Prev).GetAnim(),
			Util::HandAnimPool::Instance()->GetAnim(m_Now).GetAnim(),
			m_AnimChangePer);

		Util::VRAnim Answer;
		{
			auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
			auto EyePos =
				Util::Lerp(
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosRight)),
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosLeft)),
					m_SwitchPer
				);
			Util::VRAnim	AimAnim;
			AimAnim.m_RightHandPos = EyePos.pos() * -1.f;
			AimAnim.m_RightRot = Util::Matrix3x3::Get33DX(EyePos);

			Answer = Util::VRAnim::LerpAnim(m_VRAnim, AimAnim, m_Handgun.m_GunReadyPer);
		}
		{
			auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
			auto EyePos = 
				Util::Lerp(
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosRight)),
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosLeft)),
					m_SwitchPer
				);
			Util::VRAnim	AimAnim;
			AimAnim.m_RightHandPos = EyePos.pos() * -1.f;
			AimAnim.m_RightRot = Util::Matrix3x3::Get33DX(EyePos);

			Answer = Util::VRAnim::LerpAnim(Answer, AimAnim, m_Maingun.m_GunReadyPer);
		}
		{
			Util::Matrix4x4 Mat = Answer.m_RightRot.Get44DX() * HandBaseMat.rotation() *
				Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(Answer.m_RightHandPos, HandBaseMat.rotation()) + HandBaseMat.pos());
			Mat = Util::Lerp(Util::Lerp(Mat, GetHolsterMat(), m_Handgun.m_Per), GetHolsterPullMat(), m_Handgun.m_PullPer);
			Mat = Util::Lerp(Util::Lerp(Mat, GetSlingMat(), m_Maingun.m_Per), GetSlingPullMat(), m_Maingun.m_PullPer);

			Draw::IK_RightArm(
				&ModelID,
				GetFrame(static_cast<int>(CharaFrame::RightArm)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
				GetFrame(static_cast<int>(CharaFrame::RightArm2)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
				GetFrame(static_cast<int>(CharaFrame::RightWrist)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
				Mat
			);
		}

		{
			Util::Matrix4x4 RightMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-90)) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(-90)) *
				GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::RightHandJoint));
			{
				Util::Matrix4x4 Mat = RightMat;
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
				if (m_Handgun.m_EquipPhase <= 1) {
					Mat = Util::Lerp(GetHolsterMat(), GetHolsterPullMat(), m_Handgun.m_GunPer);
				}
				gun->SetMatrix(Mat);
			}
			{
				Util::Matrix4x4 Mat = RightMat;
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
				if (m_Maingun.m_EquipPhase <= 1) {
					Mat = Util::Lerp(GetSlingMat(), GetSlingPullMat(), m_Maingun.m_GunPer);
				}
				gun->SetMatrix(Mat);
			}
		}

		{
			Util::Matrix4x4 LeftHandMat = m_VRAnim.m_LeftRot.Get44DX() * HandBaseMat.rotation() *
				Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(m_VRAnim.m_LeftHandPos, HandBaseMat.rotation()) + HandBaseMat.pos());
			{
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
				LeftHandMat = Util::Lerp(LeftHandMat, ((std::shared_ptr<Gun>&)gun)->GetBaseLeftHandMat(), m_Handgun.m_GunReadyPer);
			}
			{
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
				LeftHandMat = Util::Lerp(LeftHandMat, ((std::shared_ptr<Gun>&)gun)->GetBaseLeftHandMat(), m_Maingun.m_GunReadyPer);
			}

			Draw::IK_LeftArm(
				&ModelID,
				GetFrame(static_cast<int>(CharaFrame::LeftArm)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
				GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
				GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
				LeftHandMat
			);
		}

		if (false && (1.f - m_SwitchPer) > 0.f) {
			Util::Matrix4x4 RightHandMat = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::RightWrist));
			RightHandMat =
				Util::Matrix4x4::Axis1(Util::VECTOR3D::back(), Util::VECTOR3D::right()) *
				RightHandMat.rotation() *
				Util::Matrix4x4::Mtrans(RightHandMat.pos());


			{
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
				RightHandMat = Util::Lerp(RightHandMat, ((std::shared_ptr<Gun>&)gun)->GetBaseRightHandMat(), m_Handgun.m_GunReadyPer * (1.f - m_SwitchPer));
			}
			{
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
				RightHandMat = Util::Lerp(RightHandMat, ((std::shared_ptr<Gun>&)gun)->GetBaseRightHandMat(), m_Maingun.m_GunReadyPer * (1.f - m_SwitchPer));
			}

			Draw::IK_RightArm(
				&ModelID,
				GetFrame(static_cast<int>(CharaFrame::RightArm)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
				GetFrame(static_cast<int>(CharaFrame::RightArm2)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
				GetFrame(static_cast<int>(CharaFrame::RightWrist)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
				RightHandMat
			);

			Util::Matrix4x4 LeftHandMat = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::LeftWrist));
			LeftHandMat = 
				Util::Matrix4x4::Axis1(Util::VECTOR3D::back(), Util::VECTOR3D::left()) *
				LeftHandMat.rotation() *
				Util::Matrix4x4::Mtrans(LeftHandMat.pos());


			{
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.m_UniqueID));
				LeftHandMat = Util::Lerp(LeftHandMat, ((std::shared_ptr<Gun>&)gun)->GetMat(), m_Handgun.m_GunReadyPer * (1.f - m_SwitchPer));
			}
			{
				auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.m_UniqueID));
				LeftHandMat = Util::Lerp(LeftHandMat, ((std::shared_ptr<Gun>&)gun)->GetMat(), m_Maingun.m_GunReadyPer * (1.f - m_SwitchPer));
			}
			Draw::IK_LeftArm(
				&ModelID,
				GetFrame(static_cast<int>(CharaFrame::LeftArm)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
				GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
				GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
				LeftHandMat
			);
		}
	}
}