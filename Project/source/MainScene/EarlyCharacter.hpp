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

enum class EarlyCharaAnim {
	Stand,//立ち
	Walk,//歩き

	Upper,//左に向く
	Max,
};

enum class EarlyCharaFrame {
	Center,
	Upper,
	Upper2,
	Head,
	Eye,
	Max,
};
static const char* EarlyCharaFrameName[static_cast<int>(EarlyCharaFrame::Max)] = {
	"センター",
	"上半身",
	"上半身2",
	"頭",
	"両目",
};

namespace AIs {
	class PathChecker {
	public:
		// 経路探索処理用の１ポリゴンの情報
		class PATHPLANNING_UNIT {
			int PolyIndex;						// ポリゴン番号
			float TotalDistance;				// 経路探索でこのポリゴンに到達するまでに通過したポリゴン間の距離の合計
			PATHPLANNING_UNIT* PrevPolyUnit;		// 経路探索で確定した経路上の一つ前のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
			PATHPLANNING_UNIT* NextPolyUnit;		// 経路探索で確定した経路上の一つ先のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
		public:
			PATHPLANNING_UNIT* ActiveNextUnit;		// 経路探索処理対象になっている次のポリゴンのメモリアドレスを格納する変数
		public:
			const auto& GetPolyIndex() const noexcept { return this->PolyIndex; }
			const auto& GetNextPolyUnit() const noexcept { return this->NextPolyUnit; }
		public:
			bool SetPrevPolyUnit(PATHPLANNING_UNIT* PUnit, int tris) {
				auto* BackGroundParts = BackGround::Instance();
				// 隣接するポリゴンが既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
				auto& Unit = BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(PUnit->PolyIndex));

				auto trisdistance = PUnit->TotalDistance +
					(BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(Unit.GetLinkPolyIndex(tris))).GetPos() + Unit.GetPos()).magnitude();

				if (this->TotalDistance > trisdistance) {
					this->TotalDistance = trisdistance;		// 隣接するポリゴンにここに到達するまでの距離を代入する
				}
				else {
					if (this->PrevPolyUnit) { return false; }
				}
				this->PrevPolyUnit = PUnit;			// 隣接するポリゴンに経路情報となる自分のポリゴンの番号を代入する
				return true;
			}
			bool SearchThisUnit(PATHPLANNING_UNIT* SearchUnit) {
				// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
				PATHPLANNING_UNIT* PUnitSub2 = SearchUnit;
				while (true) {
					if (PUnitSub2 == nullptr) { break; }
					if (PUnitSub2 == this) { return false; }//既に追加されとる
					PUnitSub2 = PUnitSub2->ActiveNextUnit;
				}
				return PUnitSub2 != nullptr;
			}
		public:
			// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
			static void SetNextIndex(PATHPLANNING_UNIT* pGoal, PATHPLANNING_UNIT* pStart) {
				PATHPLANNING_UNIT* PUnit = pGoal;
				while (true) {
					auto* PrevPUnitIndex = PUnit;
					PUnit = PUnit->PrevPolyUnit;
					PUnit->NextPolyUnit = PrevPUnitIndex;
					if (PUnit == pStart) { break; }
				}
			}
		public:
			void Init(int index) {
				this->PolyIndex = index;
				this->TotalDistance = 0.0f;
				this->PrevPolyUnit = nullptr;
				this->NextPolyUnit = nullptr;
				this->ActiveNextUnit = nullptr;
			}
		};
	private:
		Util::VECTOR3D GoalPosition;					// 目標位置
		char		padding[4]{};
		std::vector<PATHPLANNING_UNIT>UnitArray;	// 経路探索処理で使用する全ポリゴンの情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
		PATHPLANNING_UNIT* StartUnit{ nullptr };			// 経路のスタート地点にあるポリゴン情報へのメモリアドレスを格納する変数
		PATHPLANNING_UNIT* GoalUnit{ nullptr };				// 経路のゴール地点にあるポリゴン情報へのメモリアドレスを格納する変数
	public:
		const auto& GetStartUnit() const noexcept { return this->StartUnit; }
	public:
		Util::VECTOR3D GetNextPoint(const Util::VECTOR3D& NowPosition, int* TargetPathPlanningIndex) const {
			{
				auto* BackGroundParts = BackGround::Instance();
				const float COLLWIDTH = 0.001f * Scale3DRate;												// 当たり判定のサイズ
				auto Goal = this->GoalPosition;
				if (!BackGroundParts->CheckLine(NowPosition, &Goal)) {
					// 方向は目標座標
					return this->GoalPosition;
				}
				int NowIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds(NowPosition);
				if (!((*TargetPathPlanningIndex != -1) && (this->GoalUnit))) {
					return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(BackGroundParts->GetWayPoint()->GetNearestBuilds2(NowPosition))).GetPos();
				}
				if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
					if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
						auto NextIndex = this->UnitArray.at(static_cast<size_t>(*TargetPathPlanningIndex)).GetNextPolyUnit()->GetPolyIndex();
						(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
					}
					// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
					return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(*TargetPathPlanningIndex)).GetPos();
				}
				else {
					// 方向は目標座標
					return this->GoalPosition;
				}
			}



			auto* BackGroundParts = BackGround::Instance();
			int NowIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds(NowPosition);
			if (!((*TargetPathPlanningIndex != -1) && (this->GoalUnit))) {
				return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(BackGroundParts->GetWayPoint()->GetNearestBuilds2(NowPosition))).GetPos();
			}
			if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
				if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
					const float COLLWIDTH = 0.001f * Scale3DRate;												// 当たり判定のサイズ
					while (true) {																				// 次の中間地点が決定するまでループし続ける
						if (!this->UnitArray.at(static_cast<size_t>(*TargetPathPlanningIndex)).GetNextPolyUnit()) { break; }
						auto NextIndex = this->UnitArray.at(static_cast<size_t>(*TargetPathPlanningIndex)).GetNextPolyUnit()->GetPolyIndex();
						if (!BackGroundParts->GetWayPoint()->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// 経路上の次のポリゴンの中心座標に直線的に移動できない場合はループから抜ける
						(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
						if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// もしゴール地点のポリゴンだったらループを抜ける
					}
				}
				// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
				return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(*TargetPathPlanningIndex)).GetPos();
			}
			else {
				// 方向は目標座標
				return this->GoalPosition;
			}
		}
	public:
		bool Init(Util::VECTOR3D StartPos, Util::VECTOR3D GoalPos) {
			auto* BackGroundParts = BackGround::Instance();
			// 指定の２点の経路を探索する( 戻り値 true:経路構築成功 false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
			this->GoalPosition = GoalPos;			// ゴール位置を保存

			this->UnitArray.resize(BackGroundParts->GetWayPoint()->GetWayPoints().size());			// 経路探索用のポリゴン情報を格納するメモリ領域を確保、初期化
			for (auto& p : this->UnitArray) {
				p.Init(static_cast<int>(&p - &this->UnitArray.front()));
			}

			int StartIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds2(StartPos);	// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
			if (StartIndex == -1) { return false; }
			this->StartUnit = &this->UnitArray[static_cast<size_t>(StartIndex)];					// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存

			int GoalIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds2(GoalPos);		// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
			if (GoalIndex == -1) { return false; }
			this->GoalUnit = &this->UnitArray[static_cast<size_t>(GoalIndex)];				// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
			if (GoalIndex == StartIndex) { return false; }				// ゴール地点にあるポリゴンとスタート地点にあるポリゴンが同じだったら false を返す

			PATHPLANNING_UNIT* ActiveFirstUnit = this->StartUnit;		// 経路探索処理対象のポリゴンとしてスタート地点にあるポリゴンを登録する
			// 経路を探索してゴール地点のポリゴンにたどり着くまでループを繰り返す
			while (true) {
				bool Goal = false;
				// 経路探索処理対象になっているポリゴンをすべて処理
				PATHPLANNING_UNIT* PUnit = ActiveFirstUnit;
				ActiveFirstUnit = nullptr;
				while (true) {
					// ポリゴンの辺の数だけ繰り返し
					for (int K = 0; K < 8; K++) {
						int Index = BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(PUnit->GetPolyIndex())).GetLinkPolyIndex(K);
						if (Index == -1) { continue; }											// 辺に隣接するポリゴンが無い場合は何もしない
						if (Index == this->StartUnit->GetPolyIndex()) { continue; }				//スタート地点のポリゴンだった場合は何もしない
						auto& NowUnit = this->UnitArray[static_cast<size_t>(Index)];
						if (!NowUnit.SetPrevPolyUnit(PUnit, K)) {
							continue;
						}
						// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
						if (!NowUnit.SearchThisUnit(ActiveFirstUnit)) {
							NowUnit.ActiveNextUnit = ActiveFirstUnit;
							ActiveFirstUnit = &NowUnit;
						}
						// 隣接するポリゴンがゴール地点にあるポリゴンだったらゴールに辿り着いたフラグを立てる
						if (Index == this->GoalUnit->GetPolyIndex()) {
							Goal = true;
						}
					}
					PUnit = PUnit->ActiveNextUnit;
					if (PUnit == nullptr) { break; }
				}

				if (!ActiveFirstUnit) { return false; }			// スタート地点にあるポリゴンからゴール地点にあるポリゴンに辿り着けないということなので false を返す
				if (Goal) { break; }
			}
			PATHPLANNING_UNIT::SetNextIndex(this->GoalUnit, this->StartUnit);		// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
			return true;										// ここにきたらスタート地点からゴール地点までの経路が探索できたということなので true を返す
		}
		void Draw(void) const noexcept {
			PATHPLANNING_UNIT* Now = GetStartUnit();
			while (true) {
				if (!Now) { break; }
				Util::VECTOR3D Vec1 = BackGround::Instance()->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(Now->GetPolyIndex())).GetPos();

				DrawSphere3D(
					Vec1.get(),
					0.2f * Scale3DRate / 2.f,
					6,
					DxLib::GetColor(255, 255, 128),
					DxLib::GetColor(255, 255, 0),
					true
				);
				//
				if (Now == GoalUnit) {
					break;
				}
				Now = Now->GetNextPolyUnit();
			}
		}
		void Dispose(void) {
			this->UnitArray.clear();
		}
	};
}
class EarlyCharacter :public BaseObject {
	Util::VECTOR3D MyTarget = Util::VECTOR3D::zero();

	Util::VECTOR3D MyPosTarget = Util::VECTOR3D::zero();
	float Xrad = 0.f;
	float Yrad = 0.f;
	float Zrad = 0.f;
	float Speed = 0.f;
	float MovePer = 0.f;
	Util::VECTOR2D VecR = Util::VECTOR2D::zero();
	float													m_YVec{};
	std::array<float, static_cast<int>(EarlyCharaAnim::Max)>		m_AnimPer{};
	float				m_YradDif{};
	uint8_t				m_MoveKey{};
	char		padding[3]{};
	Sound::SoundUniqueID runfootID{ InvalidID };
	int					m_FootSoundID{};
	int										TargetPathPlanningIndex{ 0 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	AIs::PathChecker								m_PathChecker;
	float									m_PathUpdateTimer{ 0.f };
	char		padding2[4]{};
public:
	EarlyCharacter(void) noexcept {}
	EarlyCharacter(const EarlyCharacter&) = delete;
	EarlyCharacter(EarlyCharacter&&) = delete;
	EarlyCharacter& operator=(const EarlyCharacter&) = delete;
	EarlyCharacter& operator=(EarlyCharacter&&) = delete;
	virtual ~EarlyCharacter(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(EarlyCharaFrame::Max); }
	const char*		GetFrameStr(int id) noexcept override { return EarlyCharaFrameName[id]; }
public:
	const Util::Matrix4x4 GetEyeMat(void) const noexcept;
	float GetSpeed(void) const noexcept { return Speed; }
	float GetSpeedMax(void) const noexcept {
		return 2.f * Scale3DRate / 60.f;
	}
	void SetPos(Util::VECTOR3D MyPos) noexcept {
		MyPosTarget = MyPos - Util::VECTOR3D::up() * Scale3DRate;
		if (!BackGround::Instance()->CheckLine(MyPos + Util::VECTOR3D::up() * Scale3DRate, &MyPosTarget)) {
			MyPosTarget = MyPos;
		}
		MyMat = Util::Matrix4x4::Mtrans(MyPosTarget);
	}

	void		ChangePoint() noexcept {
		auto* BackGroundParts = BackGround::Instance();
		Util::VECTOR3D MyPos = MyPosTarget;

		this->TargetPathPlanningIndex = -1;
		for (int i = 0; i < 10; i++) {
			this->m_PathChecker.Dispose();
			if (m_PathChecker.Init(MyPos, MyTarget)) {	// 指定の２点の経路情報を探索する
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
				break;
			}
			else {
				MyPos = BackGroundParts->GetWayPoint()->GetRandomPoint(MyPos, 10.f * Scale3DRate);//選定できない場合10m以内で再選定
			}
		}
	}

	void		SetTarget(const Util::VECTOR3D& pos) noexcept {
		MyTarget = pos;
	}
public:
	void Load_Sub(void) noexcept override {
		runfootID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/runfoot.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(MyMat.pos(), 10.f * Scale3DRate);
	}
	void Init_Sub(void) noexcept override {
		Speed = 0.f;
		this->m_PathUpdateTimer = 1.f;
	}
	void Update_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		ModelID.DrawModel();

		return;
		auto Pos = (MyPosTarget + Util::VECTOR3D::up() * (1.f * Scale3DRate));

		int X = this->TargetPathPlanningIndex;
		Util::VECTOR3D Vec3D = m_PathChecker.GetNextPoint(Pos, &X);

		DxLib::DrawLine3D(Vec3D.get(), Pos.get(), DxLib::GetColor(255, 0, 0));

		for (const auto& m : BackGround::Instance()->GetWayPoint()->GetWayPoints()) {
			Util::VECTOR3D Vec1 = m.GetPos();
			//if (!((Vec1 - Pos).magnitude() < 10.f * Scale3DRate)) { continue; }
			//*
			for (int K = 0; K < 8; K++) {
				int LinkIndex = m.GetLinkPolyIndex(K);
				if (LinkIndex == -1) { continue; }
				Util::VECTOR3D Vec2 = BackGround::Instance()->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(LinkIndex)).GetPos();

				DxLib::DrawLine3D(Vec1.get(), Vec2.get(), DxLib::GetColor(255, 255, 0));
			}
			//*/
			/*
			DrawCube3D(
				m.GetMinPos().get(),
				m.GetMaxPos().get(),
				DxLib::GetColor(255, 0, 0),
				DxLib::GetColor(255, 0, 0),
				false
			);
			//*/
		}
		m_PathChecker.Draw();
	}
	void ShadowDraw_Sub(void) const noexcept override {
		ModelID.DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		ModelID.Dispose();
	}
};
