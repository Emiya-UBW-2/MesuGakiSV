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

enum class EnumInput :size_t {
	Begin,
	Mouse_Begin = Begin,
	Mouse_Left = Mouse_Begin,
	Mouse_Right,
	Mouse_Middle,
	Mouse_4,
	Mouse_5,
	Mouse_6,
	Mouse_7,
	Mouse_8,
	Mouse_Max,

	Key_Begin = Mouse_Max,
	Key_BACK = Key_Begin,
	Key_TAB,
	Key_RETURN,
	Key_LSHIFT,
	Key_RSHIFT,
	Key_LCONTROL,
	Key_RCONTROL,
	Key_ESCAPE,
	Key_SPACE,
	Key_PGUP,
	Key_PGDN,
	Key_END,
	Key_HOME,
	Key_LEFT,
	Key_UP,
	Key_RIGHT,
	Key_DOWN,
	Key_INSERT,
	Key_DELETE,
	Key_MINUS,
	Key_YEN,
	Key_PREVTRACK,
	Key_PERIOD,
	Key_SLASH,
	Key_LALT,
	Key_RALT,
	Key_SCROLL,
	Key_SEMICOLON,
	Key_COLON,
	Key_LBRACKET,
	Key_RBRACKET,
	Key_AT,
	Key_BACKSLASH,
	Key_COMMA,
	Key_KANJI,
	Key_CONVERT,
	Key_NOCONVERT,
	Key_KANA,
	Key_APPS,
	Key_CAPSLOCK,
	Key_SYSRQ,
	Key_PAUSE,
	Key_LWIN,
	Key_RWIN,
	Key_NUMLOCK,
	Key_NUMPAD0,
	Key_NUMPAD1,
	Key_NUMPAD2,
	Key_NUMPAD3,
	Key_NUMPAD4,
	Key_NUMPAD5,
	Key_NUMPAD6,
	Key_NUMPAD7,
	Key_NUMPAD8,
	Key_NUMPAD9,
	Key_MULTIPLY,
	Key_ADD,
	Key_SUBTRACT,
	Key_DECIMAL,
	Key_DIVIDE,
	Key_NUMPADENTER,
	Key_F1,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,
	Key_A,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,
	Key_0,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,
	Key_Max,

	XInput_Begin = Key_Max,
	XInput_SQUARE = XInput_Begin,
	XInput_NG,
	XInput_OK,
	XInput_TRIANGLE,
	XInput_L1,
	XInput_R1,
	XInput_L2,
	XInput_R2,
	XInput_OPTION,
	XInput_L3,
	XInput_R3,
	XInput_LEFT,
	XInput_UP,
	XInput_RIGHT,
	XInput_DOWN,
	XInput_ESCAPE,

	XInput_LSTICKLEFT,
	XInput_LSTICKUP,
	XInput_LSTICKRIGHT,
	XInput_LSTICKDOWN,
	XInput_RSTICKLEFT,
	XInput_RSTICKUP,
	XInput_RSTICKRIGHT,
	XInput_RSTICKDOWN,
	XInput_Max,

	DInput_Begin = XInput_Max,
	DInput_SQUARE = DInput_Begin,
	DInput_NG,
	DInput_OK,
	DInput_TRIANGLE,
	DInput_L1,
	DInput_R1,
	DInput_L2,
	DInput_R2,
	DInput_OPTION,
	DInput_L3,
	DInput_R3,
	DInput_LEFT,
	DInput_UP,
	DInput_RIGHT,
	DInput_DOWN,
	DInput_ESCAPE,

	DInput_LSTICKLEFT,
	DInput_LSTICKUP,
	DInput_LSTICKRIGHT,
	DInput_LSTICKDOWN,
	DInput_RSTICKLEFT,
	DInput_RSTICKUP,
	DInput_RSTICKRIGHT,
	DInput_RSTICKDOWN,
	DInput_Max,

	Max = DInput_Max,
};

static const char* MouseInputStr[static_cast<size_t>(EnumInput::Mouse_Max) - static_cast<size_t>(EnumInput::Mouse_Begin)] = {
	"LM",
	"RM",
	"MM",
	"M4",
	"M5",
	"M6",
	"M7",
	"M8",
};

static const char* KeyInputStr[static_cast<size_t>(EnumInput::Key_Max) - static_cast<size_t>(EnumInput::Key_Begin)] = {
	"BackSpace",
	"Tab",
	"Enter",

	"Shift",
	"Shift",
	"Ctrl",
	"Ctrl",
	"ESC",
	"none",
	"PageUp",
	"PageDown",
	"End",
	"Home",
	"hidari",
	"ue",
	"migi",
	"sita",
	"Ins",
	"Del",

	"mainasu",
	"en",
	"quotation",
	"period",
	"slash",
	"Alt",
	"Alt",
	"ScrollLock",
	"semikoron",
	"coron",
	"kakko3",
	"kakko4",
	"atto",
	"backslash",
	"comma",
	"kanji",
	"Henkan",
	"Muhenkan",
	"kana",
	"F",
	"CapsLock",
	"PrintScreen",
	"Pause",
	"W",
	"W",

	"NumLock",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"asutarisku",
	"purasu",
	"mainasu",
	"period",
	"slash",
	"Enter",

	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",

	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",

};

static const char* XInputInputStr[static_cast<size_t>(EnumInput::XInput_Max) - static_cast<size_t>(EnumInput::XInput_Begin)] = {
	"square",
	"ng",
	"ok",
	"triangle",
	"L1",
	"R1",
	"L2",
	"R2",
	"option",
	"L_stick",
	"R_stick",
	"left",
	"up",
	"right",
	"down",
	"share",
	"L_stick",
	"L_stick",
	"L_stick",
	"L_stick",
	"R_stick",
	"R_stick",
	"R_stick",
	"R_stick",
};

static const char* DInputInputStr[static_cast<size_t>(EnumInput::DInput_Max) - static_cast<size_t>(EnumInput::DInput_Begin)] = {
	"square",
	"ng",
	"ok",
	"triangle",
	"L1",
	"R1",
	"L2",
	"R2",
	"option",
	"L_stick",
	"R_stick",
	"left",
	"up",
	"right",
	"down",
	"share",
	"L_stick",
	"L_stick",
	"L_stick",
	"L_stick",
	"R_stick",
	"R_stick",
	"R_stick",
	"R_stick",
};

static const char* GetKeyStr(EnumInput ID) noexcept {
	if (static_cast<size_t>(EnumInput::Mouse_Max) > static_cast<size_t>(ID)) {
		return MouseInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::Mouse_Begin)];
	}
	else if (static_cast<size_t>(EnumInput::Key_Max) > static_cast<size_t>(ID)) {
		return KeyInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::Key_Begin)];
	}
	else if (static_cast<size_t>(EnumInput::XInput_Max) > static_cast<size_t>(ID)) {
		return XInputInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::XInput_Begin)];
	}
	else if (static_cast<size_t>(EnumInput::DInput_Max) > static_cast<size_t>(ID)) {
		return DInputInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::DInput_Begin)];
	}
	return "NONE";
}


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto maincontrol = std::make_unique<DXDraw>();

	int LastSize = 512;

	int Screen = MakeScreen(LastSize, LastSize, TRUE);
	SetDrawScreen(Screen);
	ClearDrawScreen();
	{
		std::ofstream outputfile("OutputFont.psf");

		int x = 1, y = 1;
		int ysMax = 0;

		int num = 0;
		while(true){
			std::string Path = "key_glay/";
			std::string Name = "";

			Name += GetKeyStr(static_cast<EnumInput>(num));
			Path += Name;

			if (static_cast<size_t>(EnumInput::Mouse_Max) > static_cast<size_t>(num)) {
				Path += ".png";
			}
			else if (static_cast<size_t>(EnumInput::Key_Max) > static_cast<size_t>(num)) {
				Path += ".jpg";
			}
			else if (static_cast<size_t>(EnumInput::XInput_Max) > static_cast<size_t>(num)) {
				Path += ".png";
			}
			else if (static_cast<size_t>(EnumInput::DInput_Max) > static_cast<size_t>(num)) {
				Path += ".png";
			}

			int KeyGraph = LoadGraph(Path.c_str(), false);
			int xs, ys;
			GetGraphSize(KeyGraph, &xs, &ys);
			if (x + xs > LastSize) {
				x = 1;
				y += ysMax + 1;
				ysMax = 0;
			}
			else {
				ysMax = (ysMax > ys) ? ysMax : ys;
			}
			DrawGraph(x, y, KeyGraph, true);
			DeleteGraph(KeyGraph);

			outputfile << Name + "=" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(xs) + "," + std::to_string(ys) + "\n";
			x += xs + 1;

			++num;
			if (static_cast<size_t>(EnumInput::DInput_Max) <= static_cast<size_t>(num)) {
				break;
			}
		}

		outputfile.close();
		SaveDrawScreenToPNG(0, 0, LastSize, LastSize, "OutputFont.png");
	}
	//メインループ開始
	while ((ProcessMessage() == 0) && (CheckHitKey(KEY_INPUT_ESCAPE) == 0)) {
		//描画
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			DrawExtendGraph(0, 0, 720, 720, Screen, TRUE);
			//デバッグ表示
			clsDx();
			printfDx("FPS:%4.1f\n", GetFPS());

		}
		ScreenFlip();
	}
	return 0;// ソフトの終了 
}
