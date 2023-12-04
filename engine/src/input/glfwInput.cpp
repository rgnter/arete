
#include "arete/vulkan.hpp"
#include "arete/input/glfwInput.hpp"

namespace arete::input
{

/**
 * \brief 
 * \param window 
 */
void GlfwInput::setup(GLFWwindow * window)
{
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* input = static_cast<GlfwInput*>(glfwGetWindowUserPointer(window));

		if (input)
		{
			float value = 0.f;

			switch (action) {
				case GLFW_PRESS:
				case GLFW_REPEAT:
					value = 1.f;
				break;
				default:
					value = 0.f;
			}

			input->updateKeyboardState(GlfwInput::keyToInputKey(key), value);
		}
	});

  
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
	{
		auto* input = static_cast<GlfwInput*>(glfwGetWindowUserPointer(window));

		if (input)
		{
			input->updateMouseState(mouseButtonToInputKey(button), action == GLFW_PRESS ? 1.f : 0.f);
		}
	});

	// glfwSetJoystickCallback([](int joystickId, int event) {
	// 	if (this) {
	// 		if (event == GLFW_CONNECTED /*&& glfwJoystickIsGamepad(joystickId)*/) {
	// 			this->registerDevice(
	// 				InputDevice{
	// 					.type = InputDeviceType::GAMEPAD,
	// 					.index = joystickId,
	// 					.stateFunc = std::bind(&GlfwInput::getGamepadStateByID, this, std::placeholders::_1)
	// 				}
	// 			);
	// 		}
	// 		else if (event == GLFW_DISCONNECTED) {
	// 			this->removeDevice(InputDeviceType::GAMEPAD, joystickId);
	// 		}
	// 	}}
	// );


	// register input devices
	registerDevice( InputDevice {
			.type = InputDeviceType::KEYBOARD,
			.index = 0,
			.stateFunc = std::bind(&GlfwInput::getKeyboardState, this, std::placeholders::_1)
		}
	);

	registerDevice( InputDevice {
			.type = InputDeviceType::MOUSE,
			.index = 0,
			.stateFunc = std::bind(&GlfwInput::getMouseState, this, std::placeholders::_1)
		}
	);

    // for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
	// 	if (glfwJoystickPresent(i)) {		
	// 		registerDevice(
	// 			InputDevice{
	// 				.type = InputDeviceType::GAMEPAD,
	// 				.index = i,
	// 				.stateFunc = std::bind(&GlfwInput::getGamepadStateByID, this, std::placeholders::_1)
	// 			}
	// 		);
	// 	}
	// }
}

void GlfwInput::processInput()
{
	glfwPollEvents();

	Input::processInput();
}

InputKey GlfwInput::keyToInputKey(int key)
{
	switch (key) {
        case GLFW_KEY_A: return InputKey::KEY_A;
        case GLFW_KEY_B: return InputKey::KEY_B;
        case GLFW_KEY_C: return InputKey::KEY_C;
        case GLFW_KEY_D: return InputKey::KEY_D;
        case GLFW_KEY_E: return InputKey::KEY_E;
        case GLFW_KEY_S: return InputKey::KEY_S;
        case GLFW_KEY_W: return InputKey::KEY_W;
        default: return InputKey::UNKNOWN;
	}
}

InputKey GlfwInput::mouseButtonToInputKey(int mouseButton)
{
	switch (mouseButton) {
        case GLFW_MOUSE_BUTTON_LEFT: return InputKey::MOUSE_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return InputKey::MOUSE_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return InputKey::MOUSE_MIDDLE;
        default: return InputKey::UNKNOWN;
	}
}

std::unordered_map<InputKey, InputDeviceState> GlfwInput::getGamepadStateByID (int joystickId)
{
	GLFWgamepadstate state;
	
	if (glfwGetGamepadState(joystickId, &state)) {
		return getGamepadState(state);
	}

	return std::unordered_map<InputKey, InputDeviceState>{};
}

std::unordered_map<InputKey, InputDeviceState> GlfwInput::getGamepadState(const GLFWgamepadstate& state) {
	std::unordered_map<InputKey, InputDeviceState> gamepadState{};

	for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
		int buttonState = state.buttons[i];
		float value = buttonState == GLFW_PRESS ? 1.f : 0.f;

		switch (i) {
			case GLFW_GAMEPAD_BUTTON_A:
				gamepadState[InputKey::GAMEPAD_A].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_B:
				gamepadState[InputKey::GAMEPAD_B].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_X:
				gamepadState[InputKey::GAMEPAD_X].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_Y:
				gamepadState[InputKey::GAMEPAD_Y].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:
				gamepadState[InputKey::GAMEPAD_BUMPER_L].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:
				gamepadState[InputKey::GAMEPAD_BUMPER_R].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_BACK:
				gamepadState[InputKey::GAMEPAD_SELECT].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_START:
				gamepadState[InputKey::GAMEPAD_START].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_LEFT_THUMB:
				gamepadState[InputKey::GAMEPAD_L3].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:
				gamepadState[InputKey::GAMEPAD_R3].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_DPAD_UP:
				gamepadState[InputKey::GAMEPAD_DPAD_UP].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
				gamepadState[InputKey::GAMEPAD_DPAD_RIGHT].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
				gamepadState[InputKey::GAMEPAD_DPAD_DOWN].value = value;
				break;

			case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
				gamepadState[InputKey::GAMEPAD_DPAD_LEFT].value = value;
				break;

			
			default:
			case GLFW_GAMEPAD_BUTTON_GUIDE:
				break;
		}
	}

	for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; i++) {
		float value = state.axes[i];

		switch (i) {
			case GLFW_GAMEPAD_AXIS_LEFT_X:
				gamepadState[InputKey::GAMEPAD_L_THUMB_X].value = value;
				break;
			case GLFW_GAMEPAD_AXIS_LEFT_Y:
				gamepadState[InputKey::GAMEPAD_L_THUMB_Y].value = value;
				break;
			case GLFW_GAMEPAD_AXIS_RIGHT_X:
				gamepadState[InputKey::GAMEPAD_R_THUMB_X].value = value;
				break;
			case GLFW_GAMEPAD_AXIS_RIGHT_Y:
				gamepadState[InputKey::GAMEPAD_R_THUMB_Y].value = value;
				break;
			case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:
				gamepadState[InputKey::GAMEPAD_L_TRIGGER].value = value;
				break;
			case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:
				gamepadState[InputKey::GAMEPAD_R_TRIGGER].value = value;
				break;
		}
	}

	return gamepadState;
}

} // namespace arete::input