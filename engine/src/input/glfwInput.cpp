#include "arete/vulkan.hpp"
#include "arete/input/glfwInput.hpp"

#include <cstdio>
namespace arete::input
{

void GlfwInput::bind(GLFWwindow * window)
{
    glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (auto * input = static_cast<GlfwInput*>(glfwGetWindowUserPointer(window)))
		{
			if (auto * device = input->getDevice(InputDeviceType::KEYBOARD, 0))
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

				device->addToNewStateBuffer(GlfwInput::keyToInputKey(key), InputDeviceState(value));
			}
		}
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		if (auto * input = static_cast<GlfwInput*>(glfwGetWindowUserPointer(window)))
		{
			if (auto * device = input->getDevice(InputDeviceType::MOUSE, 0))
			{
				device->addToNewStateBuffer(
					GlfwInput::mouseButtonToInputKey(button),
					InputDeviceState(action == GLFW_PRESS ? 1.f : 0.f)
				);
			}
		}
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
		if (auto * input = static_cast<GlfwInput*>(glfwGetWindowUserPointer(window)))
		{
			input->_mouseX = static_cast<int>(xPos);
			input->_mouseY = static_cast<int>(yPos);

			if (auto * device = input->getDevice(InputDeviceType::MOUSE, 0))
			{
				device->addToNewStateBuffer(
					InputKey::MOUSE_POS_X,
					InputDeviceState(input->_mouseX)
				);
				device->addToNewStateBuffer(
					InputKey::MOUSE_POS_Y,
					InputDeviceState(input->_mouseY)
				);
			}
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
	registerDevice(InputDevice(InputDeviceType::KEYBOARD, 0));
	registerDevice(InputDevice(InputDeviceType::MOUSE, 0));

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

	boundWindow = window;
}


void GlfwInput::processInput()
{
	glfwPollEvents();

	Input::processInput();
}

void GlfwInput::setMouseMode(MouseMode mouseMode)
{
	switch (mouseMode)
	{
		case MouseMode::NORMAL:
			glfwSetInputMode(boundWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;

		case MouseMode::HIDDEN:
			glfwSetInputMode(boundWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;

		case MouseMode::DISABLED:
			glfwSetInputMode(boundWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;	
	}
}


InputKey GlfwInput::keyToInputKey(int key)
{
	switch (key) {
        case GLFW_KEY_A: return InputKey::KEYBOARD_A;
        case GLFW_KEY_B: return InputKey::KEYBOARD_B;
        case GLFW_KEY_C: return InputKey::KEYBOARD_C;
        case GLFW_KEY_D: return InputKey::KEYBOARD_D;
        case GLFW_KEY_E: return InputKey::KEYBOARD_E;
        case GLFW_KEY_S: return InputKey::KEYBOARD_S;
        case GLFW_KEY_W: return InputKey::KEYBOARD_W;
        case GLFW_KEY_Q: return InputKey::KEYBOARD_Q;
        default: return InputKey::UNKNOWN;
	}
}

InputKey GlfwInput::mouseButtonToInputKey(int mouseButton)
{
	switch (mouseButton) {
        case GLFW_MOUSE_BUTTON_LEFT: return InputKey::MOUSE_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return InputKey::MOUSE_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return InputKey::MOUSE_MIDDLE;
        case GLFW_RAW_MOUSE_MOTION: return InputKey::MOUSE_MIDDLE;
        default: return InputKey::UNKNOWN;
	}
}

} // namespace arete::input