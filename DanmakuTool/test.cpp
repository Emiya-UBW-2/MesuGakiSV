#include "DxLib.h"

#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

#pragma once
#include <fstream>
#include <string>
#include <filesystem>

#define USE_DXLIB (false)

#if USE_DXLIB
#define NOMINMAX
#include "DxLib.h"
#endif

namespace File {
	//ファイル読み込み
	class InputFileStream {
#if USE_DXLIB
		int mdata{ InvalidID };
#else
		std::ifstream stream{};
#endif
	public:
		//コンストラクタ
		InputFileStream(void) noexcept {}
		//宣言時にファイルオープン版
		InputFileStream(std::string_view FilePath) noexcept { Open(FilePath); }

		InputFileStream(const InputFileStream&) = delete;
		InputFileStream(InputFileStream&&) = delete;
		InputFileStream& operator=(const InputFileStream&) = delete;
		InputFileStream& operator=(InputFileStream&&) = delete;

		//デストラクタ
		~InputFileStream(void) noexcept { Close(); }
	public:
		//ファイルを開き、探索ポイントを始点に移動
		void Open(std::string_view FilePath) noexcept {
#if USE_DXLIB
			mdata = DxLib::FileRead_open(FilePath, FALSE);
#else
			stream.open(FilePath);
#endif
		}
		// 1行そのまま取得し、次の行に探索ポイントを移る
		std::string SeekLineAndGetStr(void) noexcept {
#if USE_DXLIB
			const int charLength = 512;
			char mstr[charLength] = "";
			DxLib::FileRead_gets(mstr, charLength, mdata);
			return std::string(mstr);
#else
			std::string Buffer;
			std::getline(stream, Buffer);
			return Buffer;
#endif
		}
		// 探索ポイントが終端(EOF)で終わる
		bool ComeEof(void) const noexcept {
#if USE_DXLIB
			return DxLib::FileRead_eof(mdata) != 0;
#else
			return stream.eof();
#endif
		}
		//　閉じる
		void Close(void) noexcept {
#if USE_DXLIB
			if (mdata != InvalidID) {
				DxLib::FileRead_close(mdata);
				mdata = InvalidID;
			}
#else
			stream.close();
#endif
		}
	public:
		// 文字列から=より右の値取得
		static std::string getleft(const std::string& tempname, std::string_view DivWord) noexcept { return tempname.substr(0, tempname.find(DivWord)); }
		// 文字列から=より右の値取得
		static std::string getright(const std::string& tempname, std::string_view DivWord) noexcept { return tempname.substr(tempname.find(DivWord) + 1); }
	};

	//ファイル出力
	class OutputFileStream {
		std::ofstream stream{};
	public:
		//コンストラクタ
		OutputFileStream(void) noexcept {}
		//宣言時にファイルオープン版
		OutputFileStream(std::string_view FilePath) noexcept { Open(FilePath); }

		OutputFileStream(const OutputFileStream&) = delete;
		OutputFileStream(OutputFileStream&&) = delete;
		OutputFileStream& operator=(const OutputFileStream&) = delete;
		OutputFileStream& operator=(OutputFileStream&&) = delete;

		//デストラクタ
		~OutputFileStream(void) noexcept { Close(); }
	public:
		//ファイルを開き、探索ポイントを始点に移動
		void Open(std::string_view FilePath) noexcept {
			stream.open(FilePath);
		}
		// 1行書き込む
		void AddLine(const std::string& Str) noexcept {
			stream << Str + "\n";
		}
		//　閉じる
		void Close(void) noexcept {
			stream.close();
		}
	};
}
class DX {
	int ShadowMapHandle = -1;
	VECTOR ShadowVec = VGet(0.1f, -0.3f, -0.1f);
public:
	DX() {
		DxLib::SetGraphMode(640*2, 480, 32);
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
	// 初期化
	DX DXParam;

	int MS = 0;
	int Graph = LoadGraph("data/movie.mp4");
	PlayMovieToGraph(Graph);

	MS = 90000;
	SeekMovieToGraph(Graph, MS);

	int MX{}, MY{};
	int PMX{}, PMY{};
	int LMX{}, LMY{};
	bool PrevPressSpace = false;

	bool PrevPressLMouse = false;

	bool PrevPressLEFT = false;
	bool PrevPressRIGHT = false;

	double Speed = 1.0;
	File::OutputFileStream Ostream;
	Ostream.Open("Path.txt");
	// メインループ
	while (ProcessMessage() == 0) {
		PMX = MX;
		PMY = MY;
		GetMousePoint(&MX, &MY);
		LMX = (MX-(640 + 32))* 864 /384;
		LMY = (MY - 16) * 864 / 384;
		//
		{
			bool Press = ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
			if (Press && !PrevPressLMouse) {
				if (Speed == 0.0) {
					Ostream.AddLine("Time(" + std::to_string(MS / 30) + ")");
					Ostream.AddLine("	ToPoint(" + std::to_string(LMX) + "," + std::to_string(LMY) + ")");
				}
			}
			PrevPressLMouse = Press;
		}
		{
			bool Press = (CheckHitKey(KEY_INPUT_LEFT) != 0);
			if (Press && !PrevPressLEFT) {
				if (Speed == 0.0) {
					if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
						MS -= 1000 / 60 * 30;
					}
					else {
						MS -= 1000 / 60;
					}
					if (MS < 0) { MS = 0; }
					SeekMovieToGraph(Graph, MS);
				}
			}
			PrevPressLEFT = Press;
		}
		{
			bool Press = (CheckHitKey(KEY_INPUT_RIGHT) != 0);
			if (Press && !PrevPressRIGHT) {
				if (Speed == 0.0) {
					if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
						MS += 1000 / 60 * 30;
					}
					else {
						MS += 1000 / 60;
					}
					SeekMovieToGraph(Graph, MS);
				}
			}
			PrevPressRIGHT = Press;
		}
		{
			bool Press = (CheckHitKey(KEY_INPUT_SPACE) != 0);
			if (Press && !PrevPressSpace) {
				if (Speed > 0.0) {
					Speed = 0.0;
				}
				else {
					Speed = 1.0;
				}
				SetPlaySpeedRateMovieToGraph(Graph, Speed);
			}
			PrevPressSpace = Press;
		}
		if (Speed > 0.0) {
			MS = TellMovieToGraph(Graph);
		}

		// FPSを表示
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		printfDx("%5d ms\n", MS);
		printfDx("(%5d,%5d)\n", LMX, LMY);
		printfDx("←→ 1F移動　LSHIFTで30Fとばし\n");
		printfDx("SPACE再生/停止\n");
		// シャドウマップに描画する範囲を設定
		// 裏画面への描画
		SetDrawScreen(DX_SCREEN_BACK);				// 描画先を裏画面に変更
		ClearDrawScreen();											// 画面をクリア
		{
			DrawGraph(640, 0, Graph, true);

			DrawLine(640 + 32, 16,MX,MY, GetColor(0, 255, 0), TRUE);
		}
		ScreenFlip();								// 裏画面の内容を表画面に反映
	}
	Ostream.Close();					//プロット
	return 0;					// ソフトの終了
}
