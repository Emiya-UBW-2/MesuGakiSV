#define NOMINMAX

#include "DxLib.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <fstream>

static const int draw_x = 960;			//ウィンドウサイズX
static const int draw_y = 720;			//ウィンドウサイズY

// --------------------------------------------------------------------------------------------------
// シングルトン
// --------------------------------------------------------------------------------------------------
template <class T>
class SingletonBase {
private:
	static const T* m_Singleton;
public:
	static void Create(void) noexcept {
		m_Singleton = new T();
	}
	static T* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new T(); }
		return (T*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
protected:
	SingletonBase(void) noexcept {}
	virtual ~SingletonBase(void) noexcept {}
private:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
	SingletonBase(SingletonBase&&) = delete;
	SingletonBase& operator=(SingletonBase&&) = delete;
};
// 子のサンプル
/*
class A : public SingletonBase<A> {
private:
	friend class SingletonBase<A>;
}
//*/


//DXLib描画クラス
class DXLibDrawer : public SingletonBase<DXLibDrawer> {
private:
	friend class SingletonBase<DXLibDrawer>;
private:
	//コンストラクタ
	DXLibDrawer(void) noexcept {
		SetOutApplicationLogValidFlag(FALSE);           /*log*/
		SetMainWindowText("game title");                /*タイトル*/
		ChangeWindowMode(TRUE);                         /*窓表示*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);          /*directX ver*/
		SetGraphMode(draw_x, draw_y, 32);               /*解像度*/
		SetUseDirectInputFlag(TRUE);                    /*DirectInput使用*/
		SetDirectInputMouseMode(TRUE);                  /*DirectInputマウス使用*/
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);    /*ウインドウサイズを手動変更不可、ウインドウサイズに合わせて拡大もしないようにする*/
		SetUsePixelLighting(TRUE);                      /*ピクセルライティングの使用*/
		SetFullSceneAntiAliasingMode(4, 2);             /*アンチエイリアス*/
		SetWaitVSyncFlag(TRUE);                         /*垂直同期*/
		DxLib_Init();                                   /*ＤＸライブラリ初期化処理*/
		SetSysCommandOffFlag(TRUE);                     /*タスクスイッチを有効にするかどうかを設定する*/
		SetAlwaysRunFlag(TRUE);                         /*background*/
	}
	//デストラクタ
	~DXLibDrawer(void) noexcept {
		DxLib_End();
	}
};

const DXLibDrawer* SingletonBase<DXLibDrawer>::m_Singleton = nullptr;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXLibDrawer::Create();
	auto maincontrol = DXLibDrawer::Instance();
	//メインループ開始
	while ((ProcessMessage() == 0) && (CheckHitKey(KEY_INPUT_ESCAPE) == 0)) {
		//描画
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			//デバッグ表示
			clsDx();
			printfDx("FPS:%4.1f\n", GetFPS());

		}
		ScreenFlip();
	}
	DXLibDrawer::Release();
	return 0;// ソフトの終了 
}
