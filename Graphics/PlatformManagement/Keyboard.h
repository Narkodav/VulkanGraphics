#pragma once
#include <map>

#include "Window.h"
#include "InputStateTracker.h"

enum class KeyboardKey
{	
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
	Escape, LControl, LShift, LAlt, LSuper, RSuper, Menu, Lmb, Rmb, Mmb,
	Enter, Tab, Backspace, Space, CapsLock, NumLock, ScrollLock, Pause, PrintScreen,
	PageUp, PageDown, End, Home, Insert, Delete, Left, Right, Up, Down,
	Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
	Add, Subtract, Multiply, Divide, Decimal, NumpadEnter, NumpadEquals,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Tilda, Minus, Equals, LBracket, RBracket, Backslash, Semicolon, Apostrophe, Comma, Period, Slash,
	Unknown,
	Num
};

enum class KeyboardKeyState
{
	Pressed,
	Repeated,
	Changed,
	Num
};

class Keyboard : public InputStateTracker<KeyboardKey, static_cast<size_t>(KeyboardKey::Num),
	KeyboardKeyState, static_cast<size_t>(KeyboardKeyState::Num)>
{
private:
	static inline const std::map<int, KeyboardKey> glfwKeyConverter = {
		// Letters
		{GLFW_KEY_A, KeyboardKey::A}, {GLFW_KEY_B, KeyboardKey::B}, {GLFW_KEY_C, KeyboardKey::C},
		{GLFW_KEY_D, KeyboardKey::D}, {GLFW_KEY_E, KeyboardKey::E}, {GLFW_KEY_F, KeyboardKey::F},
		{GLFW_KEY_G, KeyboardKey::G}, {GLFW_KEY_H, KeyboardKey::H}, {GLFW_KEY_I, KeyboardKey::I},
		{GLFW_KEY_J, KeyboardKey::J}, {GLFW_KEY_K, KeyboardKey::K}, {GLFW_KEY_L, KeyboardKey::L},
		{GLFW_KEY_M, KeyboardKey::M}, {GLFW_KEY_N, KeyboardKey::N}, {GLFW_KEY_O, KeyboardKey::O},
		{GLFW_KEY_P, KeyboardKey::P}, {GLFW_KEY_Q, KeyboardKey::Q}, {GLFW_KEY_R, KeyboardKey::R},
		{GLFW_KEY_S, KeyboardKey::S}, {GLFW_KEY_T, KeyboardKey::T}, {GLFW_KEY_U, KeyboardKey::U},
		{GLFW_KEY_V, KeyboardKey::V}, {GLFW_KEY_W, KeyboardKey::W}, {GLFW_KEY_X, KeyboardKey::X},
		{GLFW_KEY_Y, KeyboardKey::Y}, {GLFW_KEY_Z, KeyboardKey::Z},

		// Numbers
		{GLFW_KEY_1, KeyboardKey::_1}, {GLFW_KEY_2, KeyboardKey::_2}, {GLFW_KEY_3, KeyboardKey::_3},
		{GLFW_KEY_4, KeyboardKey::_4}, {GLFW_KEY_5, KeyboardKey::_5}, {GLFW_KEY_6, KeyboardKey::_6},
		{GLFW_KEY_7, KeyboardKey::_7}, {GLFW_KEY_8, KeyboardKey::_8}, {GLFW_KEY_9, KeyboardKey::_9},
		{GLFW_KEY_0, KeyboardKey::_0},

		// Special keys
		{GLFW_KEY_ESCAPE, KeyboardKey::Escape},
		{GLFW_KEY_LEFT_CONTROL, KeyboardKey::LControl},
		{GLFW_KEY_LEFT_SHIFT, KeyboardKey::LShift},
		{GLFW_KEY_LEFT_ALT, KeyboardKey::LAlt},
		{GLFW_KEY_LEFT_SUPER, KeyboardKey::LSuper},
		{GLFW_KEY_RIGHT_SUPER, KeyboardKey::RSuper},
		{GLFW_KEY_MENU, KeyboardKey::Menu},
		{GLFW_MOUSE_BUTTON_LEFT, KeyboardKey::Lmb},
		{GLFW_MOUSE_BUTTON_RIGHT, KeyboardKey::Rmb},
		{GLFW_MOUSE_BUTTON_MIDDLE, KeyboardKey::Mmb},

		// Control keys
		{GLFW_KEY_ENTER, KeyboardKey::Enter},
		{GLFW_KEY_TAB, KeyboardKey::Tab},
		{GLFW_KEY_BACKSPACE, KeyboardKey::Backspace},
		{GLFW_KEY_SPACE, KeyboardKey::Space},
		{GLFW_KEY_CAPS_LOCK, KeyboardKey::CapsLock},
		{GLFW_KEY_NUM_LOCK, KeyboardKey::NumLock},
		{GLFW_KEY_SCROLL_LOCK, KeyboardKey::ScrollLock},
		{GLFW_KEY_PAUSE, KeyboardKey::Pause},
		{GLFW_KEY_PRINT_SCREEN, KeyboardKey::PrintScreen},

		// Navigation keys
		{GLFW_KEY_PAGE_UP, KeyboardKey::PageUp},
		{GLFW_KEY_PAGE_DOWN, KeyboardKey::PageDown},
		{GLFW_KEY_END, KeyboardKey::End},
		{GLFW_KEY_HOME, KeyboardKey::Home},
		{GLFW_KEY_INSERT, KeyboardKey::Insert},
		{GLFW_KEY_DELETE, KeyboardKey::Delete},
		{GLFW_KEY_LEFT, KeyboardKey::Left},
		{GLFW_KEY_RIGHT, KeyboardKey::Right},
		{GLFW_KEY_UP, KeyboardKey::Up},
		{GLFW_KEY_DOWN, KeyboardKey::Down},

		// Numpad
		{GLFW_KEY_KP_0, KeyboardKey::Numpad0},
		{GLFW_KEY_KP_1, KeyboardKey::Numpad1},
		{GLFW_KEY_KP_2, KeyboardKey::Numpad2},
		{GLFW_KEY_KP_3, KeyboardKey::Numpad3},
		{GLFW_KEY_KP_4, KeyboardKey::Numpad4},
		{GLFW_KEY_KP_5, KeyboardKey::Numpad5},
		{GLFW_KEY_KP_6, KeyboardKey::Numpad6},
		{GLFW_KEY_KP_7, KeyboardKey::Numpad7},
		{GLFW_KEY_KP_8, KeyboardKey::Numpad8},
		{GLFW_KEY_KP_9, KeyboardKey::Numpad9},
		{GLFW_KEY_KP_ADD, KeyboardKey::Add},
		{GLFW_KEY_KP_SUBTRACT, KeyboardKey::Subtract},
		{GLFW_KEY_KP_MULTIPLY, KeyboardKey::Multiply},
		{GLFW_KEY_KP_DIVIDE, KeyboardKey::Divide},
		{GLFW_KEY_KP_DECIMAL, KeyboardKey::Decimal},
		{GLFW_KEY_KP_ENTER, KeyboardKey::NumpadEnter},
		{GLFW_KEY_KP_EQUAL, KeyboardKey::NumpadEquals},

		// Function keys
		{GLFW_KEY_F1, KeyboardKey::F1}, {GLFW_KEY_F2, KeyboardKey::F2},
		{GLFW_KEY_F3, KeyboardKey::F3}, {GLFW_KEY_F4, KeyboardKey::F4},
		{GLFW_KEY_F5, KeyboardKey::F5}, {GLFW_KEY_F6, KeyboardKey::F6},
		{GLFW_KEY_F7, KeyboardKey::F7}, {GLFW_KEY_F8, KeyboardKey::F8},
		{GLFW_KEY_F9, KeyboardKey::F9}, {GLFW_KEY_F10, KeyboardKey::F10},
		{GLFW_KEY_F11, KeyboardKey::F11}, {GLFW_KEY_F12, KeyboardKey::F12},

		// Symbols
		{GLFW_KEY_GRAVE_ACCENT, KeyboardKey::Tilda},
		{GLFW_KEY_MINUS, KeyboardKey::Minus},
		{GLFW_KEY_EQUAL, KeyboardKey::Equals},
		{GLFW_KEY_LEFT_BRACKET, KeyboardKey::LBracket},
		{GLFW_KEY_RIGHT_BRACKET, KeyboardKey::RBracket},
		{GLFW_KEY_BACKSLASH, KeyboardKey::Backslash},
		{GLFW_KEY_SEMICOLON, KeyboardKey::Semicolon},
		{GLFW_KEY_APOSTROPHE, KeyboardKey::Apostrophe},
		{GLFW_KEY_COMMA, KeyboardKey::Comma},
		{GLFW_KEY_PERIOD, KeyboardKey::Period},
		{GLFW_KEY_SLASH, KeyboardKey::Slash}
	};

	MT::EventSystem<IOEventPolicy>::Subscription m_keyPressedSub;
	MT::EventSystem<IOEventPolicy>::Subscription m_keyReleasedSub;
	MT::EventSystem<IOEventPolicy>::Subscription m_keyRepeatedSub;

	bool m_keyStateChanged = false;
public:

	Keyboard() = default;

	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	Keyboard(Keyboard&&) = delete;
	Keyboard& operator=(Keyboard&&) = delete;

	Keyboard(Window& window)
	{
		init(window);
	}

	void init(Window& window)
	{
		m_keyPressedSub = window.registerCallback<IOEvents::KeyPressed>([this]
		(int keyGlfw, int scancode, int mods) {
				auto key = glfwKeyConverter.find(keyGlfw);
				if (key == glfwKeyConverter.end())
					keyPressedCallback(KeyboardKey::Unknown);
				keyPressedCallback(key->second);
			});
		m_keyReleasedSub = window.registerCallback<IOEvents::KeyReleased>([this]
		(int keyGlfw, int scancode, int mods) {
				auto key = glfwKeyConverter.find(keyGlfw);
				if (key == glfwKeyConverter.end())
					keyReleasedCallback(KeyboardKey::Unknown);
				keyReleasedCallback(key->second);
			});
		m_keyRepeatedSub = window.registerCallback<IOEvents::KeyRepeated>([this]
		(int keyGlfw, int scancode, int mods) {
				auto key = glfwKeyConverter.find(keyGlfw);
				if (key == glfwKeyConverter.end())
					keyRepeatedCallback(KeyboardKey::Unknown);
				keyRepeatedCallback(key->second);
			});
	}

	bool keyStateChanged() const { return m_keyStateChanged; };

	// no need to add custom callback registration because it is handled by the window
	void refreshState()
	{
		m_keyStateChanged = false;
		for (int i = 0; i < static_cast<int>(KeyboardKey::Num); ++i)
			setInputState<KeyboardKeyState::Changed>(static_cast<KeyboardKey>(i), false);
	}

	template<KeyboardKey E>
	bool keyJustPressed() const
	{
		return getInputState<E, KeyboardKeyState::Pressed>() && getInputState<E, KeyboardKeyState::Changed>();
	}

	bool keyJustPressed(KeyboardKey key) const
	{
		return getInputState<KeyboardKeyState::Pressed>(key) && getInputState<KeyboardKeyState::Changed>(key);
	}

	template<KeyboardKey E>
	bool keyJustReleased() const
	{
		return !getInputState<E, KeyboardKeyState::Pressed>() && getInputState<E, KeyboardKeyState::Changed>();
	}

	bool keyJustReleased(KeyboardKey key) const
	{
		return !getInputState<KeyboardKeyState::Pressed>(key) && getInputState<KeyboardKeyState::Changed>(key);
	}

	//callbacks can be triggered manually through the window event system or on platform level
private:
	void keyPressedCallback(KeyboardKey key)
	{
		setInputState<KeyboardKeyState::Pressed>(key, true);
		setInputState<KeyboardKeyState::Changed>(key, true);
		m_keyStateChanged = true;
	}

	void keyReleasedCallback(KeyboardKey key)
	{
		setInputState<KeyboardKeyState::Pressed>(key, false);
		setInputState<KeyboardKeyState::Changed>(key, true);
		m_keyStateChanged = true;
	}

	void keyRepeatedCallback(KeyboardKey key)
	{
		setInputState<KeyboardKeyState::Repeated>(key, true);
		setInputState<KeyboardKeyState::Changed>(key, true);
		m_keyStateChanged = true;
	}
};
