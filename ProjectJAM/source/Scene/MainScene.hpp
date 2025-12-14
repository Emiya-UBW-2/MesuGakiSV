#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"

#include "../Util/SceneManager.hpp"

struct Move {
	Util::VECTOR2D					m_Pos{};
	float							m_Rad{};
};

class Object2DBase {
	Move					m_Pos{};
	std::array<Move, 3>	m_PosRe{};
public:
	Move& SetPos() noexcept { return m_Pos; }
public:
	void Update() noexcept {
		for (size_t loop = 1; loop < m_PosRe.size(); ++loop) {
			m_PosRe.at(loop - 1) = m_PosRe.at(loop);
		}
		m_PosRe.back() = m_Pos;
	}
	void Draw(const Draw::GraphHandle* m_Pic) noexcept {
		for (size_t loop = 0; loop < m_PosRe.size(); ++loop) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255 * (loop + 1) / m_PosRe.size()), 0, 255));
			m_Pic->DrawRotaGraph(static_cast<int>(m_PosRe.at(loop).m_Pos.x), static_cast<int>(m_PosRe.at(loop).m_Pos.y), 1.f, m_PosRe.at(loop).m_Rad, true);
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
};
class ObjectAmmo : public Object2DBase {
public:
	Util::VECTOR2D					m_Vec{};
public:
	bool IsActive() noexcept {
		return (SetPos().m_Pos.y >= -100.f);
	}
	void UpdateAmmo() noexcept {
		SetPos().m_Pos += m_Vec * DeltaTime;
		SetPos().m_Rad -= Util::deg2rad(60) * DeltaTime;
	}
};


struct EnemyPicture {
	const Draw::GraphHandle* m_picture{};
	int								m_EndAnim{};
};
struct EnemyMove {
	int				m_Frame{};
	Util::VECTOR2D	m_Pos;
	bool			m_IsEnd{ false };
};

class EnemyData {
public:
	std::string					m_Path;
	std::vector<EnemyPicture>	m_Picture;
	std::vector<EnemyMove>		m_Move;
	int							m_PicTotalFrame{};
	int							m_HP{};
public:
	void Load(std::string Path) noexcept {
		m_Path = Path;
		m_Picture.clear();
		m_PicTotalFrame = 0;
		m_HP = 1;
		{
			File::InputFileStream FileStream;
			FileStream.Open("data/Enemy/" + Path + "/Data.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "Picture") {
						m_Picture.emplace_back();
						m_Picture.back().m_EndAnim = std::stoi(Args.at(1));
						m_Picture.back().m_picture = Draw::GraphPool::Instance()->Get(Args.at(2))->Get();

						m_PicTotalFrame = std::max(m_PicTotalFrame, m_Picture.back().m_EndAnim);
					}
					else if (Args.at(0) == "HitPoint") {
						m_HP = std::stoi(Args.at(1));
					}
				}
			}
			FileStream.Close();
		}
		{
			File::InputFileStream FileStream;
			FileStream.Open("data/Enemy/" + Path + "/Move.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "Time") {
						m_Move.emplace_back();
						if (m_Move.size() > 1) {
							m_Move.at(m_Move.size() - 1) = m_Move.at(m_Move.size() - 1 - 1);
						}
						m_Move.back().m_Frame = std::stoi(Args.at(1));
					}
					else if (Args.at(0) == "ToPoint") {
						m_Move.back().m_Pos.x = static_cast<float>((1920 / 2 - 864 / 2) + std::stoi(Args.at(1)));
						m_Move.back().m_Pos.y = static_cast<float>(36 + std::stoi(Args.at(2)));
					}
					else if (Args.at(0) == "Erace") {
						m_Move.back().m_IsEnd = true;
					}
				}
			}
			FileStream.Close();
		}
	}
};

class ObjectEnemy : public Object2DBase {
public:
	EnemyData* m_EnemyData{};
	const Draw::GraphHandle* m_enemy{};
	int							m_Anim{};
	size_t						m_Frame{};

	float						m_MoveAnim{};
	size_t						m_MoveFrame{};

	bool						m_IsActive{ true };
public:
	bool IsActive() noexcept {
		return m_IsActive;
	}
	void InitEnemy(EnemyData* pEnemyData) noexcept {
		m_EnemyData = pEnemyData;
		m_Frame = 0;
		m_IsActive = true;
	}
	void UpdateEnemy() noexcept {
		if (!IsActive()) { return; }
		{
			m_Anim++;
			if (m_Anim > m_EnemyData->m_PicTotalFrame) {
				m_Anim = 0;
			}
			if (m_EnemyData->m_Picture.size() > 0) {
				auto& f = m_EnemyData->m_Picture.at(m_Frame);
				m_enemy = f.m_picture;
				if (m_Anim >= f.m_EndAnim) {
					++m_Frame %= m_EnemyData->m_Picture.size();
				}
			}
		}
		{
			if (m_EnemyData->m_Move.size() - 1 > m_MoveFrame) {
				auto& Now = m_EnemyData->m_Move.at(m_MoveFrame);
				auto& Next = m_EnemyData->m_Move.at(m_MoveFrame + 1);
				auto MoveAnimPer = static_cast<float>(m_MoveAnim - Now.m_Frame) / static_cast<float>(Next.m_Frame - Now.m_Frame);
				if (Next.m_Frame <= m_MoveAnim) {
					++m_MoveFrame;
				}
				SetPos().m_Pos = Util::Lerp(Now.m_Pos, Next.m_Pos, MoveAnimPer);
				if (Now.m_IsEnd) {
					m_IsActive = false;
				}
				m_MoveAnim+=0.75f;//todo
			}
			else {
				m_IsActive = false;
			}
		}
	}
	void DrawEnemy() noexcept {
		if (!IsActive()) { return; }
		if (!m_enemy) { return; }
		Draw(m_enemy);
	}
};

struct StageData {
	std::string m_Enemy;
	int			m_Frame;
};
class StageScript {
public:
	std::vector<EnemyData>	m_EnemyData;
	std::vector<StageData>	m_StageData;
	std::vector<ObjectEnemy>	m_ObjectEnemy;
	int							m_Anim{};
public:
	EnemyData* GetEnemyData(std::string Make) noexcept {
		for (auto& e : m_EnemyData) {
			if (e.m_Path == Make) {
				return &e;
			}
		}
		return nullptr;
	}
	void LoadEnemyData(std::string Make) noexcept {
		if (GetEnemyData(Make)) {
			return;
		}
		m_EnemyData.emplace_back();
		m_EnemyData.back().Load(Make);
	}
public:
	void Load() noexcept {
		m_EnemyData.clear();
		m_StageData.clear();

		File::InputFileStream FileStream;
		FileStream.Open("data/stage.txt");
		while (true) {
			if (FileStream.ComeEof()) { break; }
			std::vector<std::string> Args;
			File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
			//
			{
				if (Args.at(0) == "MakeEnemy") {
					m_StageData.emplace_back();
					m_StageData.back().m_Enemy = Args.at(1);
					m_StageData.back().m_Frame = std::stoi(Args.at(2));
					LoadEnemyData(m_StageData.back().m_Enemy);
				}
			}
		}
		FileStream.Close();

		m_ObjectEnemy.clear();
		for (auto& s : m_StageData) {
			auto* ptr = GetEnemyData(s.m_Enemy);
			if (ptr) {
				m_ObjectEnemy.emplace_back();
				m_ObjectEnemy.back().InitEnemy(ptr);
			}
		}
	}

	void Update() noexcept {
		for (auto& e : m_ObjectEnemy) {
			int index = static_cast<int>(&e - &m_ObjectEnemy.front());
			if (m_Anim < m_StageData.at(index).m_Frame) { continue; }
			e.UpdateEnemy();
			e.Update();
		}
		++m_Anim;
	}

	void Draw() noexcept {
		for (auto& e : m_ObjectEnemy) {
			int index = static_cast<int>(&e - &m_ObjectEnemy.front());
			if (m_Anim < m_StageData.at(index).m_Frame) { continue; }
			e.DrawEnemy();
		}
	}
};



class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_Exit{ false };
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsResetMouse{ false };
	float							m_Fade{ 1.f };
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	Sound::SoundUniqueID			m_EnviID{ InvalidID };
	const Draw::GraphHandle*		m_BackScreen{};

	const Draw::GraphHandle* m_ReimuStay{};
	const Draw::GraphHandle* m_ReimuLeft{};
	const Draw::GraphHandle* m_ReimuRight{};
	const Draw::GraphHandle* m_Reimu{};

	const Draw::GraphHandle* m_ReimuOption{};

	const Draw::GraphHandle* m_Ammo00{};
	const Draw::GraphHandle* m_Ammo01{};

	Object2DBase					m_ReimuOpt0{};
	Object2DBase					m_ReimuOpt1{};
	Object2DBase					m_ReimuPos{};
	float							m_ReimuSpeed{};
	float							m_ReimuSlowPer{};

	StageScript						m_StageScript{};

	std::vector<ObjectAmmo>			m_Ammo00Pos{};
	float							m_Ammo00ShotTimer{};

	std::vector<ObjectAmmo>			m_Ammo01Pos{};
	float							m_Ammo01ShotTimer{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
private:
	void SetAmmo00(const Util::VECTOR2D& pos, const Util::VECTOR2D& vec) noexcept {
		for (auto& a : m_Ammo00Pos) {
			if (!a.IsActive()) {
				a.SetPos().m_Pos = pos;
				a.SetPos().m_Rad = Util::deg2rad(GetRand(90));
				a.m_Vec = vec;
				return;
			}
		}
		m_Ammo00Pos.emplace_back();
		m_Ammo00Pos.back().SetPos().m_Pos = pos;
		m_Ammo00Pos.back().SetPos().m_Rad = Util::deg2rad(GetRand(90));
		m_Ammo00Pos.back().m_Vec = vec;
	}
	void SetAmmo01(const Util::VECTOR2D& pos, const Util::VECTOR2D& vec) noexcept {
		for (auto& a : m_Ammo01Pos) {
			if (!a.IsActive()) {
				a.SetPos().m_Pos = pos;
				a.SetPos().m_Rad = Util::deg2rad(GetRand(90));
				a.m_Vec = vec;
				return;
			}
		}
		m_Ammo01Pos.emplace_back();
		m_Ammo01Pos.back().SetPos().m_Pos = pos;
		m_Ammo01Pos.back().SetPos().m_Rad = Util::deg2rad(GetRand(90));
		m_Ammo01Pos.back().m_Vec = vec;
	}
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override;
	void SetShadowDrawRigid_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) noexcept override;
	void Draw_Sub(void) noexcept override;
	void DepthDraw_Sub(void) noexcept override;
	void ShadowDrawFar_Sub(void) noexcept override;
	void ShadowDraw_Sub(void) noexcept override;
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
