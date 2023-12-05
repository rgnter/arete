#include "arete/input/input.hpp"


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
	case InputKey::MOUSE_MOVE_X:
	case InputKey::MOUSE_MOVE_Y:
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



// Input Action - Input Action registered to devices callbacks

void InputAction<glm::vec3>::mapInput(std::vector<InputMapping<glm::vec3>> axisMappings)
{
    for (auto & axisMapping : axisMappings)
    {
        Input::InputDevice inputDevice;
        if (Input::tryGetInputDevice(getInputDeviceTypeFromKey(axisMapping.inputKey), 0, inputDevice))
        {
            inputDevice.registerListener(axisMapping.inputKey, [](float){});
        }
    }
      
}

void InputAction<bool>::mapInput(std::vector<InputMapping<bool>> buttonMappings)
{
    for (auto & buttonMapping : buttonMappings)
    {
        Input::InputDevice inputDevice;
        if (Input::tryGetInputDevice(getInputDeviceTypeFromKey(buttonMapping.inputKey), 0, inputDevice))
        {
            inputDevice.registerListener(buttonMapping.inputKey, [](float){});
        }
    } 
}



// Action Map - Mapping actions

template<typename T>
void ActionMap::mapInput(const std::string & actionName, std::vector<InputMapping<T>> inputMappings)
{
    InputAction<T> inputAction;
    if (getOrAddAction<T>(actionName, inputAction))
    {
        inputAction.mapInput(buttonMappings);
    }   
}

template<typename T>
void ActionMap::mapInput(const std::string & actionName, InputMapping<T> inputMapping)
{
    InputAction<T> & inputAction;
    if (getOrAddAction<T>(actionName, inputAction))
    {
        std::vector<InputMapping<T>> inputMappings;
        inputMappings.emplace_back(inputMapping);
        inputAction.mapInput(actionName, inputMappings);
    }
}


template<typename T>
bool ActionMap::getOrAddAction(const std::string & actionName, InputAction<T> & inputAction)
{
    return false;
}

template<>
bool ActionMap::getOrAddAction(const std::string & actionName, InputAction<bool> & inputAction)
{
    inputAction = _boolInputActions[actionName];
    return true;
}

template<>
bool ActionMap::getOrAddAction(const std::string & actionName, InputAction<glm::vec3> & inputAction)
{
    inputAction = _vectorInputActions[actionName];
    return true;
}


template<typename T>
bool ActionMap::getAction(const std::string & actionName, InputAction<T> & inputAction)
{
    return false;
}

template<>
bool ActionMap::getAction(const std::string & actionName, InputAction<bool> & inputAction)
{
    if (_boolInputActions.contains(actionName))
    {
        inputAction = _boolInputActions[actionName];
        return true;
    }
    return false;
}

template<>
bool ActionMap::getAction(const std::string & actionName, InputAction<glm::vec3> & inputAction)
{
    if (_vectorInputActions.contains(actionName))
    {
        inputAction = _vectorInputActions[actionName];
        return true;
    }
    return false;
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

Input::CallbackBankHandle Input::InputDevice::registerListener(
    InputKey inputKey,
    Callback callback)
{
    auto & listeners = _onStateChanged[inputKey];
    return listeners.emplace(listeners.end(), callback);
}

void Input::InputDevice::unregisterListener(
    InputKey inputKey,
    CallbackBankHandle handle)
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

        if (_currentState.contains(newInputKey) && _currentState[newInputKey].value == newValue)
        {
            continue;
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
    }
    _newStateBuffer.clear();
}



// Input - Handling input devices

Input::DeviceMap Input::_devices;

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
}


bool Input::tryGetInputDevice(InputDeviceType type, int index, InputDevice & inputDevice)
{
    if (containsDevice(type, index))
    {
        inputDevice = _devices[type][index];
        return true;
    }
    return false;
}

bool Input::containsDevice(InputDeviceType type, int index)
{
    return _devices.contains(type) && _devices[type].contains(index);
}


} // namespace arete::input