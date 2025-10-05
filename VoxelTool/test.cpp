#include "DxLib.h"
#include	"BackGroundSub.hpp"

#include <algorithm>

class DX {
	int ShadowMapHandle = -1;
	VECTOR ShadowVec = VGet(0.1f, -0.3f, -0.1f);
public:
	DX() {
		DxLib::SetGraphMode(1280, 1000, 32);
		DxLib_Init();												// ＤＸライブラリ初期化処理
		ChangeWindowMode(TRUE);										// ウィンドウモード
		SetAlwaysRunFlag(TRUE);										// 非アクティブでも動くようにする
		SetMouseDispFlag(TRUE);
		ShadowMapHandle = MakeShadowMap(2048, 2048);				// シャドウマップハンドルの作成
		SetLightDirection(ShadowVec);								// ライトの方向を設定
		SetShadowMapLightDirection(ShadowMapHandle, ShadowVec);		// シャドウマップが想定するライトの方向もセット
	}
	~DX() {
		DeleteShadowMap(ShadowMapHandle);	// シャドウマップの削除
		DxLib_End();				// ＤＸライブラリ使用の終了処理
	}
public:
	VECTOR GetShadowVec() { return ShadowVec; }
public:
	void SetShadowMapStart(VECTOR CamPos) {
		SetShadowMapDrawArea(ShadowMapHandle, VAdd(CamPos, VGet(-50.0f, -50.0f, -50.0f)), VAdd(CamPos, VGet(50.0f, 0.0f, 50.0f)));
		ShadowMap_DrawSetup(ShadowMapHandle);		// シャドウマップへの描画の準備
	}
	void SetShadowMapEnd() {
		ShadowMap_DrawEnd();						// シャドウマップへの描画を終了
	}
	void SetUseShadowMapStart() {
		SetUseShadowMap(0, ShadowMapHandle);	// 描画にシャドウマップを使用する
	}
	void SetUseShadowMapEnd() {
		SetUseShadowMap(0, -1);					// 描画に使用するシャドウマップの設定を解除
	}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	BackGround::VoxelControl Voxel;
	VECTOR MyPos = VGet(0.f, 0.f, 0.f);
	VECTOR MyVec = VGet(0.f, 0.f, 0.f);
	float Xrad = -45.f * DX_PI_F / 180.f;
	float Yrad = 0.f;
	float Speed = 0.f;
	// 初期化
	DX DXParam;

	// 読み込み
	Voxel.Load();												// 事前読み込み

	Voxel.InitStart();											// 初期化開始時処理
	//Voxel.LoadCellsFile("data/Map.txt");						// ボクセルデータの読み込み
	{
		int XSize{}, YSize{};
		int SoftImage = LoadSoftImage("data/map.bmp");
		GetSoftImageSize(SoftImage, &XSize, &YSize);
		for (int Xvoxel = 0; Xvoxel < Voxel.GetReferenceCells().All; ++Xvoxel) {
			for (int Zvoxel = 0; Zvoxel < Voxel.GetReferenceCells().All; ++Zvoxel) {
				int xp = XSize * Xvoxel / (Voxel.GetReferenceCells().All - 1);
				int yp = YSize - YSize * Zvoxel / (Voxel.GetReferenceCells().All - 1);
				int r = 0;
				GetPixelSoftImage(SoftImage, xp, yp, &r, nullptr, nullptr, nullptr);
				if (r == 0) {
					for (int Yvoxel = 0; Yvoxel < Voxel.GetReferenceCells().All-52; ++Yvoxel) {
						Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, 1, false);
					}
				}
				else {
					for (int Yvoxel = 0; Yvoxel < Voxel.GetReferenceCells().All / 4 + Voxel.GetReferenceCells().All * 3 / 4 * r / 255; ++Yvoxel) {
						Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, 1, false);
					}
				}
			}
		}
		DeleteSoftImage(SoftImage);
	}
	for (int Xvoxel = 0; Xvoxel < Voxel.GetReferenceCells().All; ++Xvoxel) {
		for (int Yvoxel = 0; Yvoxel < Voxel.GetReferenceCells().All; ++Yvoxel) {
			for (int Zvoxel = 0; Zvoxel < Voxel.GetReferenceCells().All; ++Zvoxel) {
				if (Yvoxel >= Voxel.GetReferenceCells().All / 2) {
					if (Yvoxel % 20 == 0) {
						//Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, 2, false);
					}
				}
				else {
					Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, 1, false);
				}
			}
		}
	}
	Voxel.InitEnd();											// 初期化終了時処理
	// 自分の座標と向きを指定
	MyPos = VGet(0.f, 0.f, 0.f);

	int Lange = 10;
	int MX{}, MY{};
	int PMX{}, PMY{};
	int DMX{}, DMY{};
	// メインループ
	while (ProcessMessage() == 0) {
		PMX = MX;
		PMY = MY;
		GetMousePoint(&MX, &MY);
		//
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			float X = static_cast<float>(MY - PMY) / 100.f;
			float Y = static_cast<float>(MX - PMX) / 100.f;
			if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				X = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				Y = 0.f;
			}
			Xrad -= X;
			Yrad += Y;
		}
		if (CheckHitKey(KEY_INPUT_F1) != 0) {
			Xrad = -89.f * DX_PI_F / 180.f;
			Yrad = -90.f * DX_PI_F / 180.f;
		}
		if (CheckHitKey(KEY_INPUT_F2) != 0) {
			Xrad = -89.f * DX_PI_F / 180.f;
			Yrad = -0.f * DX_PI_F / 180.f;
		}
		if (CheckHitKey(KEY_INPUT_F3) != 0) {
			Xrad = -0.f * DX_PI_F / 180.f;
			Yrad = -0.f * DX_PI_F / 180.f;
		}

		Xrad = std::clamp(Xrad, -89.f * DX_PI_F / 180.f, 90.f * DX_PI_F / 180.f);
		MATRIX Mat = MMult(MGetRotAxis(VGet(1.f, 0.f, 0.f), Xrad), MGetRotAxis(VGet(0.f, 1.f, 0.f), Yrad));
		//
		Lange -= GetMouseWheelRotVol();
		Lange = std::clamp(Lange, 1, 20);
		//
		MyVec = VGet(0.f, 0.f, 0.f);
		if ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) {
			float X = static_cast<float>(MY - PMY) / 100.f;
			float Y = static_cast<float>(MX - PMX) / 100.f;
			if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				X = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				Y = 0.f;
			}
			MyVec = VAdd(MyVec, VTransform(VGet(Y * static_cast<float>(Lange) / 3.f, X * static_cast<float>(Lange) / 3.f, 0.f), Mat));
		}



		// 仮座標を反映
		MyPos = VAdd(MyPos, MyVec);
		// カメラ座標を指定
		VECTOR CamPos = VSub(MyPos, VTransform(VGet(0.f, 0.f, -50.f), Mat));
		VECTOR CamTarget = MyPos;// 自身が向いている方向を注視点とする
		// FPSを表示
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		// シャドウマップに描画する範囲を設定
		// ボクセル処理
		Voxel.SetDrawInfo(VGet(0.f, 0.f,0.f), VNorm(VSub(CamTarget, CamPos)));// 描画する際の描画中心座標と描画する向きを指定
		Voxel.SetShadowDrawInfo(VGet(0.f, 0.f, 0.f), DXParam.GetShadowVec());// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		Voxel.Update();
		// シャドウマップへの描画
		DXParam.SetShadowMapStart(CamPos);
		Voxel.DrawShadow();							// シャドウマップへの描画
		DXParam.SetShadowMapEnd();
		// 裏画面への描画
		SetDrawScreen(DX_SCREEN_BACK);				// 描画先を裏画面に変更
		ClearDrawScreen();											// 画面をクリア
		{
			SetCameraPositionAndTarget_UpVecY(CamPos, CamTarget);// カメラの位置と向きを設定
			SetCameraNearFar(0.5f, 100.0f);			// 描画する奥行き方向の範囲を設定
			SetupCamera_Ortho(2.f* static_cast<float>(Lange));

			DXParam.SetUseShadowMapStart();
			Voxel.Draw();							// 描画
			DXParam.SetUseShadowMapEnd();

			int Axis = 30;
			for (int loop = -Axis; loop <= Axis; ++loop) {
				DrawLine3D(VGet(static_cast<float>(-Axis), 0.f, static_cast<float>(loop)), VGet(static_cast<float>(Axis), 0.f, static_cast<float>(loop)), GetColor(255, 255, 255));
				DrawLine3D(VGet(static_cast<float>(loop), 0.f, static_cast<float>(-Axis)), VGet(static_cast<float>(loop), 0.f, static_cast<float>(Axis)), GetColor(255, 255, 255));
			}

			DrawLine3D(VGet(-100.f, 0.f, 0.f), VGet(0.f, 0.f, 0.f), GetColor(128, 0, 0));
			DrawLine3D(VGet(0.f, -100.f, 0.f), VGet(0.f, 0.f, 0.f), GetColor(0, 128, 0));
			DrawLine3D(VGet(0.f, 0.f, -100.f), VGet(0.f, 0.f, 0.f), GetColor(0, 0, 128));

			DrawLine3D(VGet(0.f, 0.f, 0.f), VGet(100.f, 0.f, 0.f), GetColor(255, 0, 0));
			DrawLine3D(VGet(0.f, 0.f, 0.f), VGet(0.f, 100.f, 0.f), GetColor(0, 255, 0));
			DrawLine3D(VGet(0.f, 0.f, 0.f), VGet(0.f, 0.f, 100.f), GetColor(0, 0, 255));
		}
		ScreenFlip();								// 裏画面の内容を表画面に反映
	}

	Voxel.SaveCellsFile("data/Map.txt");						// ボクセルデータの読み込み

	Voxel.Dispose();
	Voxel.Dispose_Load();
	return 0;					// ソフトの終了
}
