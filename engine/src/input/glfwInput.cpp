#include "arete/vulkan.hpp"
#include "arete/input/glfwInput.hpp"

namespace arete::input
{

void GlfwInput::setup(GLFWwindow * window)
{
    // glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

		InputDevice inputDevice;
		if (Input::tryGetInputDevice(InputDeviceType::KEYBOARD, 0, inputDevice))
		{
			inputDevice.addToNewStateBuffer(GlfwInput::keyToInputKey(key), InputDeviceState(value));
		}
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
	{
        InputDevice inputDevice;
        if (Input::tryGetInputDevice(InputDeviceType::MOUSE, 0, inputDevice))
        {
            inputDevice.addToNewStateBuffer(
                GlfwInput::mouseButtonToInputKey(button),
                InputDeviceState(action == GLFW_PRESS ? 1.f : 0.f)
            );
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
	Input::registerDevice(Input::InputDevice(InputDeviceType::KEYBOARD, 0));
	Input::registerDevice(Input::InputDevice(InputDeviceType::MOUSE, 0));

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
        case GLFW_KEY_Q: return InputKey::KEY_Q;
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

} // namespace arete::input