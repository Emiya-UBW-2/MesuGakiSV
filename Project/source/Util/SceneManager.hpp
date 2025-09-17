#pragma once

#include "Util.hpp"

#pragma warning( push, 3 )
#include <vector>
#pragma warning( pop )

class SceneBase {
	const SceneBase* m_pNextScene = nullptr;
	int m_SceneID{};
	bool m_IsEndGame = false;
	bool m_IsEndScene = false;
	char		padding[2]{};
public:
	SceneBase(void) noexcept {}
	virtual ~SceneBase(void) noexcept {}
public:
	auto GetID(void) const noexcept { return m_SceneID; }
	const SceneBase* GetNextScene(void) const noexcept { return m_pNextScene; }
	bool IsEndGame(void) const noexcept { return m_IsEndGame; }
	bool IsEndScene(void) const noexcept { return m_IsEndScene; }
protected:
	virtual void SetID(int SceneID) noexcept { this->m_SceneID = SceneID; }
public:
	void SetNextScene(const SceneBase* Ptr) noexcept { m_pNextScene = Ptr; }
	void SetEndGame(void) noexcept { m_IsEndGame = true; }
	void SetEndScene(void) noexcept { m_IsEndScene = true; }
public:
	void Init(void) noexcept {
		this->m_IsEndGame = false;
		this->m_IsEndScene = false;
		Init_Sub();
	}
	void Update(void) noexcept { Update_Sub(); }
	void Draw(void) noexcept { Draw_Sub(); }
	void Dispose(void) noexcept { Dispose_Sub(); }
	virtual void Init_Sub(void) noexcept = 0;
	virtual void Update_Sub(void) noexcept = 0;
	virtual void Draw_Sub(void) noexcept = 0;
	virtual void Dispose_Sub(void) noexcept = 0;
};

class SceneManager : public SingletonBase<SceneManager> {
private:
	friend class SingletonBase<SceneManager>;
private:
	enum class EnumScenePhase : size_t {
		Update,
		GoNext,
		GoEnd,
	};
private:
	SceneBase* m_FirstScene = nullptr;
	std::vector<const SceneBase*> m_pScene{};
	SceneBase* m_NowScene = nullptr;
	EnumScenePhase m_Phase = EnumScenePhase::GoNext;
private:
	SceneManager(void) noexcept {
		m_NowScene = nullptr;
		m_Phase = EnumScenePhase::GoNext;
	}
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;
	~SceneManager(void) noexcept {}
public:
	auto IsEndScene(void) const noexcept { return (m_Phase == EnumScenePhase::GoEnd); }
public:
	const SceneBase* GetScene(int SceneID) const noexcept {
		for (auto& pS : m_pScene) {
			if (pS->GetID() == SceneID) {
				return pS;
			}
		}
		return nullptr;
	}
public:
	void AddScenePtrList(const SceneBase* Ptr) noexcept {
		m_pScene.emplace_back(Ptr);
	}
	void SetFirstScene(const SceneBase* Ptr) noexcept {
		m_FirstScene = const_cast<SceneBase*>(Ptr);
	}
public:
	void Update(void) noexcept {
#if _DEBUG
		if (!m_FirstScene) {
			MessageBox(NULL, "FirstScene doesnt Setting Yet", "", MB_OK);
			exit(-1);
		}
#endif
		switch (m_Phase) {
		case EnumScenePhase::Update:
			if (m_NowScene) {
				m_NowScene->Update();
				if (m_NowScene->IsEndScene()) {
					m_Phase = EnumScenePhase::GoNext;
				}
				if (m_NowScene->IsEndGame()) {
					m_Phase = EnumScenePhase::GoEnd;
				}
			}
			break;
		case EnumScenePhase::GoNext:
			if (m_NowScene) {
				m_NowScene->Dispose();
			}
			if (!m_NowScene) {
				m_NowScene = m_FirstScene;
			}
			else {
				m_NowScene = const_cast<SceneBase*>(m_NowScene->GetNextScene());
			}
			if (m_NowScene) {
				m_NowScene->Init();
				m_Phase = EnumScenePhase::Update;
			}
			break;
		case EnumScenePhase::GoEnd:
			if (m_NowScene) {
				m_NowScene->Dispose();
			}
			break;
		default:
			break;
		}
	}
	void Draw(void) noexcept {
		if (m_NowScene) {
			m_NowScene->Draw();
		}
	}
};
