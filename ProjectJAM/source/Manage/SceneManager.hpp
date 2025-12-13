#pragma once

#include "Util.hpp"
#include "MainDraw.hpp"

#include <vector>

namespace Scene {
	/// <summary>
	/// シーンの基底クラス
	/// </summary>
	class SceneBase {
		const SceneBase*	m_pNextScene{ nullptr };
		int					m_SceneID{ 0 };
		bool				m_IsEndGame{ false };
		bool				m_IsEndScene{ false };
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
		void Load(void) noexcept { Load_Sub(); }
		void Init(void) noexcept {
			this->m_IsEndGame = false;
			this->m_IsEndScene = false;
			Init_Sub();
		}
		void Update(void) noexcept { Update_Sub(); }
		void Draw(void) noexcept { Draw_Sub(); }
		void UIDraw(void) noexcept { UIDraw_Sub(); }
		void Dispose(void) noexcept { Dispose_Sub(); }
	public:
		virtual void Load_Sub(void) noexcept = 0;
		virtual void Init_Sub(void) noexcept = 0;
		virtual void Update_Sub(void) noexcept = 0;
		virtual void Draw_Sub(void) noexcept = 0;
		virtual void UIDraw_Sub(void) noexcept = 0;
		virtual void Dispose_Sub(void) noexcept = 0;
	};

	/// <summary>
	/// シーンの常駐管理人
	/// </summary>
	class SceneManager : public Util::SingletonBase<SceneManager> {
	private:
		friend class Util::SingletonBase<SceneManager>;
	private:
		enum class EnumScenePhase : size_t {
			Load,
			Update,
			GoNext,
			GoEnd,
		};
	private:
		SceneBase*						m_FirstScene{ nullptr };
		std::vector<const SceneBase*>	m_pScene{};
		SceneBase*						m_NowScene{ nullptr };
		EnumScenePhase					m_Phase{ EnumScenePhase::GoNext };
		int								m_ASyncLoadNum{ 0 };
		bool							m_LoadEndSwitch{ false };
		bool							m_IsUpdateActive{ false };
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
		void Draw(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->Draw();
			}
		}
		void UIDraw(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				//ロード画面
				DrawBox(0, 0, DispWidth, DispHeight, ColorPalette::Black, TRUE);				// 背景
				if (this->m_ASyncLoadNum > 0) {
					DrawBox(0, 0, DispWidth * (this->m_ASyncLoadNum - GetASyncLoadNum()) / this->m_ASyncLoadNum, DispHeight, ColorPalette::Green, TRUE);// 進捗ゲージ
				}
				return;
			}
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
			case EnumScenePhase::Load:
				if (this->m_NowScene) {
					if (GetASyncLoadNum() == 0) {
						this->m_Phase = EnumScenePhase::Update;
						//
						this->m_NowScene->Init();
						this->m_LoadEndSwitch = true;
					}
				}
				break;
			case EnumScenePhase::Update:
				this->m_LoadEndSwitch = false;
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
					SetUseASyncLoadFlag(true);
					this->m_NowScene->Load();
					SetUseASyncLoadFlag(false);
					this->m_Phase = EnumScenePhase::Load;
					this->m_ASyncLoadNum = GetASyncLoadNum();
				}
				break;
			case EnumScenePhase::GoEnd:
				if (this->m_NowScene) {
					this->m_NowScene->Dispose();
				}
				break;
			default:
				break;
			}
		}
	};
}
