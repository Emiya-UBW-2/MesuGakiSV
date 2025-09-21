#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5259)
#include "SceneManager.hpp"

const Util::SceneManager* Util::SingletonBase<Util::SceneManager>::m_Singleton = nullptr;

namespace Util {
	void SceneManager::Update(void) noexcept {
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
				this->m_NowScene->Init();
				this->m_Phase = EnumScenePhase::Update;
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
}