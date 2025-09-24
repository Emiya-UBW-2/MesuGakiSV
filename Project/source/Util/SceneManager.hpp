#pragma once

#pragma warning(disable:4464)
#include "Util.hpp"
#include "../Draw/PostPass.hpp"

#pragma warning(disable:4710)
#pragma warning( push, 3 )
#include <vector>
#pragma warning( pop )

namespace Util {
	class SceneBase {
		const SceneBase* m_pNextScene{ nullptr };
		int					m_SceneID{ 0 };
		bool				m_IsEndGame{ false };
		bool				m_IsEndScene{ false };
		char		padding[2]{};
	public:
		SceneBase(void) noexcept {}
		virtual ~SceneBase(void) noexcept {}
	public:
		auto GetID(void) const noexcept { return this->m_SceneID; }
		const SceneBase* GetNextScene(void) const noexcept { return this->m_pNextScene; }
		bool IsEndGame(void) const noexcept { return this->m_IsEndGame; }
		bool IsEndScene(void) const noexcept { return this->m_IsEndScene; }
	protected:
		virtual void SetID(int SceneID) noexcept { this->m_SceneID = SceneID; }
	public:
		void SetNextScene(const SceneBase* Ptr) noexcept { this->m_pNextScene = Ptr; }
		void SetEndGame(void) noexcept { this->m_IsEndGame = true; }
		void SetEndScene(void) noexcept { this->m_IsEndScene = true; }
	public:
		void Init(void) noexcept {
			this->m_IsEndGame = false;
			this->m_IsEndScene = false;

			auto* PostPassParts = DXLibRef::PostPassEffect::Instance();
			PostPassParts->ResetAllParams();
			PostPassParts->ResetAllBuffer();
			PostPassParts->UpdateActive();

			Init_Sub();
		}
		void Update(void) noexcept { Update_Sub(); }
		void Draw(void) noexcept {
			Draw_Sub();
		}
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
		SceneBase* m_FirstScene{ nullptr };
		std::vector<const SceneBase*>	m_pScene{};
		SceneBase* m_NowScene{ nullptr };
		EnumScenePhase					m_Phase{ EnumScenePhase::GoNext };
	private:
		SceneManager(void) noexcept {
			this->m_NowScene = nullptr;
			this->m_Phase = EnumScenePhase::GoNext;
		}
		SceneManager(const SceneManager&) = delete;
		SceneManager(SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;
		SceneManager& operator=(SceneManager&&) = delete;
		~SceneManager(void) noexcept {}
	public:
		auto IsEndScene(void) const noexcept { return (this->m_Phase == EnumScenePhase::GoEnd); }
	public:
		const SceneBase* GetScene(int SceneID) const noexcept {
			for (auto& pS : this->m_pScene) {
				if (pS->GetID() == SceneID) {
					return pS;
				}
			}
			return nullptr;
		}
	public:
		void AddScenePtrList(const SceneBase* Ptr) noexcept { this->m_pScene.emplace_back(Ptr); }
		void SetFirstScene(const SceneBase* Ptr) noexcept { this->m_FirstScene = const_cast<SceneBase*>(Ptr); }
	public:
		void Update(void) noexcept;
		void Draw(void) noexcept;
	};
}
