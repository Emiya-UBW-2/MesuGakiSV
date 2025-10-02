#include "DxLib.h"

#include	"BackGroundSub.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	int ShadowMapHandle = -1;
	BackGround::VoxelControl Voxel;
	VECTOR ShadowVec = VGet(0.1f, -0.8f, -0.1f);
	VECTOR MyPos = VGet(0.f, 0.f, 0.f);
	VECTOR MyDir = VGet(0.f, 0.f, -1.f);
	VECTOR MyVec = VGet(0.f, 0.f, 0.f);
	float Yrad = 0.f;
	float Speed = 0.f;
	// 初期化
	DxLib_Init();												// ＤＸライブラリ初期化処理
	ChangeWindowMode(TRUE);										// ウィンドウモード
	SetAlwaysRunFlag(TRUE);										// 非アクティブでも動くようにする
	// 読み込み
	Voxel.Load();												// 事前読み込み
	ShadowMapHandle = MakeShadowMap(2048, 2048);				// シャドウマップハンドルの作成
	// 初期化
	SetLightDirection(ShadowVec);								// ライトの方向を設定
	SetShadowMapLightDirection(ShadowMapHandle, ShadowVec);		// シャドウマップが想定するライトの方向もセット

	Voxel.InitStart();											// 初期化開始時処理
	Voxel.LoadCellsFile("data/Map.txt");						// ボクセルデータの読み込み
	Voxel.InitEnd();											// 初期化終了時処理
	// 自分の座標と向きを指定
	MyPos = VGet(0.f, 0.f, 0.f);
	MyDir = VGet(0.f, 0.f, -1.f);
	// 自分の座標を地形に合わせる
	MyPos = VGet(MyPos.x, -50.f, MyPos.z);
	Voxel.CheckLine(VGet(MyPos.x, 0.f, MyPos.z), &MyPos);
	// メインループ
	while (ProcessMessage() == 0) {
		// 左右回転
		if (CheckHitKey(KEY_INPUT_A) != 0) {
			Yrad -= 1.f / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_D) != 0) {
			Yrad += 1.f / GetFPS();
		}
		// 進行方向に前進後退
		if (CheckHitKey(KEY_INPUT_W) != 0) {
			Speed = 1.f / GetFPS();
		}
		else if (CheckHitKey(KEY_INPUT_S) != 0) {
			Speed = -1.f / GetFPS();
		}
		else {
			Speed = 0.f;
		}
		MyDir = VTransform(VGet(0.f, 0.f, -1.f), MGetRotAxis(VGet(0.f, 1.f, 0.f), Yrad));
		MyVec = VScale(MyDir, Speed);

		// 移動ベクトルを加算した仮座標を作成
		VECTOR PosBuffer = VAdd(MyPos, MyVec);
		// 壁判定
		std::vector<int> addonColObj;
		Voxel.CheckWall(MyPos, &PosBuffer, VGet(0.f, 0.7f, 0.f), VGet(0.f, 1.6f, 0.f), 0.7f, addonColObj);// 現在地から仮座標に進んだ場合
		// 地面判定
		PosBuffer = VGet(PosBuffer.x, PosBuffer.y - 0.1f, PosBuffer.z);
		if (!Voxel.CheckLine(VGet(PosBuffer.x, PosBuffer.y + 1.f, PosBuffer.z), &PosBuffer)) {
			// ヒットしていない際は落下させる(加速度は無視)
			PosBuffer.y -= 9.8f / (GetFPS() * GetFPS());
		}
		// 仮座標を反映
		MyPos = PosBuffer;
		// カメラ座標を指定
		VECTOR CamPos = MyPos; CamPos.y += 1.f;// 自分の座標の1m上を示す
		VECTOR CamTarget = VAdd(CamPos, MyDir);// 自身が向いている方向を注視点とする
		// FPSを表示
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		// シャドウマップに描画する範囲を設定
		SetShadowMapDrawArea(ShadowMapHandle, VAdd(CamPos, VGet(-10.0f, -10.0f, -10.0f)), VAdd(CamPos, VGet(10.0f, 10.0f, 10.0f)));
		// ボクセル処理
		Voxel.SetDrawInfo(CamPos, VNorm(VSub(CamTarget, CamPos)));// 描画する際の描画中心座標と描画する向きを指定
		Voxel.SetShadowDrawInfo(CamPos, ShadowVec);// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		Voxel.Update();
		// シャドウマップへの描画
		ShadowMap_DrawSetup(ShadowMapHandle);		// シャドウマップへの描画の準備
		Voxel.DrawShadow();							// シャドウマップへの描画
		ShadowMap_DrawEnd();						// シャドウマップへの描画を終了
		// 裏画面への描画
		SetDrawScreen(DX_SCREEN_BACK);				// 描画先を裏画面に変更
		{
			ClearDrawScreen();											// 画面をクリア
			DrawBox(0, 0, 640, 480, GetColor(150, 250, 255), TRUE);		// 背景色を塗る

			SetCameraPositionAndTarget_UpVecY(CamPos, CamTarget);// カメラの位置と向きを設定
			SetCameraNearFar(0.5f, 100.0f);			// 描画する奥行き方向の範囲を設定

			SetUseShadowMap(0, ShadowMapHandle);	// 描画にシャドウマップを使用する
			Voxel.Draw();							// 描画
			SetUseShadowMap(0, -1);					// 描画に使用するシャドウマップの設定を解除
		}
		ScreenFlip();								// 裏画面の内容を表画面に反映
	}
	// 終了時の処理
	Voxel.Dispose();
	// 読み込みの破棄処理
	Voxel.Dispose_Load();
	DeleteShadowMap(ShadowMapHandle);	// シャドウマップの削除

	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;					// ソフトの終了
}
