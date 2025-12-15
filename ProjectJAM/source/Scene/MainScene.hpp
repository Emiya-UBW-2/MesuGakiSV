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


class Object2DBase {
private:
	struct Move {
		Util::VECTOR2D					m_Pos{};
		float							m_Rad{};
	};
private:
	const Draw::GraphHandle* m_Pic{};
	Move					m_Pos{};
	std::array<Move, 3>		m_PosRe{};
	int						m_UniqueID{};
	bool					m_IsActive{ true };
	char		padding[3]{};
public:
	Object2DBase(void) noexcept {}
	virtual ~Object2DBase(void) noexcept {}
	Object2DBase(const Object2DBase&) = delete;
	Object2DBase(Object2DBase&&) = delete;
	Object2DBase& operator=(const Object2DBase&) = delete;
	Object2DBase& operator=(Object2DBase&&) = delete;
public:
	Move& SetPos() noexcept { return m_Pos; }
public:
	void SetPtr(const Draw::GraphHandle* ptr) noexcept { m_Pic = ptr; }
	void SetActive(bool IsActive) noexcept { m_IsActive = IsActive; }
	bool IsActive() const noexcept { return m_IsActive; }

	void SetUniqueID(int UniqueID) noexcept { m_UniqueID = UniqueID; }
	int GetUniqueID() const noexcept { return m_UniqueID; }
public:
	void Init(void) noexcept {
		// フレーム
		Init_Sub();
	}
	void Update(void) noexcept {
		if (!IsActive()) { return; }
		Update_Sub();
		for (size_t loop = 1; loop < m_PosRe.size(); ++loop) {
			m_PosRe.at(loop - 1) = m_PosRe.at(loop);
		}
		m_PosRe.back() = m_Pos;
	}
	void Draw(void) const noexcept {
		if (!IsActive()) { return; }
		Draw_Sub();
		if (m_Pic) {
			for (size_t loop = 0; loop < m_PosRe.size(); ++loop) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255 * (loop + 1) / m_PosRe.size()), 0, 255));
				m_Pic->DrawRotaGraph(static_cast<int>(m_PosRe.at(loop).m_Pos.x), static_cast<int>(m_PosRe.at(loop).m_Pos.y), 1.f, m_PosRe.at(loop).m_Rad, true);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	void Dispose() noexcept {
		Dispose_Sub();
	}
protected:
	virtual void Init_Sub(void) noexcept = 0;
	virtual void Update_Sub(void) noexcept = 0;
	virtual void Draw_Sub(void) const noexcept = 0;
	virtual void Dispose_Sub(void) noexcept = 0;
};
using SharedObj2D = std::shared_ptr<Object2DBase>;

class Object2DManager : public Util::SingletonBase<Object2DManager> {
private:
	friend class Util::SingletonBase<Object2DManager>;
private:
	std::vector<SharedObj2D>		m_ObjectList;
	int							m_LastUniqueID{ 0 };
	char		padding[4]{};
private:
	Object2DManager(void) noexcept {
		this->m_ObjectList.reserve(256);
	}
	Object2DManager(const Object2DManager&) = delete;
	Object2DManager(Object2DManager&&) = delete;
	Object2DManager& operator=(const Object2DManager&) = delete;
	Object2DManager& operator=(Object2DManager&&) = delete;
	virtual ~Object2DManager(void) noexcept {
		this->m_ObjectList.clear();
		this->m_ObjectList.shrink_to_fit();
	}
public:
	void			AddObject(const SharedObj2D& pObj) noexcept {
		this->m_ObjectList.emplace_back(pObj);
		this->m_ObjectList.back()->Init();
		this->m_ObjectList.back()->SetUniqueID(m_LastUniqueID);
		++m_LastUniqueID;
	}
public:
	SharedObj2D* GetObj(int UniqueID) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (object->GetUniqueID() == UniqueID) {
				return &object;
			}
		}
		return nullptr;
	}
public:
	void			UpdateObject(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->Update();
		}
	}
	void			Draw(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->Draw();
		}
	}
	void			DeleteAll(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->Dispose();
			object.reset();
		}
		this->m_ObjectList.clear();

		this->m_LastUniqueID = 0;// 一旦ユニークIDもリセット

	}
};

class ObjectAmmo : public Object2DBase {
public:
	Util::VECTOR2D					m_Vec{};
public:
	ObjectAmmo(void) noexcept {}
	virtual ~ObjectAmmo(void) noexcept {}
	ObjectAmmo(const ObjectAmmo&) = delete;
	ObjectAmmo(ObjectAmmo&&) = delete;
	ObjectAmmo& operator=(const ObjectAmmo&) = delete;
	ObjectAmmo& operator=(ObjectAmmo&&) = delete;
public:
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		SetPos().m_Pos += m_Vec * DeltaTime;
		SetPos().m_Rad -= Util::deg2rad(60) * DeltaTime;
		SetActive(SetPos().m_Pos.y >= -100.f);
	}
	void Draw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};
class AmmoPool : public Util::SingletonBase<AmmoPool> {
private:
	friend class Util::SingletonBase<AmmoPool>;
private:
	std::vector<std::shared_ptr<ObjectAmmo>>			m_AmmoPos{};
private:
	AmmoPool(void) noexcept {
		this->m_AmmoPos.reserve(256);
	}
	virtual ~AmmoPool(void) noexcept {
		this->m_AmmoPos.clear();
		this->m_AmmoPos.shrink_to_fit();
	}
	AmmoPool(const AmmoPool&) = delete;
	AmmoPool(AmmoPool&&) = delete;
	AmmoPool& operator=(const AmmoPool&) = delete;
	AmmoPool& operator=(AmmoPool&&) = delete;
public:
	void SetAmmo(const Util::VECTOR2D& pos, const Util::VECTOR2D& vec, const Draw::GraphHandle* ptr) noexcept {
		for (auto& a : m_AmmoPos) {
			if (!a->IsActive()) {
				a->SetActive(true);
				a->SetPos().m_Pos = pos;
				a->SetPos().m_Rad = Util::deg2rad(GetRand(90));
				a->m_Vec = vec;
				a->SetPtr(ptr);
				return;
			}
		}
		m_AmmoPos.emplace_back();
		auto& a = m_AmmoPos.back();
		a = std::make_shared<ObjectAmmo>();
		Object2DManager::Instance()->AddObject(a);
		a->SetActive(true);
		a->SetPos().m_Pos = pos;
		a->SetPos().m_Rad = Util::deg2rad(GetRand(90));
		a->m_Vec = vec;
		a->SetPtr(ptr);

	}
};


struct EnemyPicture {
	const Draw::GraphHandle* m_picture{};
	int								m_EndAnim{};
	char		padding[4]{};
};
struct EnemyMove {
	int				m_Frame{};
	Util::VECTOR2D	m_Pos;
	bool			m_IsEnd{ false };
	char		padding[3]{};
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
	int							m_Anim{};
	char		padding[4]{};
	size_t						m_Frame{};

	float						m_MoveAnim{};
	char		padding2[4]{};
	size_t						m_MoveFrame{};
public:
	ObjectEnemy(void) noexcept {}
	virtual ~ObjectEnemy(void) noexcept {}
	ObjectEnemy(const ObjectEnemy&) = delete;
	ObjectEnemy(ObjectEnemy&&) = delete;
	ObjectEnemy& operator=(const ObjectEnemy&) = delete;
	ObjectEnemy& operator=(ObjectEnemy&&) = delete;
public:
	void InitEnemy(EnemyData* pEnemyData) noexcept {
		m_EnemyData = pEnemyData;
	}
public:
	void Init_Sub(void) noexcept override{
		SetActive(true);
		m_Frame = 0;
	}
	void Update_Sub(void) noexcept override {
		{
			m_Anim++;
			if (m_Anim > m_EnemyData->m_PicTotalFrame) {
				m_Anim = 0;
			}
			if (m_EnemyData->m_Picture.size() > 0) {
				auto& f = m_EnemyData->m_Picture.at(m_Frame);
				SetPtr(f.m_picture);
				if (m_Anim >= f.m_EndAnim) {
					++m_Frame %= m_EnemyData->m_Picture.size();
				}
			}
		}
		{
			if (m_EnemyData->m_Move.size() - 1 > m_MoveFrame) {
				auto& Now = m_EnemyData->m_Move.at(m_MoveFrame);
				auto& Next = m_EnemyData->m_Move.at(m_MoveFrame + 1);
				auto MoveAnimPer = static_cast<float>(m_MoveAnim - static_cast<float>(Now.m_Frame)) / static_cast<float>(Next.m_Frame - Now.m_Frame);
				if (static_cast<float>(Next.m_Frame) <= m_MoveAnim) {
					++m_MoveFrame;
				}
				SetPos().m_Pos = Util::Lerp(Now.m_Pos, Next.m_Pos, MoveAnimPer);
				if (Now.m_IsEnd) {
					SetActive(false);
				}
				m_MoveAnim += 1.0;// 0.75f;//todo
			}
			else {
				SetActive(false);
			}
		}
	}
	void Draw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};

class EnemyPool : public Util::SingletonBase<EnemyPool> {
private:
	friend class Util::SingletonBase<EnemyPool>;
public:
	std::vector<std::shared_ptr<ObjectEnemy>>			m_EnemyPos{};
private:
	EnemyPool(void) noexcept {
		this->m_EnemyPos.reserve(256);
	}
	virtual ~EnemyPool(void) noexcept {
		this->m_EnemyPos.clear();
		this->m_EnemyPos.shrink_to_fit();
	}
	EnemyPool(const EnemyPool&) = delete;
	EnemyPool(EnemyPool&&) = delete;
	EnemyPool& operator=(const EnemyPool&) = delete;
	EnemyPool& operator=(EnemyPool&&) = delete;
public:
	void SetEnemy(EnemyData* ptr) noexcept {
		m_EnemyPos.emplace_back();
		auto& a = m_EnemyPos.back();
		a = std::make_shared<ObjectEnemy>();
		Object2DManager::Instance()->AddObject(a);
		a->SetActive(true);
		a->InitEnemy(ptr);
	}
};

class ObjectOption : public Object2DBase {
	const Draw::GraphHandle* m_ReimuOption{};
public:
	ObjectOption(void) noexcept {}
	virtual ~ObjectOption(void) noexcept {}
	ObjectOption(const ObjectOption&) = delete;
	ObjectOption(ObjectOption&&) = delete;
	ObjectOption& operator=(const ObjectOption&) = delete;
	ObjectOption& operator=(ObjectOption&&) = delete;
public:
	void Init_Sub(void) noexcept override {
		m_ReimuOption = Draw::GraphPool::Instance()->Get("data/Image/Option.png")->Get();
	}
	void Update_Sub(void) noexcept override {
		SetPtr(m_ReimuOption);
	}
	void Draw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};
class ObjectMine : public Object2DBase {
	const Draw::GraphHandle* m_ReimuStay{};
	const Draw::GraphHandle* m_ReimuLeft{};
	const Draw::GraphHandle* m_ReimuRight{};
	const Draw::GraphHandle* m_Ammo00{};
	const Draw::GraphHandle* m_Ammo01{};
	std::shared_ptr<ObjectOption>	m_ReimuOpt0{};
	std::shared_ptr<ObjectOption>	m_ReimuOpt1{};
	float							m_ReimuSlowPer{};
	float							m_Ammo00ShotTimer{};
	float							m_Ammo01ShotTimer{};
	float							m_ReimuSpeed{};
public:
	ObjectMine(void) noexcept {}
	virtual ~ObjectMine(void) noexcept {}
	ObjectMine(const ObjectMine&) = delete;
	ObjectMine(ObjectMine&&) = delete;
	ObjectMine& operator=(const ObjectMine&) = delete;
	ObjectMine& operator=(ObjectMine&&) = delete;
public:
	void Init_Sub(void) noexcept override {
		m_ReimuStay = Draw::GraphPool::Instance()->Get("data/Image/stay00.png")->Get();
		m_ReimuLeft = Draw::GraphPool::Instance()->Get("data/Image/left00.png")->Get();
		m_ReimuRight = Draw::GraphPool::Instance()->Get("data/Image/right00.png")->Get();

		m_Ammo00 = Draw::GraphPool::Instance()->Get("data/Image/Ammo00.png")->Get();
		m_Ammo01 = Draw::GraphPool::Instance()->Get("data/Image/Ammo01.png")->Get();

		m_ReimuOpt0 = std::make_shared<ObjectOption>();
		Object2DManager::Instance()->AddObject(m_ReimuOpt0);

		m_ReimuOpt1 = std::make_shared<ObjectOption>();
		Object2DManager::Instance()->AddObject(m_ReimuOpt1);
	}
	void Update_Sub(void) noexcept override {
		auto* KeyMngr = Util::KeyParam::Instance();
		SetPtr(m_ReimuStay);

		bool IsSlow = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run);
		if (IsSlow) {
			m_ReimuSpeed = 300.f * DeltaTime;
		}
		else {
			m_ReimuSpeed = 600.f * DeltaTime;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::A)) {
			SetPtr(m_ReimuLeft);
			SetPos().m_Pos.x -= m_ReimuSpeed;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::D)) {
			SetPtr(m_ReimuRight);
			SetPos().m_Pos.x += m_ReimuSpeed;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::W)) {
			SetPos().m_Pos.y -= m_ReimuSpeed;
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::S)) {
			SetPos().m_Pos.y += m_ReimuSpeed;
		}

		SetPos().m_Pos.x = std::clamp(SetPos().m_Pos.x, static_cast<float>(1920 / 2 - 864 / 2 + 50), static_cast<float>(1920 / 2 + 864 / 2 - 50));
		SetPos().m_Pos.y = std::clamp(SetPos().m_Pos.y, static_cast<float>(36 + 50), static_cast<float>(1080 - 36 - 50));

		m_ReimuSlowPer = std::clamp(m_ReimuSlowPer + (IsSlow ? DeltaTime : -DeltaTime), 0.f, 1.f);
		m_ReimuOpt0->SetPos().m_Pos = SetPos().m_Pos + Util::Lerp(Util::VECTOR2D::vget(-50.f, 0.f), Util::VECTOR2D::vget(-20.f, -50.f), m_ReimuSlowPer);
		m_ReimuOpt1->SetPos().m_Pos = SetPos().m_Pos + Util::Lerp(Util::VECTOR2D::vget(50.f, -0.f), Util::VECTOR2D::vget(20.f, -50.f), m_ReimuSlowPer);
		m_ReimuOpt0->SetPos().m_Rad -= Util::deg2rad(360) * DeltaTime;
		m_ReimuOpt1->SetPos().m_Rad += Util::deg2rad(360) * DeltaTime;

		{

			if (m_Ammo00ShotTimer <= 0.f) {
				if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
					m_Ammo00ShotTimer = 0.1f;
					if (IsSlow) {
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(-18.f, -10.f), Util::VECTOR2D::vget(-100.f, -1200.f), m_Ammo00);
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(-12.f, -10.f), Util::VECTOR2D::vget(0.f, -1200.f), m_Ammo00);
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(12.f, -10.f), Util::VECTOR2D::vget(0.f, -1200.f), m_Ammo00);
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(18.f, -10.f), Util::VECTOR2D::vget(100.f, -1200.f), m_Ammo00);
					}
					else {
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(-18.f, -10.f), Util::VECTOR2D::vget(-200.f, -1200.f), m_Ammo00);
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(-12.f, -10.f), Util::VECTOR2D::vget(0.f, -1200.f), m_Ammo00);
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(12.f, -10.f), Util::VECTOR2D::vget(0.f, -1200.f), m_Ammo00);
						AmmoPool::Instance()->SetAmmo(SetPos().m_Pos + Util::VECTOR2D::vget(18.f, -10.f), Util::VECTOR2D::vget(200.f, -1200.f), m_Ammo00);
					}
				}
			}
			else {
				m_Ammo00ShotTimer = std::max(m_Ammo00ShotTimer - DeltaTime, 0.f);
			}
			if (m_Ammo01ShotTimer <= 0.f) {
				if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
					m_Ammo01ShotTimer = 0.1f;
					if (IsSlow) {
						AmmoPool::Instance()->SetAmmo(m_ReimuOpt0->SetPos().m_Pos, Util::VECTOR2D::vget(-1200.f, -1200.f), m_Ammo01);
						AmmoPool::Instance()->SetAmmo(m_ReimuOpt1->SetPos().m_Pos, Util::VECTOR2D::vget(1200.f, -1200.f), m_Ammo01);
					}
					else {
						AmmoPool::Instance()->SetAmmo(m_ReimuOpt0->SetPos().m_Pos, Util::VECTOR2D::vget(-1200.f, -1200.f), m_Ammo01);
						AmmoPool::Instance()->SetAmmo(m_ReimuOpt1->SetPos().m_Pos, Util::VECTOR2D::vget(1200.f, -1200.f), m_Ammo01);
					}
				}
			}
			else {
				m_Ammo01ShotTimer = std::max(m_Ammo01ShotTimer - DeltaTime, 0.f);
			}
		}
	}
	void Draw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};

struct StageData {
	std::string m_Enemy{};
	int			m_Frame{};
	char		padding[4]{};
};
class StageScript {
	std::vector<EnemyData>	m_EnemyData;
	std::vector<StageData>	m_StageData;
	int							m_StoryStart{};
	int							m_Anim{};
	bool						m_IsStory = false;
	bool						m_IsClear = false;
	char		padding[2]{};

	int			m_StartFrame{};
	int			m_SkipFrame{};
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
	bool IsClear() { return m_IsClear; }
	bool IsStory() { return m_IsStory; }
	void SetStoryEnd() noexcept { m_IsStory = false; }
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
				if (Args.at(0) == "SkipWhenKillEnemyAll") {
					m_StartFrame = std::stoi(Args.at(1));
					m_SkipFrame = std::stoi(Args.at(2));
				}
				if (Args.at(0) == "StoryStart") {
					m_StoryStart = std::stoi(Args.at(1));
				}
			}
		}
		FileStream.Close();

		for (auto& s : m_StageData) {
			auto* ptr = GetEnemyData(s.m_Enemy);
			if (ptr) {
				EnemyPool::Instance()->SetEnemy(ptr);
			}
		}
	}

	void Update() noexcept {
		for (auto& e : EnemyPool::Instance()->m_EnemyPos) {
			size_t index = static_cast<size_t>(&e - &EnemyPool::Instance()->m_EnemyPos.front());
			if (m_Anim < m_StageData.at(index).m_Frame) {
				e->SetActive(false);
			}
			if (m_Anim == m_StageData.at(index).m_Frame) {
				e->SetActive(true);
			}
		}
		if (m_IsStory || m_IsClear) {

		}
		else {
			++m_Anim;
			if (m_StartFrame < m_Anim && m_Anim <= m_SkipFrame) {
				//全ての敵が消えていたらクリア
				bool IsClearAll = true;
				for (auto& e : EnemyPool::Instance()->m_EnemyPos) {
					if (e->IsActive()) {
						IsClearAll = false;
						break;
					}
				}
				if (IsClearAll) {
					//m_SkipEnemy;
					m_Anim = m_SkipFrame;
				}
			}


			if (m_Anim == m_StoryStart) {
				m_IsStory = true;
			}
			if (m_Anim > m_StoryStart) {
				//全ての敵が消えていたらクリア
				bool IsClearAll = true;
				for (auto& e : EnemyPool::Instance()->m_EnemyPos) {
					if (e->IsActive()) {
						IsClearAll = false;
						break;
					}
				}
				if (IsClearAll) {
					m_IsClear = true;
				}
			}
		}
	}
};


struct SpeakData {
	std::string m_Speaker;
	const Draw::GraphHandle* m_Image{};
	Util::VECTOR2D m_Pos;
	std::string m_Mes1;
	std::string m_Mes2;
};
class SpeakScript {
	std::vector<SpeakData>	m_SpeakData;
	int total_Mes1{};
	int total_Mes2{};

	float float_Mes{};

	float seek_Mes{};
	size_t m_NowPoint = 0;
	bool m_IsStart = false;
	bool m_IsEnd = false;
	char		padding[6]{};
public:
	bool IsEnd() const noexcept { return m_IsEnd; }
	void SetStoryStart() noexcept {
		m_IsStart = true;
	}
public:
	void Load(const char* Path) noexcept {
		m_SpeakData.clear();

		File::InputFileStream FileStream;
		FileStream.Open(Path);
		while (true) {
			if (FileStream.ComeEof()) { break; }
			std::vector<std::string> Args;
			File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
			//
			{
				if (Args.at(0) == "speaker") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().m_Speaker = Args.at(1);
					m_SpeakData.back().m_Mes1 = "";
					m_SpeakData.back().m_Mes2 = "";
				}
				else if (Args.at(0) == "position") {
					if (Args.at(1) == "LEFT") {
						m_SpeakData.back().m_Pos.x = 1920 / 2 - 864 / 2 - 100 + 512 / 2;
						m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
					else if (Args.at(1) == "RIGHT") {
						m_SpeakData.back().m_Pos.x = 1920 / 2 + 864 / 2 + 100 - 512 / 2;
						m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
				}
				else if (Args.at(0) == "image") {
					m_SpeakData.back().m_Image = Draw::GraphPool::Instance()->Get(Args.at(1))->Get();
				}
				else if (Args.at(0) == "mes1") {
					m_SpeakData.back().m_Mes1 = Args.at(1);
				}
				else if (Args.at(0) == "mes2") {
					m_SpeakData.back().m_Mes2 = Args.at(1);
				}
			}
		}
		FileStream.Close();
		m_IsEnd = false;
		float_Mes = 1080.f;
	}

	void Update() noexcept {
		if (m_IsStart && !m_IsEnd) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (m_NowPoint < m_SpeakData.size()) {
				auto& Now = m_SpeakData.at(m_NowPoint);
				total_Mes1 = static_cast<int>(Now.m_Mes1.length() / 2);
				total_Mes2 = static_cast<int>(Now.m_Mes2.length() / 2);

				seek_Mes += DeltaTime / 0.1f;

				if (KeyMngr->GetBattleKeyRepeat(Util::EnumBattle::Attack)) {
					if (static_cast<int>(seek_Mes) >= total_Mes1 + total_Mes2) {
						++m_NowPoint;
						seek_Mes = 0.f;
					}
					else {
						seek_Mes = static_cast<float>(total_Mes1 + total_Mes2);
					}
				}
			}
			else {
				m_IsEnd = true;
			}
		}
		Util::Easing(&float_Mes, ((!m_IsStart || m_IsEnd) ? 1080.f : 0.f), 0.9f);
	}

	void Draw() noexcept {
		auto* Font = Draw::FontPool::Instance();

		for (size_t loop = 0; loop < m_SpeakData.size(); ++loop) {
			auto& Now = m_SpeakData.at(loop);
			if (loop < m_NowPoint) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawBright(0, 0, 0);
				Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
				SetDrawBright(255, 255, 255);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			if (loop == m_NowPoint) {
				//DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f* seek_Mes), 0, 255));
				Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);

				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((1080.f - float_Mes) / 1080.f * 128.f));
				DxLib::DrawBox(
					1920 / 2 - 864 / 2 + 64, 1080 - 36 - 12 - 125,
					1920 / 2 + 864 / 2 - 64, 1080 - 36 - 12,
					ColorPalette::Gray50, TRUE);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((1080.f - float_Mes) / 1080.f * 255.f));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::BOTTOM,
					1920 / 2 - 864 / 2 + 64 + 32, 1080 - 36 - 12 - 125 - 6,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Speaker));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
					1920 / 2 - 864 / 2 + 64 + 64, 1080 - 36 - 12 - 125 + 62 / 2,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Mes1.substr(0, static_cast<size_t>(std::clamp(static_cast<int>(seek_Mes), 0, total_Mes1) * 2))));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
					1920 / 2 - 864 / 2 + 64 + 64, 1080 - 36 - 12 - 125 + 62 + 62 / 2,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Mes2.substr(0, static_cast<size_t>(std::clamp(static_cast<int>(seek_Mes) - total_Mes1, 0, total_Mes2) * 2))));
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
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

	Sound::SoundUniqueID			m_NormalBGMID{ InvalidID };
	Sound::SoundUniqueID			m_BOSSBGMID{ InvalidID };
	const Draw::GraphHandle*		m_BackScreen{};

	StageScript						m_StageScript{};
	SpeakScript						m_SpeakScript{};
	SpeakScript						m_ClearScript{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
private:
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override {}
	void SetShadowDrawRigid_Sub(void) noexcept override {}
	void SetShadowDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) noexcept override {}
	void DepthDraw_Sub(void) noexcept override {}
	void ShadowDrawFar_Sub(void) noexcept override {}
	void ShadowDraw_Sub(void) noexcept override {}
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
