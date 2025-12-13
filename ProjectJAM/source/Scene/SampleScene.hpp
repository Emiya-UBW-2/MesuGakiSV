#pragma once

#include "../Manage/Enum.hpp"
#include "../Manage/Util.hpp"
#include "../Manage/SceneManager.hpp"
#include "../Manage/MainDraw.hpp"

#include "SceneEnum.hpp"

/// <summary>
/// サンプルシーン
/// </summary>
class SampleScene : public Scene::SceneBase {
public:
	SampleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Sample)); }
	SampleScene(const SampleScene&) = delete;
	SampleScene(SampleScene&&) = delete;
	SampleScene& operator=(const SampleScene&) = delete;
	SampleScene& operator=(SampleScene&&) = delete;
	virtual ~SampleScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override {
		//非同期読み込みをする個所
	}
	void Init_Sub(void) noexcept override {
		//初期化時に記載する内容を記述
	}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = Scene::SceneManager::Instance();//管理人のポインタ呼び出し
		//60FPSで更新する内容を記述
		if (false) {
			//シーン先を指定して遷移するサンプル
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Sample)));//Sampleシーンを次に指定
			SceneBase::SetEndScene();//現在のシーンを終了
		}
		if (false) {
			//ゲームを終了するサンプル
			SceneBase::SetEndGame();//ゲームを終了
		}
	}
	void Draw_Sub(void) noexcept override {
		//キャラクターなどを描画するところ
		DrawBox(0, 0, DispWidth, DispHeight, ColorPalette::Blue, TRUE);				// 背景
		DrawString(64, 64, "Sample Draw", ColorPalette::Red);
	}
	void UIDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();//管理人のポインタ呼び出し
		//体力バーなどUIを描画するところ
		DrawString(DispWidth -300, 64, "Sample UI", ColorPalette::White);
		DrawString(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), "Mouse Point", ColorPalette::Red);
	}
	void Dispose_Sub(void) noexcept override {
		//シーン遷移時に後始末する箇所
	}
};
