#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/MV1.hpp"
#include "../Draw/Voxel.hpp"

class WayPointClass {
public:
	class Builds {
		struct LinkBuffer {
			bool isActive{ false };
			char		padding[3]{};
			Util::VECTOR3D Pos{};
			int ID;
		};
	private:
		int							m_mesh{ -1 };
		Util::VECTOR3D					m_MinPos;
		Util::VECTOR3D					m_Pos;
		Util::VECTOR3D					m_MaxPos;
		int							MyIndex{ 0 };
	public:
		std::array<LinkBuffer, 8>	m_LinkPosBuffer{};
	public:
		int	GetIndex(void) const noexcept { return MyIndex; }
		int	GetLinkPolyIndex(int ID) const noexcept { return this->m_LinkPosBuffer[static_cast<size_t>(ID)].ID; }
		void		SetLinkBuffer(int tris, const Util::VECTOR3D& pos) noexcept {
			this->m_LinkPosBuffer[static_cast<size_t>(tris)].isActive = true;
			this->m_LinkPosBuffer[static_cast<size_t>(tris)].Pos = pos;
		}
	public:
		const Util::VECTOR3D& GetMinPos(void) const noexcept { return this->m_MinPos; }
		const Util::VECTOR3D& GetPos(void) const noexcept { return this->m_Pos; }
		const Util::VECTOR3D& GetMaxPos(void) const noexcept { return this->m_MaxPos; }
	public:
		void		Set(int index) noexcept {
			this->MyIndex = index;
			for (auto& L : this->m_LinkPosBuffer) {
				L.isActive = false;
				L.ID = -1;
			}
		}
		void		SetPosition(Util::VECTOR3D MinPos, Util::VECTOR3D MaxPos) noexcept {
			this->m_MinPos = MinPos;
			this->m_MaxPos = MaxPos;
			this->m_Pos = (MinPos + MaxPos) / 2;
		}
	};
private:
	std::vector<Builds>				m_WayPoints;
	char		padding[4]{};
	int								m_SeekPoint{ 0 };
public:
	std::vector<Builds>& SetWayPoints(void) noexcept { return this->m_WayPoints; }
	const std::vector<Builds>& GetWayPoints(void) const noexcept { return this->m_WayPoints; }
public:
	//所定の位置からNm未満かNm以上の座標を取得する
	Util::VECTOR3D		GetRandomPoint(const Util::VECTOR3D& Pos, float RangeSmaller = InvalidID, float RangeOver = InvalidID) const noexcept {
		std::vector<int> SelList;
		SelList.clear();
		for (auto& C : this->m_WayPoints) {
			int index = static_cast<int>(&C - &this->m_WayPoints.front());
			Util::VECTOR3D Vec = C.GetPos() - Pos; Vec.y = (0.f);
			if (RangeSmaller != InvalidID && RangeOver != InvalidID) {
				if (Vec.IsRangeSmaller(RangeSmaller) && !Vec.IsRangeSmaller(RangeOver)) {
					SelList.emplace_back(index);
				}
			}
			else {
				if (RangeSmaller != InvalidID && Vec.IsRangeSmaller(RangeSmaller)) {
					SelList.emplace_back(index);
				}
				if (RangeOver != InvalidID && !Vec.IsRangeSmaller(RangeOver)) {
					SelList.emplace_back(index);
				}
			}
		}
		if (SelList.size() == 0) {
			return Pos;
		}
		return this->m_WayPoints.at(static_cast<size_t>(SelList.at(static_cast<size_t>(GetRand((int)SelList.size() - 1))))).GetPos();
	}

	int		GetNearestBuilds(const Util::VECTOR3D& NowPosition) const noexcept {
		for (auto& bu : this->m_WayPoints) {
			if (
				(bu.GetMaxPos().x >= NowPosition.x && NowPosition.x >= bu.GetMinPos().x) &&
				(bu.GetMaxPos().y >= NowPosition.y && NowPosition.y >= bu.GetMinPos().y) &&
				(bu.GetMaxPos().z >= NowPosition.z && NowPosition.z >= bu.GetMinPos().z)
				) {
				return static_cast<int>(&bu - &this->m_WayPoints.front());
			}
		}
		return -1;
	}
	int		GetNearestBuilds2(const Util::VECTOR3D& NowPosition) const noexcept {
		float Len = 10.f * Scale3DRate * 10.f * Scale3DRate;
		int Answer = -1;
		for (auto& bu : this->m_WayPoints) {
			Util::VECTOR3D Length = bu.GetPos() - NowPosition;
			//Length.y = 0.f;
			if (Len >= Length.sqrMagnitude()) {
				Len = Length.sqrMagnitude();
				Answer = static_cast<int>(&bu - &this->m_WayPoints.front());
			}
		}
		return Answer;
	}
	// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
	bool			CheckPolyMove(Util::VECTOR3D StartPos, Util::VECTOR3D TargetPos) const {
		int Rate = 6;

		int CheckPoly[4]{};
		int CheckPolyPrev[4]{};
		int NextCheckPoly[4]{};
		int NextCheckPolyPrev[4]{};

		// 開始座標と目標座標の y座標値を 0.0f にして、平面上の判定にする
		StartPos.y = (0.0f);
		TargetPos.y = (0.0f);

		// 開始座標と目標座標の直上、若しくは直下に存在するポリゴンを検索する
		int StartPoly = this->GetNearestBuilds(StartPos);
		int TargetPoly = this->GetNearestBuilds(TargetPos);

		// ポリゴンが存在しなかったら移動できないので false を返す
		if (StartPoly == -1 || TargetPoly == -1) { return false; }

		// 指定線分上にあるかどうかをチェックするポリゴンとして開始座標の直上、若しくは直下に存在するポリゴンを登録
		int CheckPolyNum = 1;
		CheckPoly[0] = StartPoly;
		int CheckPolyPrevNum = 0;
		CheckPolyPrev[0] = -1;

		// 結果が出るまで無条件で繰り返し
		while (true) {
			int NextCheckPolyNum = 0;			// 次のループでチェック対象になるポリゴンの数をリセットしておく
			int NextCheckPolyPrevNum = 0;			// 次のループでチェック対象から外すポリゴンの数をリセットしておく
			// チェック対象のポリゴンの数だけ繰り返し
			for (int i = 0; i < CheckPolyNum; i++) {
				int Index = CheckPoly[i];
				// チェック対象のポリゴンの３座標を取得 y座標を0.0にして、平面的な判定を行うようにする
				Util::VECTOR3D Pos = this->m_WayPoints.at(static_cast<size_t>(Index)).GetPos(); Pos.y = (0.f);

				for (int K = 0; K < 8; K++) {
					int LinkIndex = this->m_WayPoints.at(static_cast<size_t>(Index)).GetLinkPolyIndex(K);

					;

					Util::VECTOR3D PolyPos = Pos;
					PolyPos.x += ((static_cast<float>(Rate) / 2.f) * ((K == 0 || K == 1) ? 1.f : -1.f));
					PolyPos.z += ((static_cast<float>(Rate) / 2.f) * ((K == 1 || K == 2) ? 1.f : -1.f));
					int K2 = (K + 1) % 4;
					Util::VECTOR3D PolyPos2 = Pos;
					PolyPos2.x += ((static_cast<float>(Rate) / 2.f) * ((K2 == 0 || K2 == 1) ? 1.f : -1.f));
					PolyPos2.z += ((static_cast<float>(Rate) / 2.f) * ((K2 == 1 || K2 == 2) ? 1.f : -1.f));
					// ポリゴンの頂点番号0と1の辺に隣接するポリゴンが存在する場合で、
					// 且つ辺の線分と移動開始点、終了点で形成する線分が接していたら if 文が真になる
					if (LinkIndex != -1 && Util::GetMinLenSegmentToSegment(StartPos, TargetPos, PolyPos, PolyPos2) < 0.01f) {
						// もし辺と接しているポリゴンが目標座標上に存在するポリゴンだったら 開始座標から目標座標上まで途切れなくポリゴンが存在するということなので true を返す
						if (LinkIndex == TargetPoly) { return true; }

						// 辺と接しているポリゴンを次のチェック対象のポリゴンに加える

						// 既に登録されているポリゴンの場合は加えない
						int j = 0;
						for (j = 0; j < NextCheckPolyNum; j++) {
							if (NextCheckPoly[j] == LinkIndex) { break; }
						}
						if (j == NextCheckPolyNum) {
							// 次のループで除外するポリゴンの対象に加える

							// 既に登録されている除外ポリゴンの場合は加えない
							int j2 = 0;
							for (j2 = 0; j2 < NextCheckPolyPrevNum; j2++) {
								if (NextCheckPolyPrev[j2] == Index) { break; }
							}
							if (j2 == NextCheckPolyPrevNum) {
								NextCheckPolyPrev[NextCheckPolyPrevNum] = Index;
								NextCheckPolyPrevNum++;
							}

							// 一つ前のループでチェック対象になったポリゴンの場合も加えない
							int j3 = 0;
							for (j3 = 0; j3 < CheckPolyPrevNum; j3++) {
								if (CheckPolyPrev[j3] == LinkIndex) { break; }
							}
							if (j3 == CheckPolyPrevNum) {
								// ここまで来たら漸く次のチェック対象のポリゴンに加える
								NextCheckPoly[NextCheckPolyNum] = LinkIndex;
								NextCheckPolyNum++;
							}
						}
					}
				}
			}

			// 次のループでチェック対象になるポリゴンが一つもなかったということは
			// 移動開始点、終了点で形成する線分と接するチェック対象のポリゴンに隣接する
			// ポリゴンが一つもなかったということなので、直線的な移動はできないということで false を返す
			if (NextCheckPolyNum == 0) { return false; }

			// 次にチェック対象となるポリゴンの情報をコピーする
			for (int i = 0; i < NextCheckPolyNum; i++) {
				CheckPoly[i] = NextCheckPoly[i];
			}
			CheckPolyNum = NextCheckPolyNum;

			// 次にチェック対象外となるポリゴンの情報をコピーする
			for (int i = 0; i < NextCheckPolyPrevNum; i++) {
				CheckPolyPrev[i] = NextCheckPolyPrev[i];
			}
			CheckPolyPrevNum = NextCheckPolyPrevNum;
		}
	}
	bool			CheckPolyMoveWidth(Util::VECTOR3D StartPos, int TargetIndex, float Width) const {
		// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
		Util::VECTOR3D TargetPos = this->m_WayPoints.at(static_cast<size_t>(TargetIndex)).GetPos();
		// 最初に開始座標から目標座標に直線的に移動できるかどうかをチェック
		if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }

		Util::VECTOR3D Direction = TargetPos - StartPos;		// 開始座標から目標座標に向かうベクトルを算出
		Direction.y = (0.0f);		// y座標を 0.0f にして平面的なベクトルにする

		// 開始座標から目標座標に向かうベクトルに直角な正規化ベクトルを算出
		Util::VECTOR3D SideDirection = Util::VECTOR3D::Cross(Direction, Util::VECTOR3D::up()).normalized();
		{
			// 開始座標と目標座標を Width / 2.0f 分だけ垂直方向にずらして、再度直線的に移動できるかどうかをチェック
			Util::VECTOR3D TempVec = SideDirection * (Width / 2.0f);
			if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
				return false;
			}
		}
		{
			// 開始座標と目標座標を Width / 2.0f 分だけ一つ前とは逆方向の垂直方向にずらして、再度直線的に移動できるかどうかをチェック
			Util::VECTOR3D TempVec = SideDirection * (-Width / 2.0f);
			if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
				return false;
			}
		}
		return true;		// ここまできたら指定の幅があっても直線的に移動できるということなので true を返す
	}
public:
	void			Init(int Count) noexcept {
		this->m_WayPoints.resize(static_cast<size_t>(Count));
		this->m_SeekPoint = 0;
	}
	auto& AddWayPoint(Util::VECTOR3D MinPos, Util::VECTOR3D MaxPos) noexcept {
		auto& w = this->m_WayPoints.at(static_cast<size_t>(this->m_SeekPoint));
		w.Set(this->m_SeekPoint);
		w.SetPosition(MinPos, MaxPos);
		++this->m_SeekPoint;
		return w;
	}
	void			Setup(void) noexcept {
		for (auto& w : this->m_WayPoints) {
			for (auto& L : w.m_LinkPosBuffer) {
				if (L.isActive) {
					L.ID = GetNearestBuilds(L.Pos);
				}
			}
		}
	}
};

class BackGround : public Util::SingletonBase<BackGround> {
private:
	friend class Util::SingletonBase<BackGround>;
private:
	std::array<BG::VoxelControl, 2>		m_Voxel;
	std::array<BG::VoxelControl*, 2>	m_pVoxel{};
	BG::VoxelControl::ThreadJobs					m_Jobs{};			// 生成用スレッド
	Draw::MV1				SkyBoxID{};
	std::unique_ptr<WayPointClass>		m_WayPoint;
	Util::VECTOR3D					VecT;
	int								m_VOfs{ 0 };
	std::string						m_NextMap{};
public:
	Util::VECTOR3D					m_Offset{};
private:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
	auto			GetVoxelPoint(const Util::VECTOR3D& StartPos) const noexcept {
		BG::Algorithm::Vector3Int Pos = m_pVoxel.at(0)->GetReferenceCells().GetVoxelPoint(StartPos);
		Pos.x -= m_pVoxel.at(0)->GetReferenceCells().All / 2;
		Pos.z -= m_pVoxel.at(0)->GetReferenceCells().All / 2;
		return Pos;
	}
	auto			GetWorldPos(const BG::Algorithm::Vector3Int& StartPos) const noexcept {
		BG::Algorithm::Vector3Int Pos = StartPos;
		//Pos.x += m_pVoxel.at(0)->GetReferenceCells().All / 2;
		//Pos.z += m_pVoxel.at(0)->GetReferenceCells().All / 2;
		return m_pVoxel.at(0)->GetReferenceCells().GetWorldPos(Pos);
	}
	int				CheckLine(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, Util::VECTOR3D* Normal = nullptr) const noexcept {
		Util::VECTOR3D EP = *EndPos - m_Offset;
		auto Answer = m_pVoxel.at(0)->CheckLine(StartPos - m_Offset, &EP, Normal);
		*EndPos = EP + m_Offset;
		return Answer;
	}
	bool			CheckWall(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& AddCapsuleMin, const Util::VECTOR3D& AddCapsuleMax, float Radius, const std::vector<const Draw::MV1*>& addonColObj) const noexcept {
		Util::VECTOR3D EP = *EndPos - m_Offset;
		auto Answer = m_pVoxel.at(0)->CheckWall(StartPos - m_Offset, &EP, AddCapsuleMin, AddCapsuleMax, Radius, addonColObj);
		*EndPos = EP + m_Offset;
		return Answer;
	}
	const auto&		GetMapInfo(void) const noexcept { return m_pVoxel.at(0)->m_MapInfo; }
	void			SettingChange(int DrawLOD, int ShadowLOD) noexcept { m_pVoxel.at(0)->SettingChange(DrawLOD, ShadowLOD); }
	const auto&		GetWayPoint(void) const noexcept { return this->m_WayPoint; }
public:
	void Load() noexcept {
		m_Voxel.at(0).Load("data/maptex.png");							// 事前読み込み
		m_Voxel.at(1).Load("data/maptex.png");							// 事前読み込み

		Draw::MV1::Load("data/model/SkyBox/model.mqoz", &SkyBoxID);

	}
	void Init(void) noexcept {
		m_NextMap = (GetRand(100) < 50) ? "Map2" : "Map1";


		m_Voxel.at(0).InitStart();											// 初期化開始時処理
		m_Voxel.at(0).LoadCellsFile("Map1");					// ボクセルデータの読み込み
		m_Voxel.at(0).InitEnd();											// 初期化終了時処理

		m_Voxel.at(1).InitStart();											// 初期化開始時処理
		m_Voxel.at(1).LoadCellsFile(m_NextMap);					// ボクセルデータの読み込み
		m_Voxel.at(1).InitEnd();											// 初期化終了時処理

		m_pVoxel.at(0) = &m_Voxel.at(0);
		m_pVoxel.at(1) = &m_Voxel.at(1);

		m_Jobs.Init([&]() {
			m_pVoxel.at(1)->InitStart2();											// 初期化開始時処理
			m_pVoxel.at(1)->LoadCellsFile(m_NextMap);					// ボクセルデータの読み込み
			m_pVoxel.at(1)->InitEnd();											// 初期化終了時処理
			}, [&]() {}, true);

		this->m_Offset = Util::VECTOR3D::vget(0.f, 0.f, 0.f) * Scale3DRate;

		m_Jobs.JobStart();
	}
	void ChangeOffset(void) noexcept {
		this->m_Offset += VecT;
		++this->m_VOfs;
		
		m_pVoxel.at(0) = &m_Voxel.at(static_cast<size_t>((m_VOfs + 0) % 2));
		m_pVoxel.at(1) = &m_Voxel.at(static_cast<size_t>((m_VOfs + 1) % 2));

		m_NextMap = (GetRand(100) < 50) ? "Map2" : "Map1";
	
		m_Jobs.JobStart();
	}
	void Update(void) noexcept {
		auto* CameraParts = Camera::Camera3D::Instance();
		auto* PostPassParts = Draw::PostPassEffect::Instance();
		// ボクセル処理
		m_pVoxel.at(0)->SetDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(), m_Offset + Util::VECTOR3D::vget(0.f, 0.f, 0.f) * Scale3DRate,
			(CameraParts->GetCameraForDraw().GetCamVec() - CameraParts->GetCameraForDraw().GetCamPos()).normalized());// 描画する際の描画中心座標と描画する向きを指定
		m_pVoxel.at(0)->SetShadowDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(),
			PostPassParts->GetAmbientLightVec());// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		m_pVoxel.at(0)->Update();

		{
			Util::VECTOR3D Vec1;
			Util::VECTOR3D Vec2;
			for (auto& m : m_pVoxel.at(0)->m_MapInfo) {
				if (m.m_InfoType == InfoType::None || m.m_InfoType == InfoType::Max) { continue; }
				switch (m.m_InfoType) {
				case InfoType::Exit1:
				case InfoType::Exit3:
					break;
				case InfoType::Exit2:
					Vec1 = GetWorldPos(m.m_pos) + m_Offset;
					Vec1.y = 0.f;
					break;
				case InfoType::Entrance1:
				case InfoType::Entrance2:
				case InfoType::Entrance3:
				case InfoType::WayPoint:
				case InfoType::WayPoint2:
				case InfoType::AmmoBox:
				case InfoType::None:
				case InfoType::Max:
				default:
					break;
				}
			}
			for (auto& m : m_pVoxel.at(1)->m_MapInfo) {
				if (m.m_InfoType == InfoType::None || m.m_InfoType == InfoType::Max) { continue; }
				switch (m.m_InfoType) {
				case InfoType::Entrance1:
					Vec2 = GetWorldPos(m.m_pos) + m_Offset;
					Vec2.y = 0.f;
					Vec2 *= -1.f;
					break;
				case InfoType::Entrance2:
				case InfoType::Entrance3:
				case InfoType::Exit1:
				case InfoType::Exit2:
				case InfoType::Exit3:
				case InfoType::WayPoint:
				case InfoType::WayPoint2:
				case InfoType::AmmoBox:
				case InfoType::None:
				case InfoType::Max:
				default:
					break;
				}
			}
			VecT = Vec1 + Vec2;
			clsDx();
			printfDx("(%5.2f,%5.2f,%5.2f)\n", VecT.x / Scale3DRate, VecT.y / Scale3DRate, VecT.z / Scale3DRate);
		}
		m_pVoxel.at(1)->SetDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(), m_Offset + VecT,
			(CameraParts->GetCameraForDraw().GetCamVec() - CameraParts->GetCameraForDraw().GetCamPos()).normalized());// 描画する際の描画中心座標と描画する向きを指定
		m_pVoxel.at(1)->SetShadowDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(),
			PostPassParts->GetAmbientLightVec());// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		m_pVoxel.at(1)->Update();

		SkyBoxID.SetMatrix(Util::Matrix4x4::Mtrans(CameraParts->GetCameraForDraw().GetCamPos()));
		m_Jobs.Update(true);
	}
	void Dispose(void) noexcept {
		m_Jobs.Dispose();
		m_Voxel.at(0).Dispose();
		m_Voxel.at(0).Dispose_Load();

		m_Voxel.at(1).Dispose();
		m_Voxel.at(1).Dispose_Load();

		SkyBoxID.Dispose();
	}

	void BGDraw(void) const noexcept {
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid(void) const noexcept {
		m_pVoxel.at(0)->DrawByShader();
		m_pVoxel.at(1)->DrawByShader();
	}
	void SetShadowDraw(void) const noexcept {}
	void Draw(void) const noexcept {
		m_pVoxel.at(0)->Draw();
		m_pVoxel.at(1)->Draw();
		/*
		{
			for (auto& m : GetMapInfo()) {
				unsigned int Color = 0;
				switch (m.m_InfoType) {
				case InfoType::Entrance1:
					Color = GetColor(255, 0, 0);
					break;
				case InfoType::Entrance2:
					Color = GetColor(255, 64, 0);
					break;
				case InfoType::Entrance3:
					Color = GetColor(255, 128, 0);
					break;
				case InfoType::Exit1:
					Color = GetColor(255, 255, 0);
					break;
				case InfoType::Exit2:
					Color = GetColor(255, 255, 64);
					break;
				case InfoType::Exit3:
					Color = GetColor(255, 255, 128);
					break;
				case InfoType::None:
				case InfoType::Max:
				default:
					break;
				}
				DrawSphere3D(m_pVoxel.at(0)->GetReferenceCells().GetWorldPos(m.m_pos).get(), 0.125f * Scale3DRate, 8, Color, GetColor(0, 0, 0), TRUE);
			}
		}
		//*/
	}
	void ShadowDrawFar(void) const noexcept {
		m_pVoxel.at(0)->DrawShadow();
		m_pVoxel.at(1)->DrawShadow();
	}
	void ShadowDraw(void) const noexcept {}
};
