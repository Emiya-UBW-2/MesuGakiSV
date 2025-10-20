#include "Character.hpp"

#include "Gun.hpp"

void GunParam::Update() noexcept {
	auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(GetUniqueID()));
	if (this->m_IsEquip) {
		switch (this->m_EquipPhase) {
		case 0:
			this->m_Per = std::clamp(this->m_Per + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_Per >= 1.f) {
				this->m_EquipPhase = 1;
			}
			break;
		case 1:
			this->m_GunPer = std::clamp(this->m_GunPer + DeltaTime / 0.1f, 0.f, 1.f);
			this->m_Per = std::clamp(this->m_Per - DeltaTime / 0.1f, 0.f, 1.f);
			this->m_PullPer = std::clamp(this->m_PullPer + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_PullPer >= 1.f) {
				this->m_EquipPhase = 2;
			}
			break;
		case 2:
			this->m_Per = 0.f;
			this->m_PullPer = std::clamp(this->m_PullPer - DeltaTime / 0.1f, 0.f, 1.f);
			break;
		default:
			break;
		}
		if (m_IsGunLoad) {
			m_GunLoadTimer = std::clamp(m_GunLoadTimer + DeltaTime, 0.f, m_GunLoadTimerMax);
			if (0.f <= GetReloadPer() && GetReloadPer() <= 0.1f) {
				m_GunLoadHandPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.f, 0.1f, GetReloadPer()));
			}
			if (0.1f <= GetReloadPer() && GetReloadPer() <= 0.3f) {
				m_GunLoadPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.1f, 0.3f, GetReloadPer()));
			}
			if (0.6f <= GetReloadPer() && GetReloadPer() <= 0.85f) {
				m_GunLoadPer = Util::Lerp(1.f, 0.f, Util::GetPer01(0.6f, 0.85f, GetReloadPer()));
			}
			if (0.9f <= GetReloadPer() && GetReloadPer() <= 1.f) {
				m_GunLoadHandPer = Util::Lerp(1.f, 0.f, Util::GetPer01(0.9f, 1.f, GetReloadPer()));
			}
			if (GetReloadPer() == 1.f) {
				m_IsGunLoad = false;
				//コッキングが必要な時はコッキングする
				if (!gun->CanShot()) {
					m_IsCocking = true;
				}
			}
		}
		else {
			m_GunLoadTimer = 0.f;
			m_GunLoadPer = 0.f;
			m_GunLoadHandPer = 0.f;
		}
		if (m_IsCocking) {
			m_CockingTimer = std::clamp(m_CockingTimer + DeltaTime, 0.f, m_CockingTimerMax);
			if (GetCockingPer() == 1.f) {
				m_IsCocking = false;
			}
		}
		else {
			m_CockingTimer = 0.f;
		}
		Util::Easing(&m_GunPullPer, m_IsCocking ? 1.f : 0.f, 0.9f);
	}
	else {
		switch (this->m_EquipPhase) {
		case 2:
			this->m_PullPer = std::clamp(this->m_PullPer + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_PullPer >= 1.f) {
				this->m_EquipPhase = 1;
			}
			break;
		case 1:
			this->m_GunPer = std::clamp(this->m_GunPer - DeltaTime / 0.1f, 0.f, 1.f);
			this->m_PullPer = std::clamp(this->m_PullPer - DeltaTime / 0.1f, 0.f, 1.f);
			this->m_Per = std::clamp(this->m_Per + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_Per >= 1.f) {
				this->m_EquipPhase = 0;
			}
			break;
		case 0:
			this->m_PullPer = 0.f;
			this->m_Per = std::clamp(this->m_Per - DeltaTime / 0.1f, 0.f, 1.f);
			break;
		default:
			break;
		}
		m_GunLoadPer = 0.f;
		m_GunLoadHandPer = 0.f;
	}
	gun->SetMagPer(m_GunLoadPer, GetReloadPer());
	gun->SetCockingPer(GetCockingPer());
}

const Util::Matrix4x4 Character::GetEyeMat(void) const noexcept {
	Util::Matrix4x4 Mat = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye));

	Mat = Mat.rotation() *
		Util::Matrix4x4::Mtrans(
			Util::Matrix4x4::Vtrans(
				Util::VECTOR3D::vget(
					std::cosf(m_WalkRad),
					std::fabsf(std::sinf(m_WalkRad)) * -0.25f,
					0.f
				) * (m_MovePer * (0.03f * Scale3DRate)),
				Mat.rotation()
			) +
			Mat.pos());
	{
		auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
		Mat = Util::Lerp(Mat, gun->GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::ADSPos)), m_Handgun.GetADSPer());
	}
	{
		auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
		Mat = Util::Lerp(Mat, gun->GetFrameLocalWorldMatrix(static_cast<int>(GunFrame::ADSPos)), m_Maingun.GetADSPer());
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
		if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Prone)) {
			if (m_CharaStyle == CharaStyle::Stand || m_CharaStyle == CharaStyle::Prone) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
			}
			if (m_CharaStyle != CharaStyle::Prone) {
				m_CharaStyle = CharaStyle::Prone;
			}
			else {
				m_CharaStyle = CharaStyle::Stand;
			}
		}
		else if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Squat)) {
			if (m_CharaStyle == CharaStyle::Stand || m_CharaStyle == CharaStyle::Squat) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
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
				if (m_CharaStyle == CharaStyle::Prone) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
				}
				if (m_CharaStyle == CharaStyle::Squat) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
				}
				m_CharaStyle = CharaStyle::Run;
			}
			if (KeyMngr->GetBattleKeyReleaseTrigger(Util::EnumBattle::Run)) {
				m_CharaStyle = CharaStyle::Stand;
			}
		}

		if (m_Handgun.GetIsReload() || m_Handgun.GetIsCocking()) {
			SetArmAnim(m_ReloadHandgunAnimIndex);
		}
		else if (m_Maingun.GetIsReload() || m_Maingun.GetIsCocking()) {
			SetArmAnim(m_ReloadRifleAnimIndex);
		}
		else if (m_Handgun.GetIsEquip()) {
			SetArmAnim(m_HaveHandgunAnimIndex);
		}
		else if (m_Maingun.GetIsEquip()) {
			SetArmAnim(m_HaveRifleAnimIndex);
		}
		else {
			if (m_MovePer > 0.5f) {
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
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(255);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
				//R
				if ((27.0f / 35.f * 16.f < Time && Time < 28.0f / 35.f * 16.f)) {
					if (this->m_FootSoundID != 1) {
						this->m_FootSoundID = 1;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(255);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
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
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(92);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 3) {
						this->m_FootSoundID = 3;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(92);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
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
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(192);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 5) {
						this->m_FootSoundID = 5;
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->SetLocalVolume(192);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_runfootID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
					}
				}
			}
			break;
		case CharaStyle::Prone:
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
	}
	m_PrevIsFPSView = IsFPSView();

	float				PrevYradAdd = m_RadAdd.y;
	float				PrevXradAdd = m_RadAdd.x;
	if (IsFPSView()) {
		m_RadAdd.y = Util::deg2rad(static_cast<float>(LookX) / 30.f);
		m_RadAdd.x = Util::deg2rad(static_cast<float>(LookY) / 30.f);

		m_Rad.y += m_RadAdd.y;
		m_Rad.y = Util::AngleRange360(m_Rad.y);

		m_Rad.x = std::clamp(m_Rad.x + m_RadAdd.x, Util::deg2rad(-80), Util::deg2rad(80));
		m_VecR = Util::VECTOR2D::zero();
	}
	else {
		Camera::Camera3D::Instance()->StopCamShake();
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
		if (m_Speed > (3.f * Scale3DRate * DeltaTime)) {
			Util::Easing(&m_VecR, Vec, 0.975f);
		}
		else {
			Util::Easing(&m_VecR, Vec, 0.9f);
		}

		m_RadAdd.y = 0.f;
		m_RadAdd.x = 0.f;

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
				case CharaStyle::Prone:
					Power = 0.1f;
					break;
				case CharaStyle::Max:
				default:
					break;
				}
				m_RadAdd.y = Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			{
				float Power = 1.f;
				switch (m_CharaStyle) {
				case CharaStyle::Run:
					if (m_Speed > (3.f * Scale3DRate * DeltaTime)) {
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
				case CharaStyle::Prone:
					Power = 0.1f;
					break;
				case CharaStyle::Max:
				default:
					break;
				}
				Util::Easing(&m_Rad.z, m_RadAdd.y * Power, 0.9f);
			}
		}

		m_Rad.y += m_RadAdd.y;
		m_Rad.y = Util::AngleRange360(m_Rad.y);

		Util::Easing(&m_Rad.x, 0.f, 0.9f);
	}

	Util::Easing(&m_RadAddR.y, std::clamp(m_RadAdd.y - PrevYradAdd, -1.f, 1.f) * 30.f, 0.95f);
	Util::Easing(&m_RadAddR.x, std::clamp(m_RadAdd.x - PrevXradAdd, -1.f, 1.f) * 30.f, 0.95f);

	Util::Easing(&m_RadAddR2.y, m_RadAddR.y, 0.95f);
	Util::Easing(&m_RadAddR2.x, m_RadAddR.x, 0.95f);

	bool IsAim = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim);

	if (IsFPSView()) {
		Util::Easing(&m_Handgun.m_GunReadyPer,
			!m_Handgun.GetIsReload() && !m_Handgun.GetIsCocking() && m_Handgun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) ? 1.f : 0.f,
			0.9f);

		Util::Easing(&m_Maingun.m_GunReadyPer,
			!m_Maingun.GetIsReload() && !m_Maingun.GetIsCocking() && m_Maingun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) ? 1.f : 0.f,
			0.9f);
	}
	else {
		Util::Easing(&m_Handgun.m_GunReadyPer,
			(m_Handgun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) && IsAim) ? 1.f : 0.f,
			0.9f);

		Util::Easing(&m_Maingun.m_GunReadyPer,
			(m_Maingun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) && IsAim) ? 1.f : 0.f,
			0.9f);
	}

	Util::Easing(&m_Handgun.m_GunADSPer,
		!m_Handgun.GetIsReload() && !m_Handgun.GetIsCocking() && m_Handgun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) && IsFPSView() && IsAim ? 1.f : 0.f,
		0.9f);
	Util::Easing(&m_Maingun.m_GunADSPer,
		!m_Maingun.GetIsReload() && !m_Maingun.GetIsCocking() && m_Maingun.GetIsEquip() && (m_CharaStyle != CharaStyle::Run) && IsFPSView() && IsAim ? 1.f : 0.f,
		0.9f);
	//
	bool IsReload = KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Reload);
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack)) {
		if (m_Handgun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
			if (gun->GetAmmoNum() == 0 && m_Handgun.CanShot()) {
				IsReload = true;
			}
		}
		if (m_Maingun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
			if (gun->GetAmmoNum() == 0 && m_Maingun.CanShot()) {
				IsReload = true;
			}
		}
	}
	//
	m_ShotSwitch = false;
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack)) {
		if (m_Handgun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
			if (gun->CanShot() && m_Handgun.CanShot()) {
				gun->ShotStart();
				m_ShotSwitch = true;
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
			}
		}
		if (m_Maingun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
			if (gun->CanShot() && m_Maingun.CanShot()) {
				gun->ShotStart();
				m_ShotSwitch = true;
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
			}
		}
	}
	{
		bool IsShot = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack);
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
			gun->SetTrigger(m_Handgun.GetIsReady() && !m_Handgun.GetIsReload() && !m_Handgun.GetIsCocking() && IsShot);
		}
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
			gun->SetTrigger(m_Maingun.GetIsReady() && !m_Maingun.GetIsReload() && !m_Maingun.GetIsCocking() && IsShot);
		}
	}

	if (m_PrevEquip != m_Equip) {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, m_standupID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
		switch (m_Equip) {
		case InvalidID:
			m_Handgun.SetIsEquip(false);
			m_Maingun.SetIsEquip(false);
			break;
		case 0:
			m_Handgun.SetIsEquip(false);
			m_Maingun.SetIsEquip(true);
			break;
		case 1:
			m_Handgun.SetIsEquip(true);
			m_Maingun.SetIsEquip(false);
			break;
		default:
			break;
		}
	}
	m_PrevEquip = m_Equip;
	if (IsReload) {
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
			if (m_Handgun.GetCanReload() && gun->CanReload()) {
				m_Handgun.ReloadStart();
			}
		}
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
			if (m_Maingun.GetCanReload() && gun->CanReload()) {
				m_Maingun.ReloadStart();
			}
		}
	}

	m_Handgun.Update();
	m_Maingun.Update();

	bool NeedAim = false;
	{
		{
			NeedAim |= m_Handgun.GetReadyPer() > 0.5f;
			NeedAim |= m_Handgun.GetLoadHandPer() > 0.5f;
			NeedAim |= m_Handgun.GetGunPullPer() > 0.5f;
		}
		{
			NeedAim |= m_Maingun.GetReadyPer() > 0.5f;
			NeedAim |= m_Maingun.GetLoadHandPer() > 0.5f;
			NeedAim |= m_Maingun.GetGunPullPer() > 0.5f;
		}
	}

	// 進行方向に前進
	Util::Easing(&m_Speed, ((MoveKey != 0) || !m_IsActive) ? GetSpeedMax() : 0.f, 0.9f);

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
		PosBuffer = m_MyPosTarget + Util::Matrix4x4::Vtrans(Vec * -m_Speed, MyMat.rotation());
	}
	else {
		PosBuffer = m_MyPosTarget + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * -m_Speed, MyMat.rotation());
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
	Util::VECTOR3D MyPos = MyMat.pos();
	Util::Easing(&MyPos, m_MyPosTarget, 0.9f);

	SetMatrix(
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), m_Rad.z) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_Rad.y) * Util::Matrix4x4::Mtrans(MyPos)
	);

	//移動割合
	Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);

	if (m_MovePer > 0.01f) {
		m_WalkRad += Util::deg2rad(360) * DeltaTime;
	}
	else {
		m_WalkRad = 0.f;
	}

	for (size_t loop = 0; loop < static_cast<size_t>(CharaStyle::Max); ++loop) {
		m_StylePer.at(loop) = std::clamp(m_StylePer.at(loop) + ((m_CharaStyle == static_cast<CharaStyle>(loop)) ? DeltaTime / 0.3f : -DeltaTime / 0.3f), 0.f, 1.f);
	}

	//停止
	if (NeedAim) {
		m_AnimPer[static_cast<size_t>(CharaAnim::ProneAim)] = (1.f - m_MovePer) * m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
		m_AnimPer[static_cast<size_t>(CharaAnim::Prone)] = 0.f;
	}
	else {
		m_AnimPer[static_cast<size_t>(CharaAnim::ProneAim)] = 0.f;
		m_AnimPer[static_cast<size_t>(CharaAnim::Prone)] = (1.f - m_MovePer) * m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
	}


	m_AnimPer[static_cast<size_t>(CharaAnim::Squat)] = (1.f - m_MovePer) * m_StylePer.at(static_cast<size_t>(CharaStyle::Squat));
	m_AnimPer[static_cast<size_t>(CharaAnim::Stand)] = (1.f - m_MovePer) * std::max(m_StylePer.at(static_cast<size_t>(CharaStyle::Stand)), m_StylePer.at(static_cast<size_t>(CharaStyle::Run)));
	m_AnimPer[static_cast<size_t>(CharaAnim::Stay)] = (1.f - std::max(m_Handgun.GetReadyPer(), m_Maingun.GetReadyPer()));
	//移動
	m_AnimPer[static_cast<size_t>(CharaAnim::ProneWalk)] = m_MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
	m_AnimPer[static_cast<size_t>(CharaAnim::SquatWalk)] = m_MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Squat));
	m_AnimPer[static_cast<size_t>(CharaAnim::Walk)] = m_MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Stand));
	m_AnimPer[static_cast<size_t>(CharaAnim::Run)] = m_MovePer * m_StylePer.at(static_cast<size_t>(CharaStyle::Run));
	//回転
	{
		float Per = 0.f;
		if (!IsFPSView()) {
			if (IsFreeView() && (m_CharaStyle != CharaStyle::Run) && !GetIsReloading()) {
				Per = -Util::VECTOR3D::SignedAngle(MyMat.zvec() * -1.f, m_AimPoint - MyMat.pos(), Util::VECTOR3D::up()) / Util::deg2rad(90);
			}
			else {
				Per = m_YradDif / Util::deg2rad(90);
			}
		}
		Per *= (1.f - m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)));

		Util::Easing(&m_SwitchPer, (Per >= -0.5f) ? 1.f : 0.f, 0.9f);
		Util::Easing(&m_AnimPer[static_cast<size_t>(CharaAnim::FlipLeft)], std::clamp(Per, 0.f, 1.f), 0.9f);
		Util::Easing(&m_AnimPer[static_cast<size_t>(CharaAnim::FlipRight)], std::clamp(Per, -1.f, 0.f) * -1.f, 0.9f);
	}
	//
	m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_1)] = 1.f;
	{
		float Per = 0.3f;
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
			Per = 0.45f;
		}
		if (GetIsReloading()) {
			Per = 0.0f;
		}
		Util::Easing(&m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_2)], Per, 0.8f);
	}
	m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_3)] = 1.f;
	m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_4)] = 1.f;
	m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_5)] = 1.f;

	m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_1)] = 0.f;
	m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_2)] = 0.3f;
	m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_3)] = 0.5f;
	m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_4)] = 0.7f;
	m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_5)] = 0.9f;

	//アニメアップデート
	for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
		SetAnim(loop).SetPer(m_AnimPer[loop]);
	}
	SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
	SetAnim(static_cast<int>(CharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::Run)).Update(true, GetSpeed() * 0.75f);
	SetAnim(static_cast<int>(CharaAnim::Squat)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::SquatWalk)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::ProneWalk)).Update(true, GetSpeed() * 8.f);
	ModelID.FlipAnimAll();

	Util::Matrix4x4 HandBaseMat = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye));

	HandBaseMat =
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), -m_RadAddR2.y) *
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), m_RadAddR2.x) *
		HandBaseMat.rotation() *
		Util::Matrix4x4::Mtrans(HandBaseMat.pos());

	{
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper));
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper2));
		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -m_Rad.x * 0.6f) *
			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper))
		);
		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper2),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -m_Rad.x * 0.4f) *
			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper2))
		);
	}
	{
		Util::HandAnimPool::Instance()->Update();
		if (m_AnimMoving) {
			m_AnimChangePer = std::clamp(m_AnimChangePer + DeltaTime / 0.25f, 0.f, 1.f);
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
			auto& gun = (*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
			auto EyePos =
				Util::Lerp(
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosRight)),
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosLeft)),
					m_SwitchPer
				);
			Util::VRAnim	AimAnim;
			AimAnim.m_RightHandPos = EyePos.pos() * -1.f;
			AimAnim.m_RightRot = Util::Matrix3x3::Get33DX(EyePos);

			Answer = Util::VRAnim::LerpAnim(m_VRAnim, AimAnim, m_Handgun.GetReadyPer());
		}
		{
			auto& gun = (*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
			auto EyePos =
				Util::Lerp(
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosRight)),
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosLeft)),
					m_SwitchPer
				);
			Util::VRAnim	AimAnim;
			AimAnim.m_RightHandPos = EyePos.pos() * -1.f;
			AimAnim.m_RightRot = Util::Matrix3x3::Get33DX(EyePos);

			Answer = Util::VRAnim::LerpAnim(Answer, AimAnim, m_Maingun.GetReadyPer());
		}
		{
			Util::Matrix4x4 RightHandMat = Answer.m_RightRot.Get44DX() * HandBaseMat.rotation() *
				Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(Answer.m_RightHandPos, HandBaseMat.rotation()) + HandBaseMat.pos());

			RightHandMat = Util::Lerp(Util::Lerp(RightHandMat, GetHolsterMat(), m_Handgun.GetPer()), GetHolsterPullMat(), m_Handgun.GetPullPer());
			RightHandMat = Util::Lerp(Util::Lerp(RightHandMat, GetSlingMat(), m_Maingun.GetPer()), GetSlingPullMat(), m_Maingun.GetPullPer());

			if (m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)) > 0.5f && !NeedAim) {
				ModelID.ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm)));
				ModelID.ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2)));
				ModelID.ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist)));
			}
			else {
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
			}
		}

		{
			Util::Matrix4x4 RightMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-90)) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(-90)) *
				GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::RightHandJoint));
			{
				Util::Matrix4x4 Mat = RightMat;
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
				if (m_Handgun.GetInHolster()) {
					Mat = Util::Lerp(GetHolsterMat(), GetHolsterPullMat(), m_Handgun.GetGunPer());
				}
				gun->SetMatrix(Mat);
				gun->SetMagLoadMat(GetMagPouchMat());
			}
			{
				Util::Matrix4x4 Mat = RightMat;
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
				if (m_Maingun.GetInHolster()) {
					Mat = Util::Lerp(GetSlingMat(), GetSlingPullMat(), m_Maingun.GetGunPer());
				}
				gun->SetMatrix(Mat);
				gun->SetMagLoadMat(GetMagPouchMat());
			}
		}

		{
			Util::Matrix4x4 LeftHandMat = m_VRAnim.m_LeftRot.Get44DX() * HandBaseMat.rotation() *
				Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(m_VRAnim.m_LeftHandPos, HandBaseMat.rotation()) + HandBaseMat.pos());
			{
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Handgun.GetUniqueID()));
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetBaseLeftHandMat(), m_Handgun.GetReadyPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetMagLeftHandMat(), m_Handgun.GetLoadHandPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetPullLeftHandMat(), m_Handgun.GetGunPullPer());
			}
			{
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(m_Maingun.GetUniqueID()));
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetBaseLeftHandMat(), m_Maingun.GetReadyPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetMagLeftHandMat(), m_Maingun.GetLoadHandPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetPullLeftHandMat(), m_Maingun.GetGunPullPer());
			}

			if (m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)) > 0.5f && !NeedAim) {
				ModelID.ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm)));
				ModelID.ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2)));
				ModelID.ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist)));
			}
			else {
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
}