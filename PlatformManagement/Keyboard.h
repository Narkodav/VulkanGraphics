#pragma once
#include <map>

#include "Window.h"
#include "InputStateTracker.h"

enum class KeyboardKey
{	
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
	ESCAPE, LCONTROL, LSHIFT, LALT, LSUPER, RSUPER, MENU, LMB, RMB, MMB,
	ENTER, TAB, BACKSPACE, SPACE, CAPSLOCK, NUMLOCK, SCROLLLOCK, PAUSE, PRINTSCREEN,
	PAGEUP, PAGEDOWN, END, HOME, INSERT, DELETE, LEFT, RIGHT, UP, DOWN,
	NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
	ADD, SUBTRACT, MULTIPLY, DIVIDE, DECIMAL, NUMPAD_ENTER, NUMPAD_EQUALS,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	TILDA, MINUS, EQUALS, LBRACKET, RBRACKET, BACKSLASH, SEMICOLON, APOSTROPHE, COMMA, PERIOD, SLASH,
	UNKNOWN,
	NUM
};

enum class KeyboardKeyState
{
	PRESSED,
	REPEATED,
	CHANGED,
	NUM
};

class Keyboard : public InputStateTracker<KeyboardKey, static_cast<size_t>(KeyboardKey::NUM), 
	KeyboardKeyState, static_cast<size_t>(KeyboardKeyState::NUM)>
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
		{GLFW_KEY_ESCAPE, KeyboardKey::ESCAPE},
		{GLFW_KEY_LEFT_CONTROL, KeyboardKey::LCONTROL},
		{GLFW_KEY_LEFT_SHIFT, KeyboardKey::LSHIFT},
		{GLFW_KEY_LEFT_ALT, KeyboardKey::LALT},
		{GLFW_KEY_LEFT_SUPER, KeyboardKey::LSUPER},
		{GLFW_KEY_RIGHT_SUPER, KeyboardKey::RSUPER},
		{GLFW_KEY_MENU, KeyboardKey::MENU},
		{GLFW_MOUSE_BUTTON_LEFT, KeyboardKey::LMB},
		{GLFW_MOUSE_BUTTON_RIGHT, KeyboardKey::RMB},
		{GLFW_MOUSE_BUTTON_MIDDLE, KeyboardKey::MMB},

		// Control keys
		{GLFW_KEY_ENTER, KeyboardKey::ENTER},
		{GLFW_KEY_TAB, KeyboardKey::TAB},
		{GLFW_KEY_BACKSPACE, KeyboardKey::BACKSPACE},
		{GLFW_KEY_SPACE, KeyboardKey::SPACE},
		{GLFW_KEY_CAPS_LOCK, KeyboardKey::CAPSLOCK},
		{GLFW_KEY_NUM_LOCK, KeyboardKey::NUMLOCK},
		{GLFW_KEY_SCROLL_LOCK, KeyboardKey::SCROLLLOCK},
		{GLFW_KEY_PAUSE, KeyboardKey::PAUSE},
		{GLFW_KEY_PRINT_SCREEN, KeyboardKey::PRINTSCREEN},

		// Navigation keys
		{GLFW_KEY_PAGE_UP, KeyboardKey::PAGEUP},
		{GLFW_KEY_PAGE_DOWN, KeyboardKey::PAGEDOWN},
		{GLFW_KEY_END, KeyboardKey::END},
		{GLFW_KEY_HOME, KeyboardKey::HOME},
		{GLFW_KEY_INSERT, KeyboardKey::INSERT},
		{GLFW_KEY_DELETE, KeyboardKey::DELETE},
		{GLFW_KEY_LEFT, KeyboardKey::LEFT},
		{GLFW_KEY_RIGHT, KeyboardKey::RIGHT},
		{GLFW_KEY_UP, KeyboardKey::UP},
		{GLFW_KEY_DOWN, KeyboardKey::DOWN},

		// Numpad
		{GLFW_KEY_KP_0, KeyboardKey::NUMPAD_0},
		{GLFW_KEY_KP_1, KeyboardKey::NUMPAD_1},
		{GLFW_KEY_KP_2, KeyboardKey::NUMPAD_2},
		{GLFW_KEY_KP_3, KeyboardKey::NUMPAD_3},
		{GLFW_KEY_KP_4, KeyboardKey::NUMPAD_4},
		{GLFW_KEY_KP_5, KeyboardKey::NUMPAD_5},
		{GLFW_KEY_KP_6, KeyboardKey::NUMPAD_6},
		{GLFW_KEY_KP_7, KeyboardKey::NUMPAD_7},
		{GLFW_KEY_KP_8, KeyboardKey::NUMPAD_8},
		{GLFW_KEY_KP_9, KeyboardKey::NUMPAD_9},
		{GLFW_KEY_KP_ADD, KeyboardKey::ADD},
		{GLFW_KEY_KP_SUBTRACT, KeyboardKey::SUBTRACT},
		{GLFW_KEY_KP_MULTIPLY, KeyboardKey::MULTIPLY},
		{GLFW_KEY_KP_DIVIDE, KeyboardKey::DIVIDE},
		{GLFW_KEY_KP_DECIMAL, KeyboardKey::DECIMAL},
		{GLFW_KEY_KP_ENTER, KeyboardKey::NUMPAD_ENTER},
		{GLFW_KEY_KP_EQUAL, KeyboardKey::NUMPAD_EQUALS},

		// Function keys
		{GLFW_KEY_F1, KeyboardKey::F1}, {GLFW_KEY_F2, KeyboardKey::F2},
		{GLFW_KEY_F3, KeyboardKey::F3}, {GLFW_KEY_F4, KeyboardKey::F4},
		{GLFW_KEY_F5, KeyboardKey::F5}, {GLFW_KEY_F6, KeyboardKey::F6},
		{GLFW_KEY_F7, KeyboardKey::F7}, {GLFW_KEY_F8, KeyboardKey::F8},
		{GLFW_KEY_F9, KeyboardKey::F9}, {GLFW_KEY_F10, KeyboardKey::F10},
		{GLFW_KEY_F11, KeyboardKey::F11}, {GLFW_KEY_F12, KeyboardKey::F12},

		// Symbols
		{GLFW_KEY_GRAVE_ACCENT, KeyboardKey::TILDA},
		{GLFW_KEY_MINUS, KeyboardKey::MINUS},
		{GLFW_KEY_EQUAL, KeyboardKey::EQUALS},
		{GLFW_KEY_LEFT_BRACKET, KeyboardKey::LBRACKET},
		{GLFW_KEY_RIGHT_BRACKET, KeyboardKey::RBRACKET},
		{GLFW_KEY_BACKSLASH, KeyboardKey::BACKSLASH},
		{GLFW_KEY_SEMICOLON, KeyboardKey::SEMICOLON},
		{GLFW_KEY_APOSTROPHE, KeyboardKey::APOSTROPHE},
		{GLFW_KEY_COMMA, KeyboardKey::COMMA},
		{GLFW_KEY_PERIOD, KeyboardKey::PERIOD},
		{GLFW_KEY_SLASH, KeyboardKey::SLASH}
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
		m_keyPressedSub = window.registerCallback<IOEvents::KEY_PRESSED>([this]
		(int keyGlfw, int scancode, int mods) {
				auto key = glfwKeyConverter.find(keyGlfw);
				if (key == glfwKeyConverter.end())
					keyPressedCallback(KeyboardKey::UNKNOWN);
				keyPressedCallback(key->second);
			});
		m_keyReleasedSub = window.registerCallback<IOEvents::KEY_RELEASED>([this]
		(int keyGlfw, int scancode, int mods) {
				auto key = glfwKeyConverter.find(keyGlfw);
				if (key == glfwKeyConverter.end())
					keyReleasedCallback(KeyboardKey::UNKNOWN);
				keyReleasedCallback(key->second);
			});
		m_keyRepeatedSub = window.registerCallback<IOEvents::KEY_REPEATED>([this]
		(int keyGlfw, int scancode, int mods) {
				auto key = glfwKeyConverter.find(keyGlfw);
				if (key == glfwKeyConverter.end())
					keyRepeatedCallback(KeyboardKey::UNKNOWN);
				keyRepeatedCallback(key->second);
			});
	}

	bool keyStateChanged() const { return m_keyStateChanged; };

	// no need to add custom callback registration because it is handled by the window
	void refreshState()
	{
		m_keyStateChanged = false;
		for (int i = 0; i < static_cast<int>(KeyboardKey::NUM); ++i)
			setInputState<KeyboardKeyState::CHANGED>(static_cast<KeyboardKey>(i), false);
	}

	template<KeyboardKey E>
	bool keyJustPressed() const
	{
		return getInputState<E, KeyboardKeyState::PRESSED>() && getInputState<E, KeyboardKeyState::CHANGED>();
	}

	bool keyJustPressed(KeyboardKey key) const
	{
		return getInputState<KeyboardKeyState::PRESSED>(key) && getInputState<KeyboardKeyState::CHANGED>(key);
	}

	template<KeyboardKey E>
	bool keyJustReleased() const
	{
		return !getInputState<E, KeyboardKeyState::PRESSED>() && getInputState<E, KeyboardKeyState::CHANGED>();
	}

	bool keyJustReleased(KeyboardKey key) const
	{
		return !getInputState<KeyboardKeyState::PRESSED>(key) && getInputState<KeyboardKeyState::CHANGED>(key);
	}

	//callbacks can be triggered manually through the window event system or on platform level
private:
	void keyPressedCallback(KeyboardKey key)
	{
		setInputState<KeyboardKeyState::PRESSED>(key, true);
		setInputState<KeyboardKeyState::CHANGED>(key, true);
		m_keyStateChanged = true;
	}

	void keyReleasedCallback(KeyboardKey key)
	{
		setInputState<KeyboardKeyState::PRESSED>(key, false);
		setInputState<KeyboardKeyState::CHANGED>(key, true);
		m_keyStateChanged = true;
	}

	void keyRepeatedCallback(KeyboardKey key)
	{
		setInputState<KeyboardKeyState::REPEATED>(key, true);
		setInputState<KeyboardKeyState::CHANGED>(key, true);
		m_keyStateChanged = true;
	}
};
