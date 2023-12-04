#include "arete/input/input.hpp"
#include <iostream>

namespace arete::input
{

InputSource getInputSourceFromKey(InputKey key)
{
	switch (key) {
	case InputKey::KEY_A:
	case InputKey::KEY_B:
	case InputKey::KEY_C:
	case InputKey::KEY_D:
	case InputKey::KEY_E:
	case InputKey::KEY_S:
	case InputKey::KEY_W:
		return InputSource::KEYBOARD;
	case InputKey::MOUSE_POS_X:
	case InputKey::MOUSE_POS_Y:
	case InputKey::MOUSE_MOVE_X:
	case InputKey::MOUSE_MOVE_Y:
	case InputKey::MOUSE_LEFT:
	case InputKey::MOUSE_RIGHT:
	case InputKey::MOUSE_MIDDLE:
		return InputSource::MOUSE;
	case InputKey::GAMEPAD_L_THUMB_X:
	case InputKey::GAMEPAD_L_THUMB_Y:
	case InputKey::GAMEPAD_R_THUMB_X:
	case InputKey::GAMEPAD_R_THUMB_Y:
	case InputKey::GAMEPAD_A:
	case InputKey::GAMEPAD_B:
	case InputKey::GAMEPAD_X:
	case InputKey::GAMEPAD_Y:
	case InputKey::GAMEPAD_START:
	case InputKey::GAMEPAD_SELECT:
	case InputKey::GAMEPAD_BUMPER_R:
	case InputKey::GAMEPAD_BUMPER_L:
	case InputKey::GAMEPAD_L3:
	case InputKey::GAMEPAD_R3:
	case InputKey::GAMEPAD_DPAD_UP:
	case InputKey::GAMEPAD_DPAD_LEFT:
	case InputKey::GAMEPAD_DPAD_DOWN:
	case InputKey::GAMEPAD_DPAD_RIGHT:
		return InputSource::GAMEPAD;
	default:
		return InputSource::UNKNOWN;
	}
}

// Action mapping
void Input::registerDevice(const InputDevice& device)
{
	std::cout << "Device registered of type: " << static_cast<int>(device.type) << std::endl;
	_devices.emplace_back(device);
}
void Input::removeDevice(InputDeviceType type, int index)
{
	erase_if(_devices, [type, index](const InputDevice& device) {
			return device.type == type && device.index == index;
		}
	);
	std::cout << "Removed device of type: " << static_cast<int>(type) << std::endl;
}

void Input::registerActionCallback(const std::string& actionName, const ActionCallback& callback)
{
	_actionCallbacks[actionName].emplace_back(callback);
}
void Input::removeActionCallback(const std::string& actionName, const std::string& callbackRef)
{
	erase_if(_actionCallbacks[actionName], [callbackRef](const ActionCallback& callback) {
			return callback.ref == callbackRef;
		}
	);
}

void Input::mapInputToAction(InputKey key, const InputAction& action)
{
	_inputActionMapping[key].emplace_back(action);
}
void Input::unmapInputFromAction(InputKey key, const std::string& actionName)
{
	erase_if(_inputActionMapping[key], [actionName](InputAction inputAction) {
		return inputAction.actionName == actionName;
	});
}

// Update event system and devices
void Input::processInput()
{
	std::vector<ActionEvent> events{};
	for (auto& device : _devices) {
		// get new state for device
		auto newState = device.stateFunc(device.index);

		// compare to old state for device
		for (auto& keyState : newState) {
			if (device.currentState[keyState.first].value != keyState.second.value) {
				// generate device action event
				auto generatedEvents = generateActionEvent(device.index, keyState.first, keyState.second.value);
				events.insert(events.end(), generatedEvents.begin(), generatedEvents.end());

				// save new state value
				device.currentState[keyState.first].value = keyState.second.value;
			}
		}
	}

	// propagate action events
	for (auto& event : events) {
		propagateActionEvent(event);
	}
}
std::vector<Input::ActionEvent> Input::generateActionEvent(int deviceIndex, InputKey key, float newVal)
{
	std::vector<ActionEvent> actionEvents{};
	auto& actions = _inputActionMapping[key];
	InputSource source = getInputSourceFromKey(key);

	for (auto& action : actions) {
		actionEvents.emplace_back(ActionEvent{
			.actionName = action.actionName,
			.source = source,
			.sourceIndex = deviceIndex,
			.value = newVal * action.scale,
			});

	}

	return actionEvents;
}
void Input::propagateActionEvent(ActionEvent event)
{
	auto& actionCallback = _actionCallbacks[event.actionName];
	for (size_t i = actionCallback.size() - 1; i >= 0; i--)
		if (actionCallback[i].func(event.source, event.sourceIndex, event.value)) break;
}

void Input::updateKeyboardState(InputKey key, float value)
{
	_keyboardState[key].value = value;
}
void Input::updateMouseState(InputKey button, float value)
{
	_mouseState[button].value = value;
}

} // namespace arete::input