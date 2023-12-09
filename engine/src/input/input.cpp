#include "arete/input/input.hpp"
#include <iostream>

namespace arete::input {

InputDeviceType getInputDeviceTypeFromKey(InputKey key)
{
	switch (key) {
	case InputKey::KEY_A:
	case InputKey::KEY_B:
	case InputKey::KEY_C:
	case InputKey::KEY_D:
	case InputKey::KEY_E:
	case InputKey::KEY_S:
	case InputKey::KEY_W:
		return InputDeviceType::KEYBOARD;
	case InputKey::MOUSE_POS_X:
	case InputKey::MOUSE_POS_Y:
	case InputKey::MOUSE_LEFT:
	case InputKey::MOUSE_RIGHT:
	case InputKey::MOUSE_MIDDLE:
		return InputDeviceType::MOUSE;
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
		return InputDeviceType::GAMEPAD;
	default:
		return InputDeviceType::UNKNOWN;
	}
}


// Input Device - Device state and callbacks

Input::InputDevice::InputDevice()
{
	type = InputDeviceType::UNKNOWN;
	index = -1;
}

Input::InputDevice::InputDevice(InputDeviceType Type, int Index)
{
	type = Type;
	index = Index;
}

Input::DeviceCallbackBankHandle Input::InputDevice::registerListener(
	InputKey inputKey,
	DeviceCallback callback)
{
	auto & listeners = _onStateChanged[inputKey];
	return listeners.emplace(listeners.end(), callback);
}

void Input::InputDevice::unregisterListener(
	InputKey inputKey,
	DeviceCallbackBankHandle handle)
{
	_onStateChanged[inputKey].erase(handle);
}

void Input::InputDevice::addToNewStateBuffer(InputKey key, InputDeviceState state)
{
	_newStateBuffer.emplace_back(key, state);
}

void Input::InputDevice::pushAndCapture(DeviceStateMap & capture)
{
	for (const auto & entry : _newStateBuffer)
	{
		InputKey newInputKey = entry.first;
		float newValue = entry.second.value;
		float oldValue = 0;

		if (_currentState.contains(newInputKey))
		{
			oldValue = _currentState[newInputKey].value;
			if (oldValue == newValue)
			{
				continue;
			}
		}

		capture.emplace(newInputKey, newValue);
		if (newValue != 0)
		{
			_currentState[newInputKey].value = newValue;
		}
		else
		{
			_currentState.erase(newInputKey);
		}

		if (_onStateChanged.contains(newInputKey))
		{
			for (const auto & callback : _onStateChanged[newInputKey])
			{
				callback(newValue, oldValue);
			}
		}   
	}
	_newStateBuffer.clear();
}



// Input - Input device management

void Input::registerDevice(const InputDevice& device)
{
	_devices[device.type].emplace(device.index, device);
}

void Input::removeDevice(InputDeviceType type, int index)
{
	if (containsDevice(type, index))
	{
		_devices[type].erase(index);
		if (_devices[type].size() == 0)
		{
			_devices.erase(type);
		}
	}

	// erase_if(_devices, [type, index](const InputDevice& device) {
	// 		return device.type == type && device.index == index;
	// 	}
	// );
}

Input::InputDevice * Input::getDevice(InputDeviceType type, int index)
{
	if (containsDevice(type, index))
	{
		return &_devices[type][index];
	}
	return nullptr;
}

void Input::addOnProcessListener(OnProcessCallback callback)
{
	_onProcessInput.push_back(std::move(callback));
}

void Input::removeOnProcessListener(OnProcessCallback callback)
{
	_onProcessInput.erase(std::remove_if(
		_onProcessInput.begin(),
		_onProcessInput.end(),
		[&](std::function<void(void)>& function) {
			// Check if the two functions have the same type
			if (typeid(function) != typeid(callback))
			{
				return false;
			}

			// Check if the two functions refer to the same method
			if ((function.target_type() != callback.target_type())
				|| (function.target<void(void)>() != callback.target<void(void)>()))
			{
				return false;
			}

			return true;
		}
	));
}

void Input::processInput()
{
	std::unordered_map<InputKey, InputDeviceState> inputChangeCapture;

	for (auto & kvp : _devices)
	{
		for (auto & device : kvp.second)
		{
			device.second.pushAndCapture(inputChangeCapture);
		}
	}

	for (auto & callback : _onProcessInput)
	{
		callback();
	}
}

bool Input::containsDevice(InputDeviceType type, int index)
{
	return _devices.contains(type) && _devices[type].contains(index);
}



// Action Map - Mapping actions

template<typename T>
InputAction<T> * ActionMap::createAction(const std::string & actionName)
{
	return nullptr;
}

template<typename T>
InputAction<T> * ActionMap::createAction(const std::string & actionName, Composite composite)
{
	return nullptr;
}

template<>
InputAction<bool> * ActionMap::createAction(const std::string & actionName)
{
	_boolInputActions.emplace(actionName, InputAction<bool>(actionName));
	return &_boolInputActions[actionName];
}

template<>
InputAction<glm::vec3> * ActionMap::createAction(const std::string & actionName, Composite composite)
{
	_vectorInputActions.emplace(actionName, InputAction<glm::vec3>(actionName, composite));
	return &_vectorInputActions[actionName];
}


template<typename T>
InputAction<T> * ActionMap::getAction(const std::string & actionName)
{
	return nullptr;
}

template<>
InputAction<bool>* ActionMap::getAction(const std::string & actionName)
{
	if (_boolInputActions.contains(actionName))
	{
		return & _boolInputActions[actionName];
	}
	return nullptr;
}

template<>
InputAction<glm::vec3> * ActionMap::getAction(const std::string & actionName)
{
	if (_vectorInputActions.contains(actionName))
	{
		return & _vectorInputActions[actionName];
	}
	return nullptr;
}

void ActionMap::bind(Input & input)
{
	for (auto & inputAction : _boolInputActions)
	{
		inputAction.second.bind(input);
	}

	for (auto & inputAction : _vectorInputActions)
	{
		inputAction.second.bind(input);
	}
}

} // namespace arete::input