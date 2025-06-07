#pragma once

#pragma once
#include <map>

#include "Window.h"
#include "InputStateTracker.h"

enum class MouseButton
{
	LMB,            // Left mouse button
	RMB,            // Right mouse button
	MMB,            // Middle mouse button
	BUTTON_4,       // Usually Browser Back button
	BUTTON_5,       // Usually Browser Forward button
	BUTTON_6,       // Additional buttons some mice have
	BUTTON_7,
	BUTTON_8,
	UNKNOWN,
	NUM
};

enum class MouseButtonState
{
	PRESSED,
	CHANGED,
	NUM
};

class Mouse : public InputStateTracker<MouseButton, static_cast<size_t>(MouseButton::NUM),
	MouseButtonState, static_cast<size_t>(MouseButtonState::NUM)>
{
private:
	static inline const std::map<int, MouseButton> glfwMouseConverter = {
		{GLFW_MOUSE_BUTTON_LEFT, MouseButton::LMB},
		{GLFW_MOUSE_BUTTON_RIGHT, MouseButton::RMB},
		{GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::MMB},
		{GLFW_MOUSE_BUTTON_4, MouseButton::BUTTON_4},
		{GLFW_MOUSE_BUTTON_5, MouseButton::BUTTON_5},
		{GLFW_MOUSE_BUTTON_6, MouseButton::BUTTON_6},
		{GLFW_MOUSE_BUTTON_7, MouseButton::BUTTON_7},
		{GLFW_MOUSE_BUTTON_8, MouseButton::BUTTON_8}
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
		m_ButtonPressedSub = window.registerCallback<IOEvents::MOUSE_BUTTON_PRESSED>([this]
		(int button, int mods) {
				auto Button = glfwMouseConverter.find(button);
				if (Button == glfwMouseConverter.end())
					ButtonPressedCallback(MouseButton::UNKNOWN);
				ButtonPressedCallback(Button->second);
			});
		m_ButtonReleasedSub = window.registerCallback<IOEvents::MOUSE_BUTTON_RELEASED>([this]
		(int button, int mods) {
				auto Button = glfwMouseConverter.find(button);
				if (Button == glfwMouseConverter.end())
					ButtonReleasedCallback(MouseButton::UNKNOWN);
				ButtonReleasedCallback(Button->second);
			});

		m_scrollSub = window.registerCallback<IOEvents::MOUSE_SCROLLED>([this]
		(double xoffset, double yoffset) {
				scrollCallback(xoffset, yoffset);
			});
		m_mouseMovedSub = window.registerCallback<IOEvents::MOUSE_MOVED>([this]
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
		for (int i = 0; i < static_cast<int>(MouseButton::NUM); ++i)
			setInputState<MouseButtonState::CHANGED>(static_cast<MouseButton>(i), false);
	}

	template<MouseButton E>
	bool ButtonJustPressed() const
	{
		return getInputState<E, MouseButtonState::PRESSED>() && getInputState<E, MouseButtonState::CHANGED>();
	}

	bool ButtonJustPressed(MouseButton Button) const
	{
		return getInputState<MouseButtonState::PRESSED>(Button) && getInputState<MouseButtonState::CHANGED>(Button);
	}

	template<MouseButton E>
	bool ButtonJustReleased() const
	{
		return !getInputState<E, MouseButtonState::PRESSED>() && getInputState<E, MouseButtonState::CHANGED>();
	}

	bool ButtonJustReleased(MouseButton Button) const
	{
		return !getInputState<MouseButtonState::PRESSED>(Button) && getInputState<MouseButtonState::CHANGED>(Button);
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
		setInputState<MouseButtonState::PRESSED>(Button, true);
		setInputState<MouseButtonState::CHANGED>(Button, true);
	}

	void ButtonReleasedCallback(MouseButton Button)
	{
		m_buttonStateChanged = true;
		setInputState<MouseButtonState::PRESSED>(Button, false);
		setInputState<MouseButtonState::CHANGED>(Button, true);
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
