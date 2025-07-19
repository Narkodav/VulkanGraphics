#pragma once

#pragma once
#include <map>

#include "Window.h"
#include "InputStateTracker.h"

enum class MouseButton
{
	Lmb,            // Left mouse button
	Rmb,            // Right mouse button
	Mmb,            // Middle mouse button
	Button4,       // Usually Browser Back button
	Button5,       // Usually Browser Forward button
	Button6,       // Additional buttons some mice have
	Button7,
	Button8,
	Unknown,
	Num
};

enum class MouseButtonState
{
	Pressed,
	Changed,
	Num
};

class Mouse : public InputStateTracker<MouseButton, static_cast<size_t>(MouseButton::Num),
	MouseButtonState, static_cast<size_t>(MouseButtonState::Num)>
{
private:
	static inline const std::map<int, MouseButton> glfwMouseConverter = {
		{GLFW_MOUSE_BUTTON_LEFT, MouseButton::Lmb},
		{GLFW_MOUSE_BUTTON_RIGHT, MouseButton::Rmb},
		{GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::Mmb},
		{GLFW_MOUSE_BUTTON_4, MouseButton::Button4},
		{GLFW_MOUSE_BUTTON_5, MouseButton::Button5},
		{GLFW_MOUSE_BUTTON_6, MouseButton::Button6},
		{GLFW_MOUSE_BUTTON_7, MouseButton::Button7},
		{GLFW_MOUSE_BUTTON_8, MouseButton::Button8}
	};

	MT::EventSystem<IOEventPolicy>::Subscription m_ButtonPressedSub;
	MT::EventSystem<IOEventPolicy>::Subscription m_ButtonReleasedSub;
	MT::EventSystem<IOEventPolicy>::Subscription m_scrollSub;
	MT::EventSystem<IOEventPolicy>::Subscription m_mouseMovedSub;

	glm::ivec2 m_mousePos = glm::ivec2(0.0);
	glm::ivec2 m_mouseDeltaPos = glm::ivec2(0.0);
	glm::dvec2 m_scrollOffsets = glm::dvec2(0.0);

	bool m_moved = false;
	bool m_scrolled = false;
	bool m_buttonStateChanged = false;

public:
	Mouse() = default;

	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	Mouse(Mouse&&) noexcept = default;
	Mouse& operator=(Mouse&&) noexcept = default;

	Mouse(Window& window)
	{
		init(window);
	}

	void init(Window& window)
	{
		m_ButtonPressedSub = window.registerCallback<IOEvents::MouseButtonPressed>([this]
		(int button, int mods) {
				auto Button = glfwMouseConverter.find(button);
				if (Button == glfwMouseConverter.end())
					ButtonPressedCallback(MouseButton::Unknown);
				ButtonPressedCallback(Button->second);
			});
		m_ButtonReleasedSub = window.registerCallback<IOEvents::MouseButtonReleased>([this]
		(int button, int mods) {
				auto Button = glfwMouseConverter.find(button);
				if (Button == glfwMouseConverter.end())
					ButtonReleasedCallback(MouseButton::Unknown);
				ButtonReleasedCallback(Button->second);
			});

		m_scrollSub = window.registerCallback<IOEvents::MouseScrolled>([this]
		(double xoffset, double yoffset) {
				scrollCallback(xoffset, yoffset);
			});
		m_mouseMovedSub = window.registerCallback<IOEvents::MouseMoved>([this]
		(double x, double y) {
				mouseMovedCallback(x, y);
			});

		m_mousePos = window.getCursorPos();
	}

	// no need to add custom callback registration because it is handled by the window
	void refreshState()
	{
		m_moved = false;
		m_scrolled = false;
		m_buttonStateChanged = false;
		m_scrollOffsets = glm::dvec2(0.0);
		m_mouseDeltaPos = glm::ivec2(0.0);
		for (int i = 0; i < static_cast<int>(MouseButton::Num); ++i)
			setInputState<MouseButtonState::Changed>(static_cast<MouseButton>(i), false);
	}

	template<MouseButton E>
	bool ButtonJustPressed() const
	{
		return getInputState<E, MouseButtonState::Pressed>() && getInputState<E, MouseButtonState::Changed>();
	}

	bool ButtonJustPressed(MouseButton Button) const
	{
		return getInputState<MouseButtonState::Pressed>(Button) && getInputState<MouseButtonState::Changed>(Button);
	}

	template<MouseButton E>
	bool ButtonJustReleased() const
	{
		return !getInputState<E, MouseButtonState::Pressed>() && getInputState<E, MouseButtonState::Changed>();
	}

	bool ButtonJustReleased(MouseButton Button) const
	{
		return !getInputState<MouseButtonState::Pressed>(Button) && getInputState<MouseButtonState::Changed>(Button);
	}

	const glm::ivec2& getMousePos() const { return m_mousePos; };
	const glm::ivec2& getMouseDeltaPos() const { return m_mouseDeltaPos; };
	const glm::dvec2& getScrollOffsets() const { return m_scrollOffsets; };

	bool moved() const { return m_moved; };

	//callbacks can be triggered manually through the window event system or on platform level
private:
	void ButtonPressedCallback(MouseButton Button)
	{
		m_buttonStateChanged = true;
		setInputState<MouseButtonState::Pressed>(Button, true);
		setInputState<MouseButtonState::Changed>(Button, true);
	}

	void ButtonReleasedCallback(MouseButton Button)
	{
		m_buttonStateChanged = true;
		setInputState<MouseButtonState::Pressed>(Button, false);
		setInputState<MouseButtonState::Changed>(Button, true);
	}

	void scrollCallback(double xoffset, double yoffset)
	{
		m_scrolled = true;
		m_scrollOffsets = glm::dvec2(xoffset, yoffset);
	}

	void mouseMovedCallback(double x, double y)
	{
		m_moved = true;
		auto newPos = glm::ivec2(x, y);
		m_mouseDeltaPos = newPos - m_mousePos;
		m_mousePos = newPos;
	}
};