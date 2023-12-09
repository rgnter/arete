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



// Input Mapping




// Input Action - Input Action registered to devices callbacks


// void InputAction<glm::vec3>::mapInput(std::vector<InputMapping<glm::vec3>> axisMappings)
// {
//     _inputMappings = axisMappings;
//     for (auto & inputMapping : _inputMappings)
//     {
//         Input::tryRegisterListenerOfDevice(
//             getInputDeviceTypeFromKey(inputMapping.inputKey), 0, inputMapping.inputKey, 
//             [&](float value) {
//                 if (value != 0)
//                 {
//                     _activeInputs++;

//                     if (!_started)
//                     {
//                         _value = glm::vec3(0);
//                         _value += inputMapping.axis * value;

//                         if (started)
//                         {
//                             CallbackContext ctx;
//                             started(ctx, _value);
//                         }
//                         _started = true;
//                     }
//                     else
//                     {
//                         _value += inputMapping.axis * value;
//                     }

//                     if (performed)
//                     {
//                         CallbackContext ctx;
//                         performed(ctx, _value);
//                     }
//                 }
//                 else
//                 {
//                     _activeInputs--;

//                     _value -= inputMapping.axis * value;

//                     if (!_started)
//                     {
//                         if (started)
//                         {
//                             CallbackContext ctx;
//                             started(ctx, _value);
//                         }
//                         _started = true;
//                     }

//                     if (performed)
//                     {
//                         CallbackContext ctx;
//                         performed(ctx, _value);
//                     }

//                     if (_activeInputs == 0)
//                     {
//                         _value = glm::vec3(0);
//                         if (canceled)
//                         {
//                             CallbackContext ctx;
//                             canceled(ctx, _value);
//                         }
//                         _started = false;
//                     }
//                 }
//             }
//         );
//     }
// }

// void InputAction<bool>::mapInput(std::vector<InputMapping<bool>> buttonMappings)
// {
//     for (auto & buttonMapping : buttonMappings)
//     {
//         Input::tryRegisterListenerOfDevice(
//             getInputDeviceTypeFromKey(buttonMapping.inputKey), 0, buttonMapping.inputKey,
//             [&](float value) {
//                 if (value == 1)
//                 {
//                     _value = true;

//                     if (!_started)
//                     {
//                         if (started)
//                         {
//                             CallbackContext startedCtx;
//                             started(startedCtx, true);
//                         }
//                         _started = true;
//                     }

//                     if (performed)
//                     {
//                         CallbackContext performedCtx;
//                         performed(performedCtx, true);
//                     }
//                 }
//                 else
//                 {
//                     if (!_started)
//                     {
//                         CallbackContext startedCtx;
//                         started(startedCtx, true);
//                         _started = true;
//                     }

//                     if (performed)
//                     {
//                         CallbackContext performedCtx;
//                         performed(performedCtx, true);
//                     }

//                     _value = false;
                    
//                     if (canceled)
//                     {
//                         CallbackContext canceledCtx;
//                         canceled(canceledCtx, false);
//                     }
//                     _started = false;
//                 }
//             }
//         );
//     } 
// }



// Action Map - Mapping actions

template<typename T>
InputAction<T> * ActionMap::createAction(const std::string & actionName)
{
    return nullptr;
}

template<>
InputAction<bool> * ActionMap::createAction(const std::string & actionName)
{
    return &_boolInputActions[actionName];
}

template<>
InputAction<glm::vec3> * ActionMap::createAction(const std::string & actionName)
{
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

        if (_currentState.contains(newInputKey))
        {
            if (_currentState[newInputKey].value == newValue)
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
                callback(newValue);
            }
        }
    }
    _newStateBuffer.clear();
}


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

void Input::activate(ActionMap inputActionMap)
{
    
}

Input::InputDevice * Input::getDevice(InputDeviceType type, int index)
{
    if (containsDevice(type, index))
    {
        return &_devices[type][index];
    }
    return nullptr;
}

bool Input::containsDevice(InputDeviceType type, int index)
{
    return _devices.contains(type) && _devices[type].contains(index);
}


} // namespace arete::input