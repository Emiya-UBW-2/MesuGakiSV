#pragma once

#pragma warning(disable:4464)
#include "Util.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Camera.hpp"

#pragma warning(disable:4710)
#pragma warning( push, 3 )
#include <vector>
#pragma warning( pop )

namespace Util {
	class SceneBase {
		const SceneBase*	m_pNextScene{ nullptr };
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
			Init_Sub();
		}
		void Update(void) noexcept { Update_Sub(); }
		void BGDraw(void) noexcept { BGDraw_Sub(); }
		void Draw(void) noexcept { Draw_Sub(); }
		void UIDraw(void) noexcept { UIDraw_Sub(); }
		void Dispose(void) noexcept { Dispose_Sub(); }
		virtual void Init_Sub(void) noexcept = 0;
		virtual void Update_Sub(void) noexcept = 0;
		virtual void BGDraw_Sub(void) noexcept = 0;
		virtual void Draw_Sub(void) noexcept = 0;
		virtual void UIDraw_Sub(void) noexcept = 0;
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
		virtual ~SceneManager(void) noexcept {}
	public:
		void CubeMapDraw(void) noexcept {
		}
		void BGDraw(void) noexcept {
			if (this->m_NowScene) {
				this->m_NowScene->BGDraw();
			}
		}
		void ShadowFarDraw3D(void) noexcept {
			if (this->m_NowScene) {
				this->m_NowScene->Draw();
			}
		}
		void ShadowDraw3D(void) noexcept {
			if (this->m_NowScene) {
				this->m_NowScene->Draw();
			}
		}
		void Draw3D(void) noexcept {
			if (this->m_NowScene) {
				this->m_NowScene->Draw();
			}
		}
		void UIDraw(void) noexcept {
			if (this->m_NowScene) {
				this->m_NowScene->UIDraw();
			}
		}
	public:
		auto IsEndScene(void) const noexcept { return (this->m_Phase == EnumScenePhase::GoEnd); }
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
		void Update(void) noexcept {
#if _DEBUG
			if (!this->m_FirstScene) {
				MessageBox(NULL, "FirstScene doesnt Setting Yet", "", MB_OK);
				exit(-1);
			}
#endif
			switch (this->m_Phase) {
			case EnumScenePhase::Update:
				if (this->m_NowScene) {
					this->m_NowScene->Update();
					if (this->m_NowScene->IsEndScene()) {
						this->m_Phase = EnumScenePhase::GoNext;
					}
					if (this->m_NowScene->IsEndGame()) {
						this->m_Phase = EnumScenePhase::GoEnd;
					}
				}
				break;
			case EnumScenePhase::GoNext:
				if (this->m_NowScene) {
					this->m_NowScene->Dispose();
				}
				if (!this->m_NowScene) {
					this->m_NowScene = this->m_FirstScene;
				}
				else {
					this->m_NowScene = const_cast<SceneBase*>(this->m_NowScene->GetNextScene());
				}
				if (this->m_NowScene) {
					auto* PostPassParts = Draw::PostPassEffect::Instance();
					PostPassParts->Reset();
					this->m_NowScene->Init();
					this->m_Phase = EnumScenePhase::Update;
				}
				break;
			case EnumScenePhase::GoEnd:
				if (this->m_NowScene) {
					this->m_NowScene->Dispose();
					auto* PostPassParts = Draw::PostPassEffect::Instance();
					PostPassParts->Reset();
				}
				break;
			default:
				break;
			}
		}
	};
}
