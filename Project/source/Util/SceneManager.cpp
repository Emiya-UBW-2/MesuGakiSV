#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5259)
#include "SceneManager.hpp"

#include "../Draw/Camera.hpp"

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

		DXLibRef::Camera3D::Instance()->Update();
	}

	void SceneManager::Draw(void) noexcept {
		int Prev = GetDrawMode();
		/*
		auto* PostPassParts = DXLibRef::PostPassEffect::Instance();
		auto* CameraParts = DXLibRef::Camera3D::Instance();
		{
			// キューブマップをセット
			{
				Vector3DX Pos = CameraParts->GetMainCamera().GetCamPos(); Pos.y *= -1.f;
				PostPassParts->Update_CubeMap([]() {}, Pos);
			}
			// 影をセット
			if (PostPassParts->UpdateShadowActive() && false) {
				Vector3DX Pos = CameraParts->GetMainCamera().GetCamPos();
				Pos.x = 0.f;
				Pos.z = 0.f;
				PostPassParts->Update_Shadow([]() {}, Pos, true);
			}
			PostPassParts->Update_Shadow([]() {}, CameraParts->GetMainCamera().GetCamPos(), false);
		}
		Draw::Camera3DInfo camInfo = CameraParts->GetMainCamera();
		// 全ての画面を初期化
		PostPassParts->ResetBuffer();
		{
			// カメラ
			PostPassParts->SetCamMat(camInfo);
			// 空
			PostPassParts->DrawGBuffer(1000.0f, 50000.0f, []() {});
			// 遠距離
			PostPassParts->DrawGBuffer(camInfo.GetCamFar(), 1000000.f, []() {
				auto* PostPassParts = DXLibRef::PostPassEffect::Instance();
				PostPassParts->DrawByPBR([]() {});
				});
			// 中間
			PostPassParts->DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), []() {
				auto* PostPassParts = DXLibRef::PostPassEffect::Instance();
				PostPassParts->DrawByPBR([]() {});
				});
			// 至近
			PostPassParts->DrawGBuffer(0.01f, camInfo.GetCamNear(), []() {
				auto* PostPassParts = DXLibRef::PostPassEffect::Instance();
				PostPassParts->DrawByPBR([]() {});
				});
			// 影
			PostPassParts->SetDrawShadow(camInfo, []() {}, []() {});
		}
		// ポストプロセス
		PostPassParts->DrawPostProcess();
		//*/
		SetDrawScreen(Prev);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		{
			//auto* DrawerMngr = Draw::MainDraw::Instance();
			//PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), false);
			if (this->m_NowScene) {
				this->m_NowScene->Draw();
			}
		}
	}
}