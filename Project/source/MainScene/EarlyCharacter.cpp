#include "EarlyCharacter.hpp"

#include "PlayerManager.hpp"

void EarlyCharacter::Update_Chara(void) noexcept {
	auto* BackGroundParts = BackGround::Instance();

	auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));
	{
		auto MyPos = GetEyeMatrix().pos();
		auto MyVec = GetEyeMatrix().zvec() * -1.f; MyVec.y = 0.f; MyVec = MyVec.normalized();
		auto TargetPos = Player->GetEyeMatrix().pos();
		auto TargetVec = TargetPos - MyPos; 
		auto Height = std::fabsf(TargetVec.y); TargetVec.y = 0.f;
		auto Len = TargetVec.magnitude();
		auto rad = std::atan2f(Util::VECTOR3D::Cross(MyVec, TargetVec.normalized()).y, Util::VECTOR3D::Dot(MyVec, TargetVec.normalized()));
		{
			bool IsAlert = !IsDown();
			{
				if (IsAlert) {
					IsAlert = Height < 1.5f * Scale3DRate;
				}
				if (IsAlert) {
					IsAlert = std::fabsf(rad) < Util::deg2rad(45) || (Len < 0.75f * Scale3DRate);
				}
				if (IsAlert) {
					IsAlert = Len < 4.f * Scale3DRate;
				}
				if (IsAlert) {
					IsAlert = !BackGroundParts->CheckLine(MyPos, &TargetPos);
				}
			}

			//誰かが見てた状態がになってないか5秒間隔で確認
			bool WatchAny = false;
			for (auto& c : PlayerManager::Instance()->GetCharacter()) {
				if (c->IsPlayer()) { continue; }
				if (c->GetObjectID() == GetObjectID()) { continue; }
				auto& ec = ((std::shared_ptr<EarlyCharacter>&)c);
				if (ec->IsDown()) { continue; }
				if (ec->m_AlertTimer <= 7.f) { continue; }
				WatchAny = true;
				break;
			}
			if (WatchAny) {
				this->m_AlertAnyTimer += DeltaTime;
			}
			else {
				this->m_AlertAnyTimer = 0.f;
			}
			if (this->m_AlertAnyTimer >= 5.f) {
				this->m_AlertAnyTimer -= 5.f;
				IsAlert = true;
			}


			if (IsAlert) {
				if (!IsWatching()) {
					if (!this->m_Alert.IsActive()) {
						this->m_Alert.SetActive();
					}
				}
				this->m_AlertTimer = 10.f;
			}
			else {
				this->m_AlertTimer = std::max(this->m_AlertTimer - DeltaTime, 0.f);
			}
		}
		if (IsWatching()) {
			//視認したらその方向を追う
			this->m_MyTarget = TargetPos;
		}
		else {
			//見失った
			this->m_MyTarget = MyPos + MyVec * 2.f;
		}
	}

	{
		this->m_Alert.Update();
		if (!(this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End)) {
			Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Base2 = Base;
			Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Target2 = Target;
			Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);
			if (Player->CheckHit(Base1, &Target1) || Player->CheckHit(Base2, &Target2) || Player->CheckHit(Base3, &Target3)) {
				if (this->m_AttackCoolDown == 0.f) {
					//パンチ
					if (!this->m_Punch.IsActive()) {
						this->m_Punch.SetActive();
						SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).SetTime(0.f);
						this->m_AttackCoolDown = 3.f;
					}
				}
			}
		}

		this->m_AttackCoolDown = std::max(this->m_AttackCoolDown - DeltaTime, 0.f);

		//前準備
		this->m_PathUpdateTimer = std::max(this->m_PathUpdateTimer - DeltaTime, 0.f);
		if (this->m_PathUpdateTimer <= 0.f) {
			this->m_PathUpdateTimer += 1.f;
			//ChangePoint
			Util::VECTOR3D MyPos = GetTargetPos();
			this->m_TargetPathPlanningIndex = -1;
		}
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
						PlayFootSound(192);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 5) {
						this->m_FootSoundID = 5;
						PlayFootSound(192);
					}
				}
			}
			if (!IsMoving) {
				this->m_FootSoundID = -1;
			}
		}
		// 左右回転
		{
			Util::VECTOR2D InputVec = Util::VECTOR2D::zero();
			{
				auto Pos = (GetTargetPos() + Util::VECTOR3D::up() * (1.f * Scale3DRate));
				Util::VECTOR3D Vec3D = this->m_MyTarget - Pos;
				InputVec.x = Vec3D.x;
				InputVec.y = Vec3D.z;

				if (InputVec.magnitude() >= 1.f * Scale3DRate) {
					InputVec = InputVec.normalized();
				}
				if (this->m_Punch.IsActive()) {
					InputVec = Util::VECTOR2D::zero();
				}
			}
			//歩くより早く移動する場合
			if (GetSpeed() > (3.f * Scale3DRate * DeltaTime)) {
				Util::Easing(&m_InputVec, InputVec, 0.975f);
			}
			else {
				Util::Easing(&m_InputVec, InputVec, 0.9f);
			}

			if (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
				this->m_InputVec.x = this->m_DownVec.x;
				this->m_InputVec.y = this->m_DownVec.z;
			}
			if (std::fabsf(this->m_HitPower) > 0.5f) {
				this->m_InputVec.x = -m_HitVec.x;
				this->m_InputVec.y = -m_HitVec.z;
			}

			if (this->m_InputVec.sqrMagnitude() > 0.f) {
				float Per = CalcYradDiff(std::atan2f(-m_InputVec.x, -m_InputVec.y));

				this->m_RadAdd.y = 0.f;
				if (std::fabsf(Per) > 0.01f) {
					float Power = 1.f;
					this->m_RadAdd.y = Per * Power * Util::deg2rad(720.f) * DeltaTime;
				}

				if (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End || (std::fabsf(this->m_HitPower) > 0.5f)) {
				}
				else if (this->m_Punch.IsActive()) {
					this->m_RadAdd.y = 0.f;
				}

				this->m_Rad.y += this->m_RadAdd.y;
				{
					float Power = 1.f;
					Util::Easing(&m_Rad.z, this->m_RadAdd.y * Power, 0.9f);
				}
				this->m_Rad.y = Util::AngleRange360(this->m_Rad.y);
			}

			Util::Easing(&m_Rad.x, 0.f, 0.9f);
		}

		// 進行方向に前進
		Util::Easing(&m_Speed, 0.f, 0.9f);

		// 移動ベクトルを加算した仮座標を作成
		Util::VECTOR3D PosBefore = GetTargetPos();
		Util::VECTOR3D PosAfter;
		{
			Util::VECTOR3D Vec = Util::VECTOR3D::forward();
			if (this->m_Punch.IsActive() || this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
				Vec = Util::VECTOR3D::zero();
			}

			PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Vec * -GetSpeed(), this->m_Rot);

			PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(5.f * Scale3DRate * DeltaTime) * this->m_DownPower, this->m_Rot);

			PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * (5.f * Scale3DRate * DeltaTime) * this->m_HitPower,
				Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), this->m_HitVec));

			Util::Easing(&this->m_HitBack, 0.f, 0.95f);
			Util::Easing(&this->m_HitPower, 0.f, 0.9f);
			Util::Easing(&this->m_DownPower, 0.f, 0.9f);

			this->m_KeepTimer = std::clamp(this->m_KeepTimer - DeltaTime, 0.f, 1.f);
			if (this->m_KeepTimer == 0.f) {
				this->m_KeepPower = std::clamp(this->m_KeepPower - DeltaTime / 2.f, 0.f, 1.f);
			}
		}
		//他キャラとのヒット判定
		if (!(this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End)) {
			float Radius = 2.0f * 0.3f * Scale3DRate;
			for (auto& c : PlayerManager::Instance()->SetCharacter()) {
				if (c->GetObjectID() == this->GetObjectID()) { continue; }
				//
				auto Vec = c->GetMat().pos() - GetMat().pos();
				float Height = std::fabsf(Vec.y);
				Vec.y = 0.f;
				float Len = Vec.magnitude();
				if (Len < Radius && Height < Radius) {
					PosAfter = PosAfter + Vec.normalized() * (Len - Radius);
				}
			}
		}
		// 壁判定
		CheckWall(PosBefore, &PosAfter, Util::VECTOR3D::zero(), Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate);
		if (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
			Util::VECTOR3D PosAdd = GetEyeMatrix().pos() - GetMat().pos(); PosAdd.y = 0.f; PosAdd = PosAdd.normalized() * (0.5f * Scale3DRate);
			CheckWall(PosBefore, &PosAfter, PosAdd, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate);
		}
		// 地面判定
		/*
		if (CheckGround(&PosAfter, Util::VECTOR3D::zero(), 1.f, 0.1f)) {
			this->m_Vector.y = 0.f;
		}
		else {
			// ヒットしていない際は落下させる
			this->m_Vector.y -= GravAccel;
			PosAfter.y += this->m_Vector.y;
		}
		//*/
		this->m_Vector.y = 0.f;
		// 仮座標を反映
		this->m_Speed = std::clamp((PosAfter - PosBefore).magnitude(), 0.f, this->m_Speed);

		this->m_MyPosTarget = PosAfter;
		Util::VECTOR3D MyPos = GetMat().pos();
		Util::Easing(&MyPos, PosAfter, 0.9f);

		this->m_Rot = Util::Matrix3x3::Get33DX(GetRotMat());
		SetMatrix(
			GetRotMat() * Util::Matrix4x4::Mtrans(MyPos)
		);

		//移動割合
		Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);

		{
			auto Prev = this->m_DownTop.m_Time;
			this->m_DownTop.m_Time = std::max(this->m_DownTop.m_Time - DeltaTime, 0.f);
			if (Prev != 0.f && this->m_DownTop.m_Time == 0.f) {
				this->m_DownTop.m_Active = false;
				//起き上がる
				if (!this->m_DownTop.m_End) {
					SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).SetTime(0.f);
				}
				this->m_DownTop.m_End = true;
			}
		}
		{
			auto Prev = this->m_DownBottom.m_Time;
			this->m_DownBottom.m_Time = std::max(this->m_DownBottom.m_Time - DeltaTime, 0.f);
			if (Prev != 0.f && this->m_DownBottom.m_Time == 0.f) {
				this->m_DownBottom.m_Active = false;
				//起き上がる
				if (!this->m_DownBottom.m_End) {
					SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).SetTime(0.f);
				}
				this->m_DownBottom.m_End = true;
			}
		}

		if (this->m_PunchAttack) {
			this->m_PunchAttack = false;
			Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);
			for (auto& c : PlayerManager::Instance()->SetCharacter()) {
				if (!c->IsPlayer()) { continue; }
				Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
				Util::VECTOR3D Base2 = Base;
				Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);

				Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
				Util::VECTOR3D Target2 = Target;
				Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);
				if (c->CheckHit(Base1, &Target1) || c->CheckHit(Base2, &Target2) || c->CheckHit(Base3, &Target3)) {
					((std::shared_ptr<Character>&)c)->SetHit(Target - Base);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_HitHumanID)->Play3D(Target, 10.f * Scale3DRate);
				}
			}
		}

		if (this->m_Punch.IsActive() && SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).GetTimePer() >= 1.f) {
			this->m_Punch.m_Active = false;
		}
		//
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Punch)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeTop)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownTop)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeBottom)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownBottom)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Upper)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Stand)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Walk)] = 0.f;

		if (this->m_DownTop.IsActive()) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownTop)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownTop)).Update(false, 1.f);
		}
		else if (this->m_DownTop.m_End) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeTop)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).Update(false, 1.f);
			if (SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).GetTimePer() >= 1.f) {
				this->m_DownTop.m_End = false;
			}
		}
		else if (this->m_DownBottom.IsActive()) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownBottom)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).Update(false, 1.f);
		}
		else if (this->m_DownBottom.m_End) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeBottom)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).Update(false, 1.f);
			if (SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).GetTimePer() >= 1.f) {
				this->m_DownBottom.m_End = false;
			}
		}
		else if (this->m_Punch.IsActive()) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Punch)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).Update(false, 1.f);

			float Now = SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).GetTime();
			if (static_cast<int>(Now) == 1 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_Time)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PunchID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			}
			if (static_cast<int>(Now) == 10 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_Time)) {
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				this->m_PunchAttack = true;
			}
			this->m_Punch.m_Time = Now;
		}
		else {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Upper)] = 1.f;
			//停止
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Stand)] = (1.f - GetMovePer01());
			//移動
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Walk)] = GetMovePer01();
		}

		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(EarlyCharaAnim::Max); ++loop) {
			SetAnim(loop).SetPer(this->m_AnimPer[loop]);
		}
		SetAnim(static_cast<int>(EarlyCharaAnim::Upper)).Update(true, 1.f);
		SetAnim(static_cast<int>(EarlyCharaAnim::Stand)).Update(true, 1.f);
		SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
		SetModel().FlipAnimAll();



		{
			ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper));
			SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper),
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), this->m_HitBack * Util::deg2rad(90.f)) *
				GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper))
			);
		}

		Util::Easing(&m_CanSeePer, this->m_CanSeeUI ? 1.f : 0.f, 0.9f);

		this->m_CanSeeUI = false;
	}
}