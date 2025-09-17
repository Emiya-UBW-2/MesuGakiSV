#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "Util.hpp"
#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

enum class InputType {
	KeyBoard,
	XInput,
	DInput,
};

enum class EnumInput {
	Mouse_Begin,
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

static int MouseInput[static_cast<size_t>(EnumInput::Mouse_Max) - static_cast<size_t>(EnumInput::Mouse_Begin)] = {
	MOUSE_INPUT_1,
	MOUSE_INPUT_2,
	MOUSE_INPUT_3,
	MOUSE_INPUT_4,
	MOUSE_INPUT_5,
	MOUSE_INPUT_6,
	MOUSE_INPUT_7,
	MOUSE_INPUT_8,
};

static int KeyInput[static_cast<size_t>(EnumInput::Key_Max) - static_cast<size_t>(EnumInput::Key_Begin)] = {
	KEY_INPUT_BACK,
	KEY_INPUT_TAB,
	KEY_INPUT_RETURN,

	KEY_INPUT_LSHIFT,
	KEY_INPUT_RSHIFT,
	KEY_INPUT_LCONTROL,
	KEY_INPUT_RCONTROL,
	KEY_INPUT_ESCAPE,
	KEY_INPUT_SPACE,
	KEY_INPUT_PGUP,
	KEY_INPUT_PGDN,
	KEY_INPUT_END,
	KEY_INPUT_HOME,
	KEY_INPUT_LEFT,
	KEY_INPUT_UP,
	KEY_INPUT_RIGHT,
	KEY_INPUT_DOWN,
	KEY_INPUT_INSERT,
	KEY_INPUT_DELETE,

	KEY_INPUT_MINUS,
	KEY_INPUT_YEN,
	KEY_INPUT_PREVTRACK,
	KEY_INPUT_PERIOD,
	KEY_INPUT_SLASH,
	KEY_INPUT_LALT,
	KEY_INPUT_RALT,
	KEY_INPUT_SCROLL,
	KEY_INPUT_SEMICOLON,
	KEY_INPUT_COLON,
	KEY_INPUT_LBRACKET,
	KEY_INPUT_RBRACKET,
	KEY_INPUT_AT,
	KEY_INPUT_BACKSLASH,
	KEY_INPUT_COMMA,
	KEY_INPUT_KANJI,
	KEY_INPUT_CONVERT,
	KEY_INPUT_NOCONVERT,
	KEY_INPUT_KANA,
	KEY_INPUT_APPS,
	KEY_INPUT_CAPSLOCK,
	KEY_INPUT_SYSRQ,
	KEY_INPUT_PAUSE,
	KEY_INPUT_LWIN,
	KEY_INPUT_RWIN,

	KEY_INPUT_NUMLOCK,
	KEY_INPUT_NUMPAD0,
	KEY_INPUT_NUMPAD1,
	KEY_INPUT_NUMPAD2,
	KEY_INPUT_NUMPAD3,
	KEY_INPUT_NUMPAD4,
	KEY_INPUT_NUMPAD5,
	KEY_INPUT_NUMPAD6,
	KEY_INPUT_NUMPAD7,
	KEY_INPUT_NUMPAD8,
	KEY_INPUT_NUMPAD9,
	KEY_INPUT_MULTIPLY,
	KEY_INPUT_ADD,
	KEY_INPUT_SUBTRACT,
	KEY_INPUT_DECIMAL,
	KEY_INPUT_DIVIDE,
	KEY_INPUT_NUMPADENTER,

	KEY_INPUT_F1,
	KEY_INPUT_F2,
	KEY_INPUT_F3,
	KEY_INPUT_F4,
	KEY_INPUT_F5,
	KEY_INPUT_F6,
	KEY_INPUT_F7,
	KEY_INPUT_F8,
	KEY_INPUT_F9,
	KEY_INPUT_F10,
	KEY_INPUT_F11,
	KEY_INPUT_F12,

	KEY_INPUT_A,
	KEY_INPUT_B,
	KEY_INPUT_C,
	KEY_INPUT_D,
	KEY_INPUT_E,
	KEY_INPUT_F,
	KEY_INPUT_G,
	KEY_INPUT_H,
	KEY_INPUT_I,
	KEY_INPUT_J,
	KEY_INPUT_K,
	KEY_INPUT_L,
	KEY_INPUT_M,
	KEY_INPUT_N,
	KEY_INPUT_O,
	KEY_INPUT_P,
	KEY_INPUT_Q,
	KEY_INPUT_R,
	KEY_INPUT_S,
	KEY_INPUT_T,
	KEY_INPUT_U,
	KEY_INPUT_V,
	KEY_INPUT_W,
	KEY_INPUT_X,
	KEY_INPUT_Y,
	KEY_INPUT_Z,

	KEY_INPUT_0 ,
	KEY_INPUT_1,
	KEY_INPUT_2,
	KEY_INPUT_3,
	KEY_INPUT_4,
	KEY_INPUT_5,
	KEY_INPUT_6,
	KEY_INPUT_7,
	KEY_INPUT_8,
	KEY_INPUT_9,

};

static int XInputInput[static_cast<size_t>(EnumInput::XInput_Max) - static_cast<size_t>(EnumInput::XInput_Begin)] = {
	XINPUT_BUTTON_X,			//□
	XINPUT_BUTTON_A,		 	//×
	XINPUT_BUTTON_B,		 	//〇
	XINPUT_BUTTON_Y,     		//△
	XINPUT_BUTTON_LEFT_SHOULDER,
	XINPUT_BUTTON_RIGHT_SHOULDER,
	0xF100, 	//L2
	0xF200, 	//R2
	XINPUT_BUTTON_START,
	XINPUT_BUTTON_LEFT_THUMB,
	XINPUT_BUTTON_RIGHT_THUMB,
	XINPUT_BUTTON_DPAD_LEFT,
	XINPUT_BUTTON_DPAD_UP,
	XINPUT_BUTTON_DPAD_RIGHT,
	XINPUT_BUTTON_DPAD_DOWN,
	XINPUT_BUTTON_BACK,
	0xF001, 	//LSTICK
	0xF002, 	//LSTICK
	0xF004, 	//LSTICK
	0xF008, 	//LSTICK
	0xF011, 	//RSTICK
	0xF012, 	//RSTICK
	0xF014, 	//RSTICK
	0xF018, 	//RSTICK
};

static int DInputInput[static_cast<size_t>(EnumInput::DInput_Max) - static_cast<size_t>(EnumInput::DInput_Begin)] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	9,
	10,
	11,
	0xF001,
	0xF002,
	0xF004,
	0xF008,
	8,
	0xF011,
	0xF012,
	0xF014,
	0xF018,
	0xF021,
	0xF022,
	0xF024,
	0xF028,
};

enum class EnumMenu {
	Esc,
	Diside,
	Cancel,
	LEFT,
	UP,
	RIGHT,
	DOWN,
	Max,
};

enum class EnumBattle {
	A,
	W,
	D,
	S,
	Max,
};

class KeyParam : public SingletonBase<KeyParam> {
private:
	friend class SingletonBase<KeyParam>;
private:
	class Key {
		int m_PressTimer{ 0 };
		bool m_Press{ false };
		bool m_PrevPress{ false };
		bool m_IsRepeat{ false };
	public:
		void Update(bool PressButton) noexcept {
			m_PrevPress = m_Press;
			m_Press = PressButton;
			if (Press()) {
				m_PressTimer++;
				m_IsRepeat = Trigger();
				if (m_PressTimer > 6) {
					m_PressTimer -= 6;
					m_IsRepeat = true;
				}
			}
			else {
				m_PressTimer = -30;
				m_IsRepeat = false;
			}
		}
	public:
		bool Press(void) const noexcept { return m_Press; }
		bool Trigger(void) const noexcept { return m_Press && !m_PrevPress; }
		bool Repeat(void) const noexcept { return m_IsRepeat; }
		bool Release(void) const noexcept { return !m_Press; }
		bool ReleaseTrigger(void) const noexcept { return !m_Press && m_PrevPress; }
	};
private:
	std::vector<Key> m_Input{};
	std::vector<std::pair<Key*, Key*>> m_MenuKey{};
	std::vector<std::pair<Key*, Key*>> m_BattleKey{};
	InputType	m_InputType{ InputType::XInput };
private:
	//コンストラクタ
	KeyParam(void) noexcept {
		m_Input.resize(static_cast<size_t>(EnumInput::Max));
		m_MenuKey.resize(static_cast<size_t>(EnumMenu::Max));
		m_BattleKey.resize(static_cast<size_t>(EnumBattle::Max));
		Assign(InputType::KeyBoard);
	}
	KeyParam(const KeyParam&) = delete;
	KeyParam(KeyParam&&) = delete;
	KeyParam& operator=(const KeyParam&) = delete;
	KeyParam& operator=(KeyParam&&) = delete;
	//デストラクタ
	~KeyParam(void) noexcept {
		m_Input.clear();
		m_MenuKey.clear();
		m_BattleKey.clear();
	}
private:
	void Assign(InputType type) noexcept {
		if (m_InputType != type) {
			m_InputType = type;
			switch (m_InputType) {
			case InputType::KeyBoard:
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Esc)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_ESCAPE));
				//
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Diside)).first = &m_Input.at(static_cast<size_t>(EnumInput::Mouse_Left));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Diside)).second = &m_Input.at(static_cast<size_t>(EnumInput::Key_F));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Cancel)).first = &m_Input.at(static_cast<size_t>(EnumInput::Mouse_Right));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Cancel)).second = &m_Input.at(static_cast<size_t>(EnumInput::Key_R));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::LEFT)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_A));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::UP)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_W));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::RIGHT)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_D));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::DOWN)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_S));
				//
				m_BattleKey.at(static_cast<size_t>(EnumBattle::A)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_A));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::W)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_W));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::D)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_D));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::S)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_S));
				break;
			case InputType::XInput:
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Esc)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_ESCAPE));
				//
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Diside)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_OK));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Cancel)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_NG));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::LEFT)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LEFT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::LEFT)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKLEFT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::UP)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_UP));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::UP)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKUP));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::RIGHT)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_RIGHT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::RIGHT)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKRIGHT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::DOWN)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_DOWN));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::DOWN)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKDOWN));
				//
				m_BattleKey.at(static_cast<size_t>(EnumBattle::A)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LEFT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::A)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKLEFT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::W)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_UP));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::W)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKUP));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::D)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_RIGHT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::D)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKRIGHT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::S)).first = &m_Input.at(static_cast<size_t>(EnumInput::XInput_DOWN));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::S)).second = &m_Input.at(static_cast<size_t>(EnumInput::XInput_LSTICKDOWN));
				break;
			case InputType::DInput:
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Esc)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_ESCAPE));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Esc)).first = &m_Input.at(static_cast<size_t>(EnumInput::Key_ESCAPE));
				//
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Diside)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_OK));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::Cancel)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_NG));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::LEFT)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LEFT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::LEFT)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKLEFT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::UP)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_UP));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::UP)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKUP));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::RIGHT)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_RIGHT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::RIGHT)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKRIGHT));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::DOWN)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_DOWN));
				m_MenuKey.at(static_cast<size_t>(EnumMenu::DOWN)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKDOWN));
				//
				m_BattleKey.at(static_cast<size_t>(EnumBattle::A)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LEFT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::A)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKLEFT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::W)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_UP));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::W)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKUP));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::D)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_RIGHT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::D)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKRIGHT));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::S)).first = &m_Input.at(static_cast<size_t>(EnumInput::DInput_DOWN));
				m_BattleKey.at(static_cast<size_t>(EnumBattle::S)).second = &m_Input.at(static_cast<size_t>(EnumInput::DInput_LSTICKDOWN));
				break;
			default:
				break;
			}
		}
	}
public:
	//入力取得
	bool GetMenuKeyTrigger(EnumMenu Select) const noexcept {
		auto& m = m_MenuKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Trigger()) || (m.second && m.second->Trigger());
	}
	bool GetMenuKeyPress(EnumMenu Select) const noexcept {
		auto& m = m_MenuKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Press()) || (m.second && m.second->Press());
	}
	bool GetMenuKeyRepeat(EnumMenu Select) const noexcept {
		auto& m = m_MenuKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Repeat()) || (m.second && m.second->Repeat());
	}
	bool GetMenuKeyRelease(EnumMenu Select) const noexcept {
		auto& m = m_MenuKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Release()) || (m.second && m.second->Release());
	}
	bool GetMenuKeyReleaseTrigger(EnumMenu Select) const noexcept {
		auto& m = m_MenuKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->ReleaseTrigger()) || (m.second && m.second->ReleaseTrigger());
	}
	bool GetBattleKeyTrigger(EnumBattle Select) const noexcept {
		auto& m = m_BattleKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Trigger()) || (m.second && m.second->Trigger());
	}
	bool GetBattleKeyPress(EnumBattle Select) const noexcept {
		auto& m = m_BattleKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Press()) || (m.second && m.second->Press());
	}
	bool GetBattleKeyRepeat(EnumBattle Select) const noexcept {
		auto& m = m_BattleKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Repeat()) || (m.second && m.second->Repeat());
	}
	bool GetBattleKeyRelease(EnumBattle Select) const noexcept {
		auto& m = m_BattleKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->Release()) || (m.second && m.second->Release());
	}
	bool GetBattleKeyReleaseTrigger(EnumBattle Select) const noexcept {
		auto& m = m_BattleKey.at(static_cast<size_t>(Select));
		return (m.first && m.first->ReleaseTrigger()) || (m.second && m.second->ReleaseTrigger());
	}
	//最後に入力したデバイス
	InputType GetLastInputDevice(void) const noexcept { return m_InputType; }
public:
	//毎フレーム更新する内容
	void Update(void) noexcept {
		//最後に入力したデバイスに更新
		if ((CheckHitKeyAll(DX_CHECKINPUT_MOUSE) != 0) || (CheckHitKeyAll(DX_CHECKINPUT_KEY) != 0)) {
			Assign(InputType::KeyBoard);
		}
		if (CheckHitKeyAll(DX_CHECKINPUT_PAD) != 0) {
			switch (GetJoypadType(DX_INPUT_PAD1)) {
			case DX_PADTYPE_XBOX_360:			// Xbox360コントローラー
			case DX_PADTYPE_XBOX_ONE:			// XboxOneコントローラー
				Assign(InputType::XInput);
				break;
			case DX_PADTYPE_DUAL_SHOCK_4:		// PS4コントローラー
			case DX_PADTYPE_DUAL_SENSE:			// PS5コントローラー
			case DX_PADTYPE_SWITCH_JOY_CON_L:	// Switch Joycon(左)
			case DX_PADTYPE_SWITCH_JOY_CON_R:	// Switch Joycon(右)
			case DX_PADTYPE_SWITCH_PRO_CTRL:	// Switch Proコントローラー
			case DX_PADTYPE_OTHER:				// その他のコントローラー
				Assign(InputType::DInput);
				break;
			default:
				break;
			}
		}
		//マウス
		for (size_t loop = static_cast<size_t>(EnumInput::Mouse_Begin); loop < static_cast<size_t>(EnumInput::Mouse_Max); ++loop) {
			size_t index = loop - static_cast<size_t>(EnumInput::Mouse_Begin);
			m_Input.at(static_cast<size_t>(loop)).Update(GetWindowActiveFlag() && (GetMouseInput() & MouseInput[index]) != 0);
		}
		//キー
		for (size_t loop = static_cast<size_t>(EnumInput::Key_Begin); loop < static_cast<size_t>(EnumInput::Key_Max); ++loop) {
			size_t index = loop - static_cast<size_t>(EnumInput::Key_Begin);
			m_Input.at(static_cast<size_t>(loop)).Update(GetWindowActiveFlag() && (CheckHitKey(KeyInput[index]) != 0));
		}
		//XInput
		{
			XINPUT_STATE input;
			GetJoypadXInputState(DX_INPUT_PAD1, &input);
			bool LS_Active = false;
			bool RS_Active = false;
			float LS_Degree = 0.f;
			float RS_Degree = 0.f;
			{
				int LS_X = input.ThumbLX;
				int LS_Y = -input.ThumbLY;
				//XInputが-1000~1000になるように変換
				LS_X = 1000 * LS_X / 32768;
				LS_Y = 1000 * LS_Y / 32768;
				//変換により誤差が発生しうるのでデッドゾーンを指定
				if (abs(LS_X) < 10) { LS_X = 0; }
				if (abs(LS_Y) < 10) { LS_Y = 0; }
				if (!(LS_X == 0 && LS_Y == 0)) {
					LS_Active = true;
					LS_Degree = rad2deg(std::atan2f(static_cast<float>(LS_X), static_cast<float>(-LS_Y)));
				}
			}
			{
				int RS_X = input.ThumbRX;
				int RS_Y = -input.ThumbRY;
				//XInputが-1000~1000になるように変換
				RS_X = 1000 * RS_X / 32768;
				RS_Y = 1000 * RS_Y / 32768;
				//変換により誤差が発生しうるのでデッドゾーンを指定
				if (abs(RS_X) < 10) { RS_X = 0; }
				if (abs(RS_Y) < 10) { RS_Y = 0; }
				if (!(RS_X == 0 && RS_Y == 0)) {
					RS_Active = true;
					RS_Degree = rad2deg(std::atan2f(static_cast<float>(RS_X), static_cast<float>(-RS_Y)));
				}
			}
			for (size_t loop = static_cast<size_t>(EnumInput::XInput_Begin); loop < static_cast<size_t>(EnumInput::XInput_Max); ++loop) {
				size_t index = loop - static_cast<size_t>(EnumInput::XInput_Begin);
				auto ID = XInputInput[index];
				bool IsPress = false;
				if (ID >= 0xF100) {
					switch (ID) {
					case 0xF100:
						IsPress = input.LeftTrigger > 0;
						break;
					case 0xF200:
						IsPress = input.RightTrigger > 0;
						break;
					case 0xF001:
						if (LS_Active) {
							IsPress = (-140.f <= LS_Degree && LS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF002:
						if (LS_Active) {
							IsPress = (-50.f <= LS_Degree && LS_Degree <= 50.f);//UP
						}
						break;
					case 0xF004:
						if (LS_Active) {
							IsPress = (40.f <= LS_Degree && LS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF008:
						if (LS_Active) {
							IsPress = (130.f <= LS_Degree || LS_Degree <= -130.f);//DOWN
						}
						break;
					case 0xF011:
						if (RS_Active) {
							IsPress = (-140.f <= RS_Degree && RS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF012:
						if (RS_Active) {
							IsPress = (-50.f <= RS_Degree && RS_Degree <= 50.f);//UP
						}
						break;
					case 0xF014:
						if (RS_Active) {
							IsPress = (40.f <= RS_Degree && RS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF018:
						if (RS_Active) {
							IsPress = (130.f <= RS_Degree || RS_Degree <= -130.f);//DOWN
						}
						break;
					default:
						break;
					}
				}
				else if (0 <= ID && ID < 16) {
					IsPress = (input.Buttons[ID] != 0);
				}

				m_Input.at(static_cast<size_t>(loop)).Update(GetWindowActiveFlag() && IsPress);
			}
		}
		//DInput
		{
			DINPUT_JOYSTATE input;
			GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
			bool LPOV_Active = false;
			bool LS_Active = false;
			bool RS_Active = false;
			float LPOV_Degree = 0.f;
			float LS_Degree = 0.f;
			float RS_Degree = 0.f;
			{
				int LS_X = input.X;
				int LS_Y = input.Y;
				//XInputが-1000~1000になるように変換
				LS_X = 1000 * LS_X / 32768;
				LS_Y = 1000 * LS_Y / 32768;
				//変換により誤差が発生しうるのでデッドゾーンを指定
				if (abs(LS_X) < 10) { LS_X = 0; }
				if (abs(LS_Y) < 10) { LS_Y = 0; }
				if (!(LS_X == 0 && LS_Y == 0)) {
					LS_Active = true;
					LS_Degree = rad2deg(std::atan2f(static_cast<float>(LS_X), static_cast<float>(-LS_Y)));
				}
			}
			{
				int RS_X = input.Z;
				int RS_Y = input.Rz;
				//XInputが-1000~1000になるように変換
				RS_X = 1000 * RS_X / 32768;
				RS_Y = 1000 * RS_Y / 32768;
				//変換により誤差が発生しうるのでデッドゾーンを指定
				if (abs(RS_X) < 10) { RS_X = 0; }
				if (abs(RS_Y) < 10) { RS_Y = 0; }
				if (!(RS_X == 0 && RS_Y == 0)) {
					RS_Active = true;
					RS_Degree = rad2deg(std::atan2f(static_cast<float>(RS_X), static_cast<float>(-RS_Y)));
				}
			}
			{
				if (input.POV[0] != 0xffffffff) {
					LPOV_Active = true;
					LPOV_Degree = static_cast<float>(input.POV[0]) / 100.f;
				}
			}
			for (size_t loop = static_cast<size_t>(EnumInput::DInput_Begin); loop < static_cast<size_t>(EnumInput::DInput_Max); ++loop) {
				size_t index = loop - static_cast<size_t>(EnumInput::DInput_Begin);
				auto ID = DInputInput[index];
				bool IsPress = false;
				if (ID >= 0xF000) {
					switch (ID) {
					case 0xF001:
						if (LPOV_Active) {
							IsPress = (220.f <= LPOV_Degree && LPOV_Degree <= 320.f);
						}
						break;
					case 0xF002:
						if (LPOV_Active) {
							IsPress = (310.f <= LPOV_Degree || LPOV_Degree <= 50.f);
						}
						break;
					case 0xF004:
						if (LPOV_Active) {
							IsPress = (40.f <= LPOV_Degree && LPOV_Degree <= 140.f);
						}
						break;
					case 0xF008:
						if (LPOV_Active) {
							IsPress = (130.f <= LPOV_Degree && LPOV_Degree <= 230.f);
						}
						break;
					case 0xF011:
						if (LS_Active) {
							IsPress = (-140.f <= LS_Degree && LS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF012:
						if (LS_Active) {
							IsPress = (-50.f <= LS_Degree && LS_Degree <= 50.f);//UP
						}
						break;
					case 0xF014:
						if (LS_Active) {
							IsPress = (40.f <= LS_Degree && LS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF018:
						if (LS_Active) {
							IsPress = (130.f <= LS_Degree || LS_Degree <= -130.f);//DOWN
						}
						break;
					case 0xF021:
						if (RS_Active) {
							IsPress = (-140.f <= RS_Degree && RS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF022:
						if (RS_Active) {
							IsPress = (-50.f <= RS_Degree && RS_Degree <= 50.f);//UP
						}
						break;
					case 0xF024:
						if (RS_Active) {
							IsPress = (40.f <= RS_Degree && RS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF028:
						if (RS_Active) {
							IsPress = (130.f <= RS_Degree || RS_Degree <= -130.f);//DOWN
						}
						break;
					default:
						break;
					}
				}
				else if (0 <= ID && ID < 16) {
					IsPress = (input.Buttons[ID] != 0);
				}

				m_Input.at(static_cast<size_t>(loop)).Update(GetWindowActiveFlag() && IsPress);
			}
		}
	}
};
