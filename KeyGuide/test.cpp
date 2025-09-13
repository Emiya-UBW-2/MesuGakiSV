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

//いろいろまとめるクラス
class DXDraw {
public:
	//コンストラクタ
	DXDraw(void) noexcept {
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
	~DXDraw(void) noexcept {
		DxLib_End();
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto maincontrol = std::make_unique<DXDraw>();
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
	return 0;// ソフトの終了 
}
