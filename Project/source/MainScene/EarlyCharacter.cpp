#include "EarlyCharacter.hpp"

#include "PlayerManager.hpp"

void EarlyCharacter::Update_Chara(void) noexcept {
	auto* BackGroundParts = BackGround::Instance();

	auto& Chara = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));
	{
		auto MyPos = GetEyeMatrix().pos();
		auto MyVec = GetEyeMatrix().zvec() * -1.f; MyVec.y = 0.f; MyVec = MyVec.normalized();
		auto TargetPos = Chara->GetEyeMatrix().pos();
		auto TargetVec = TargetPos - MyPos; 
		auto Height = std::fabsf(TargetVec.y); TargetVec.y = 0.f;
		auto Len = TargetVec.magnitude();
		auto rad = std::atan2f(Util::VECTOR3D::Cross(MyVec, TargetVec.normalized()).y, Util::VECTOR3D::Dot(MyVec, TargetVec.normalized()));
		{
			bool IsInsight = !IsDown();
			{
				if (IsInsight) {
					IsInsight = Height < 1.5f * Scale3DRate;
				}
				if (IsInsight) {
					IsInsight = std::fabsf(rad) < Util::deg2rad(45) || (Len < 0.75f * Scale3DRate);
				}
				if (IsInsight) {
					IsInsight = Len < 4.f * Scale3DRate;
				}
				if (IsInsight) {
					IsInsight = !BackGroundParts->CheckLine(MyPos, &TargetPos);
				}
			}
			if (IsInsight) {
				m_WatchTimer = 6.f;
			}
			else {
				m_WatchTimer = std::max(m_WatchTimer - DeltaTime, 0.f);
			}
		}
		{
			bool IsInsight = !IsDown();
			{
				if (IsInsight) {
					IsInsight = Height < 1.5f * Scale3DRate;
				}
				if (IsInsight) {
					IsInsight = std::fabsf(rad) < Util::deg2rad(45) || (Len < 0.75f * Scale3DRate);
				}
				if (IsInsight) {
					IsInsight = Len < 7.f * Scale3DRate;
				}
				if (IsInsight) {
					IsInsight = !BackGroundParts->CheckLine(MyPos, &TargetPos);
				}
			}
			if (IsInsight) {
				m_FindTimer = std::min(m_FindTimer + DeltaTime, 3.f);
			}
			else {
				m_FindTimer = std::max(m_FindTimer - DeltaTime, 0.f);
			}
			if (m_FindTimer == 3.f) {
				m_WatchTimer = 6.f;
			}
		}
		if (IsWatching()) {
			//視認したらその方向を追う
			SetTarget(TargetPos);
			m_IsMove = true;
			m_IsTurn = true;
		}
		else {
			//見失った
			SetTarget(MyPos + MyVec);
			m_IsMove = false;
			m_IsTurn = false;
		}
	}

	{
		bool				CanPunch{ false };
		bool				CanArmlock{ false };
		if (!this->m_Armlocked &&
			!(this->m_DownTop || this->m_WakeTop || this->m_DownBottom || this->m_WakeBottom)
			) {
			Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), m_Rot);

			Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), m_Rot);
			Util::VECTOR3D Base2 = Base;
			Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), m_Rot);

			Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), m_Rot);
			Util::VECTOR3D Target2 = Target;
			Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), m_Rot);
			if (Chara->CheckHit(Base1, &Target1) || Chara->CheckHit(Base2, &Target2) || Chara->CheckHit(Base3, &Target3)) {
				Util::VECTOR3D A = Chara->GetMat().zvec(); A.y = 0.f;
				Util::VECTOR3D B = Target - Base; B.y = 0.f;
				if (Util::VECTOR3D::Dot(A, B) > 0.f) {
					CanPunch = true;
				}
				else {
					CanArmlock = true;
				}
			}
		}


		if (CanPunch && Chara->CanDamage() && (this->m_AttackTime == 0.f)) {
			//パンチ
			if (!this->m_PunchActive) {
				this->m_PunchActive = true;
				SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).SetTime(0.f);
				m_AttackTime = 3.f;
			}
		}
		if (CanArmlock && Chara->CanDamage() && (this->m_AttackTime == 0.f)) {
			if (!this->m_ArmlockActive) {
				this->m_ArmlockActive = true;
				this->m_ArmlockTime = 2.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockStart)).SetTime(0.f);
				//IDの相手に羽交い絞めを開始させる
				Chara->SetArmlocked(this->GetObjectID());

				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockStartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockID)->Play3D(GetMat().pos(), 10.f * Scale3DRate, DX_PLAYTYPE_LOOP);
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				m_AttackTime = 3.f;
			}
		}
		if (this->m_ArmlockActive && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockStart)).GetTimePer() >= 1.f) {
			if (this->m_ArmlockTime == 0.f) {
				if (!this->m_ArmlockEnd) {
					this->m_ArmlockEnd = true;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockID)->StopAll();
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockStartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
					SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockEnd)).SetTime(0.f);
					Chara->SetArmlockedEnd();
				}
			}
		}

		this->m_ArmlockTime = std::max(this->m_ArmlockTime - DeltaTime, 0.f);
		this->m_AttackTime = std::max(this->m_AttackTime - DeltaTime, 0.f);

		//前準備
		this->m_PathUpdateTimer = std::max(this->m_PathUpdateTimer - DeltaTime, 0.f);
		if (this->m_PathUpdateTimer <= 0.f) {
			this->m_PathUpdateTimer += 1.f;
			//ChangePoint
			Util::VECTOR3D MyPos = GetTargetPos();
			this->m_TargetPathPlanningIndex = -1;
			for (int i = 0; i < 10; i++) {
				this->m_PathChecker.Dispose();
				if (this->m_PathChecker.Init(MyPos, this->m_MyTarget)) {	// 指定の２点の経路情報を探索する
					this->m_TargetPathPlanningIndex = this->m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
					break;
				}
				else {
					MyPos = BackGroundParts->GetWayPoint()->GetRandomPoint(MyPos, 10.f * Scale3DRate);//選定できない場合10m以内で再選定
				}
			}
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
			{
				Util::VECTOR3D Vec3D = this->m_MyTarget - GetTargetPos(); Vec3D.y = 0.f;
				IsMove = Vec3D.magnitude() >= 0.5f * Scale3DRate;
			}

			Util::VECTOR2D InputVec = Util::VECTOR2D::zero();
			{
				auto Pos = (GetTargetPos() + Util::VECTOR3D::up() * (1.f * Scale3DRate));
				Util::VECTOR3D Vec3D = this->m_PathChecker.GetNextPoint(Pos, &this->m_TargetPathPlanningIndex) - Pos;
				InputVec.x = Vec3D.x;
				InputVec.y = Vec3D.z;
				if (InputVec.magnitude() >= 1.f * Scale3DRate) {
					InputVec = InputVec.normalized();
				}
				if (this->m_PunchActive || this->m_ArmlockActive) {
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

			if (this->m_DownTop || this->m_WakeTop || this->m_DownBottom || this->m_WakeBottom) {
				m_InputVec.x = m_DownVec.x;
				m_InputVec.y = m_DownVec.z;
			}
			if (std::fabsf(this->m_HitPower) > 0.5f) {
				m_InputVec.x = -m_HitVec.x;
				m_InputVec.y = -m_HitVec.z;
			}

			if (this->m_InputVec.sqrMagnitude() > 0.f) {
				float Per = CalcYradDiff(std::atan2f(-m_InputVec.x, -m_InputVec.y));

				IsMove = std::fabsf(Per) < 0.3f;

				m_RadAdd.y = 0.f;
				if (std::fabsf(Per) > 0.01f) {
					float Power = 1.f;
					m_RadAdd.y = Per * Power * Util::deg2rad(720.f) * DeltaTime;
				}

				if (this->m_DownTop || this->m_WakeTop || this->m_DownBottom || this->m_WakeBottom) {
					IsMove = false;
				}
				else if (std::fabsf(this->m_HitPower) > 0.5f) {
					IsMove = false;
				}
				else if (this->m_Armlocked || this->m_PunchActive || this->m_ArmlockActive) {
					IsMove = false;
					m_RadAdd.y = 0.f;
				}
				if (!m_IsMove) {
					//停止
					IsMove = false;
				}
				if (!m_IsTurn) {
					m_RadAdd.y = 0.f;
				}


				this->m_Rad.y += m_RadAdd.y;
				{
					float Power = 1.f;
					Util::Easing(&m_Rad.z, m_RadAdd.y * Power, 0.9f);
				}
				this->m_Rad.y = Util::AngleRange360(this->m_Rad.y);
			}

			Util::Easing(&m_Rad.x, 0.f, 0.9f);
		}

		// 進行方向に前進
		Util::Easing(&m_Speed, IsMove ? GetSpeedMax() : 0.f, 0.9f);

		if (this->m_Armlocked && !this->m_ArmlockedEnd) {
			auto& Target = (*ObjectManager::Instance()->GetObj(this->m_ArmlockedPos));
			this->m_MyPosTarget = Target->GetMat().pos();
			MyMat = Target->GetMat();
			m_Rot = Util::Matrix3x3::Get33DX(MyMat);
			this->m_Rad.y = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), m_Rot.zvec(), Util::VECTOR3D::up());
		}
		// 移動ベクトルを加算した仮座標を作成
		Util::VECTOR3D PosBefore = GetTargetPos();
		Util::VECTOR3D PosAfter;
		{
			Util::VECTOR3D Vec = Util::VECTOR3D::forward();
			if (this->m_Armlocked || this->m_PunchActive || this->m_DownTop || this->m_WakeTop || this->m_DownBottom || this->m_WakeBottom) {
				Vec = Util::VECTOR3D::zero();
			}

			PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Vec * -GetSpeed(), m_Rot);

			PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(5.f * Scale3DRate * DeltaTime) * this->m_DownPower, m_Rot);

			PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * (5.f * Scale3DRate * DeltaTime) * this->m_HitPower,
				Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), m_HitVec));

			Util::Easing(&this->m_HitBack, 0.f, 0.95f);
			Util::Easing(&this->m_HitPower, 0.f, 0.9f);
			Util::Easing(&this->m_DownPower, 0.f, 0.9f);

			this->m_KeepTimer = std::clamp(this->m_KeepTimer - DeltaTime, 0.f, 1.f);
			if (this->m_KeepTimer == 0.f) {
				this->m_KeepPower = std::clamp(this->m_KeepPower - DeltaTime / 2.f, 0.f, 1.f);
			}
		}
		//他キャラとのヒット判定
		if (!this->m_Armlocked && !this->m_ArmlockActive &&
			!(this->m_DownTop || this->m_WakeTop || this->m_DownBottom || this->m_WakeBottom)
			) {
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
		if (this->m_DownTop || this->m_WakeTop || this->m_DownBottom || this->m_WakeBottom) {
			Util::VECTOR3D PosAdd = GetEyeMatrix().pos() - GetMat().pos(); PosAdd.y = 0.f; PosAdd = PosAdd.normalized() * (0.5f * Scale3DRate);
			CheckWall(PosBefore, &PosAfter, PosAdd, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate);
		}
		// 地面判定
		if (CheckGround(&PosAfter, Util::VECTOR3D::zero(), 1.f, 0.1f)) {
			this->m_Vector.y = 0.f;
		}
		else {
			// ヒットしていない際は落下させる
			this->m_Vector.y -= GravAccel;
			PosAfter.y += this->m_Vector.y;
		}
		// 仮座標を反映
		this->m_Speed = std::clamp((PosAfter - PosBefore).magnitude(), 0.f, this->m_Speed);

		this->m_MyPosTarget = PosAfter;
		Util::VECTOR3D MyPos = GetMat().pos();
		Util::Easing(&MyPos, PosAfter, 0.9f);

		m_Rot = Util::Matrix3x3::Get33DX(GetRotMat());
		SetMatrix(
			GetRotMat() * Util::Matrix4x4::Mtrans(MyPos)
		);

		//移動割合
		Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);

		{
			auto Prev = this->m_DownTopTimer;
			if (m_DrugPer < GetDrugPerMax()) {
				this->m_DownTopTimer = std::max(this->m_DownTopTimer - DeltaTime, 0.f);
			}
			if (Prev != 0.f && this->m_DownTopTimer == 0.f) {
				this->m_DownTop = false;
				//起き上がる
				if (!this->m_WakeTop) {
					SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).SetTime(0.f);
				}
				this->m_WakeTop = true;
			}
		}
		{
			auto Prev = this->m_DownBottomTimer;
			if (m_DrugPer < GetDrugPerMax()) {
				this->m_DownBottomTimer = std::max(this->m_DownBottomTimer - DeltaTime, 0.f);
			}
			if (Prev != 0.f && this->m_DownBottomTimer == 0.f) {
				this->m_DownBottom = false;
				//起き上がる
				if (!this->m_WakeBottom) {
					SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).SetTime(0.f);
				}
				this->m_WakeBottom = true;
			}
		}

		if (this->m_PunchAttack) {
			this->m_PunchAttack = false;
			Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), m_Rot);
			for (auto& c : PlayerManager::Instance()->SetCharacter()) {
				if (!c->IsPlayer()) { continue; }
				Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), m_Rot);
				Util::VECTOR3D Base2 = Base;
				Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), m_Rot);

				Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), m_Rot);
				Util::VECTOR3D Target2 = Target;
				Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), m_Rot);
				if (c->CheckHit(Base1, &Target1) || c->CheckHit(Base2, &Target2) || c->CheckHit(Base3, &Target3)) {
					((std::shared_ptr<Character>&)c)->SetHit(Target - Base);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Target, 10.f * Scale3DRate);
				}
			}
		}

		if (this->m_PunchActive && SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).GetTimePer() >= 1.f) {
			this->m_PunchActive = false;
		}
		if (this->m_ArmlockedInjector && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).GetTimePer() >= 1.f) {
			this->m_ArmlockedInjector = false;
		}
		if (this->m_ArmlockedEnd && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).GetTimePer() >= 1.f) {
			this->m_ArmlockedEnd = false;
			this->m_Armlocked = false;

			this->m_DownBottom = true;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).SetTime(SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).GetTotalTime());

			this->m_WakeBottom = false;
		}
		if (this->m_ArmlockEnd && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockEnd)).GetTimePer() >= 1.f) {
			this->m_ArmlockEnd = false;
			this->m_ArmlockActive = false;
			m_AttackTime = 3.f;
		}
		//
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Punch)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockStart)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockEnd)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedStart)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedInjector)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedEnd)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeTop)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownTop)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeBottom)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownBottom)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Upper)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Stand)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Walk)] = 0.f;

		if (this->m_ArmlockActive) {
			if (!this->m_ArmlockEnd) {
				this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockStart)] = 1.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockStart)).Update(false, 1.f);
			}
			else {
				this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockEnd)] = 1.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockEnd)).Update(false, 1.f);
			}
		}
		else if (this->m_Armlocked) {
			if (!this->m_ArmlockedEnd) {
				if (!this->m_ArmlockedInjector) {
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedStart)] = 1.f;
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedInjector)] = 0.f;
					SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedStart)).Update(false, 1.f);
				}
				else {
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedStart)] = 0.f;
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedInjector)] = 1.f;
					SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).Update(false, 1.f);

					float Now = SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).GetTime();
					if (static_cast<int>(Now) == 30 && static_cast<int>(Now) != static_cast<int>(m_ArmlockedInjectorTimer)) {
						SetDrug(GetDrugPerMax() * 100.f / 100.f);
					}
					m_ArmlockedInjectorTimer = Now;
				}
			}
			else {
				this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedEnd)] = 1.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).Update(false, 1.f);


				float Now = SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).GetTime();
				if (static_cast<int>(Now) == 10 && static_cast<int>(Now) != static_cast<int>(m_ArmlockedEndTimer)) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, DownHumanID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				}
				m_ArmlockedEndTimer = Now;
			}
		}
		else if (this->m_DownTop) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownTop)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownTop)).Update(false, 1.f);
		}
		else if (this->m_WakeTop) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeTop)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).Update(false, 1.f);
			if (SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).GetTimePer() >= 1.f) {
				this->m_WakeTop = false;
			}
		}
		else if (this->m_DownBottom) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownBottom)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).Update(false, 1.f);
		}
		else if (this->m_WakeBottom) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeBottom)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).Update(false, 1.f);
			if (SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).GetTimePer() >= 1.f) {
				this->m_WakeBottom = false;
			}
		}
		else if (this->m_PunchActive) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Punch)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).Update(false, 1.f);

			float Now = SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).GetTime();
			if (static_cast<int>(Now) == 1 && static_cast<int>(Now) != static_cast<int>(m_PunchTimer)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PunchID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			}
			if (static_cast<int>(Now) == 10 && static_cast<int>(Now) != static_cast<int>(m_PunchTimer)) {
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				m_PunchAttack = true;
			}
			m_PunchTimer = Now;
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

		Util::Easing(&m_DrugPerR, m_DrugPer, 0.9f);

		this->m_CanSeeUI = false;
	}
}