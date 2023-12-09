#pragma once

#include <vector>
#include <functional>
#include <string>

#include <glm/vec3.hpp>

namespace arete::input {

// Common data

enum class InputKey {
	UNKNOWN,

	// KEYBOARD
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_S,
	KEY_W,
	KEY_Q,

	// MOUSE
	MOUSE_POS_X,
	MOUSE_POS_Y,
	MOUSE_MOVE_X,
	MOUSE_MOVE_Y,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,

	//GAMEPAD
	GAMEPAD_L_THUMB_X,
	GAMEPAD_L_THUMB_Y,
	GAMEPAD_R_THUMB_X,
	GAMEPAD_R_THUMB_Y,
	GAMEPAD_L_TRIGGER,
	GAMEPAD_R_TRIGGER,
	GAMEPAD_A,
	GAMEPAD_B,
	GAMEPAD_X,
	GAMEPAD_Y,
	GAMEPAD_START,
	GAMEPAD_SELECT,
	GAMEPAD_BUMPER_R,
	GAMEPAD_BUMPER_L,
	GAMEPAD_L3,
	GAMEPAD_R3,
	GAMEPAD_DPAD_UP,
	GAMEPAD_DPAD_LEFT,
	GAMEPAD_DPAD_DOWN,
	GAMEPAD_DPAD_RIGHT,
};

enum class InputDeviceType {
    UNKNOWN,
    
	KEYBOARD,
	MOUSE,
	GAMEPAD
};



// Input Mapping

template<typename T>
struct InputMapping;

template<>
struct InputMapping<bool>
{
    // void receiveFromDevice(float value, bool & actionValue, int & actionActiveMappings);

    InputKey inputKey;
};

template<>
struct InputMapping<glm::vec3>
{

    // void receiveFromDevice(float value, glm::vec3 & actionValue, int & actionActiveMappings);

    InputKey inputKey;
    glm::vec3 axis;
};



// ActionMap / InputAction

template<typename T>
class InputAction
{

public:
    struct CallbackContext {

    };

    using T_typeName = T;
    using InputMappingTemplated = InputMapping<T_typeName>;
    using ActionCallback = std::function<void(CallbackContext&, T_typeName)>;


    InputAction() {}
    InputAction(const std::string & Name) : name(Name) {}

    //! MapInput Maps input to device callbacks based on input mapping parameter
    //! @param inputMappings mapping of input for this action
    void mapInput(std::vector<InputMappingTemplated> inputMappings)
    {
        _inputMappings = inputMappings;
        for (auto & inputMapping : _inputMappings)
        {
            auto & lambda = _inputLambdas.emplace_back(
                [&](float value) {
                    // inputMapping.receiveFromDevice(value);
                }
            );
        }
    }

    ActionCallback started;
    ActionCallback performed;
    ActionCallback canceled;

    std::string name;

protected:

    T_typeName _value = T_typeName();

    std::vector<InputMappingTemplated> _inputMappings;
    std::vector<std::function<void(float)>> _inputLambdas;

    bool _started = false;
    int _activeInputs = 0;
};

template<>
class InputAction<bool>
{

public:
    struct CallbackContext {

    };

    using T_typeName = bool;
    using InputMappingTemplated = InputMapping<T_typeName>;
    using ActionCallback = std::function<void(CallbackContext&, T_typeName)>;


    InputAction() {}
    InputAction(const std::string & Name) : name(Name) {}

    //! MapInput Maps input to device callbacks based on input mapping parameter
    //! @param inputMappings mapping of input for this action
    void mapInput(std::vector<InputMappingTemplated> inputMappings)
    {
        _inputMappings = inputMappings;
        for (auto & inputMapping : _inputMappings)
        {
            auto & lambda = _inputLambdas.emplace_back(
                [&](float value) {
                    // inputMapping.receiveFromDevice(value);
                }
            );
            // Input::tryRegisterListenerOfDevice(
            //     getInputDeviceTypeFromKey(inputMapping.inputKey),
            //     0, inputMapping.inputKey, lambda
            // );
        }
    }

    ActionCallback started;
    ActionCallback performed;
    ActionCallback canceled;

    std::string name;

protected:

    T_typeName _value = T_typeName();

    std::vector<InputMappingTemplated> _inputMappings;
    std::vector<std::function<void(float)>> _inputLambdas;

    bool _started = false;
    int _activeInputs = 0;
};

template<>
class InputAction<glm::vec3>
{

public:
    struct CallbackContext {

    };

    using T_typeName = glm::vec3;
    using InputMappingTemplated = InputMapping<T_typeName>;
    using ActionCallback = std::function<void(CallbackContext&, T_typeName)>;
    
    
    InputAction() {}
    InputAction(const std::string & Name) : name(Name) {}

    //! MapInput Maps input to device callbacks based on input mapping parameter
    //! @param inputMappings mapping of input for this action
    void mapInput(std::vector<InputMappingTemplated> inputMappings)
    {
        _inputMappings = inputMappings;
        for (auto & inputMapping : _inputMappings)
        {
            auto & lambda = _inputLambdas.emplace_back(
                [&](float value) {
                    // inputMapping.receiveFromDevice(value);
                }
            );
            // Input::tryRegisterListenerOfDevice(
            //     getInputDeviceTypeFromKey(inputMapping.inputKey),
            //     0, inputMapping.inputKey, lambda
            // );
        }
    }

    ActionCallback started;
    ActionCallback performed;
    ActionCallback canceled;

    std::string name;

protected:

    T_typeName _value = T_typeName(0);

    std::vector<InputMappingTemplated> _inputMappings;
    std::vector<std::function<void(float)>> _inputLambdas;

    bool _started = false;
    int _activeInputs = 0;
};


class ActionMap
{

public:
    template<typename T>
    using Mapping = std::unordered_map<std::string, InputAction<T>>;

    template<typename T>
    InputAction<T> * createAction(const std::string & actionName);    

    template<typename T>
    InputAction<T> * getAction(const std::string & actionName);

private:
    Mapping<bool> _boolInputActions;
    Mapping<glm::vec3> _vectorInputActions;

};



// Input / Input Device

InputDeviceType getInputDeviceTypeFromKey(InputKey key);

class Input
{
public:
    struct InputDeviceState {
        float value { -99.f };
    };

    using Callback = std::function<void(float)>;
    using CallbackBank = std::vector<Callback>;
    using CallbackBankHandle = CallbackBank::const_iterator;

    struct InputDevice {
        using DeviceStateMap = std::unordered_map<InputKey, InputDeviceState>;

        InputDevice();
        InputDevice(InputDeviceType Type, int Index);

        InputDeviceType type;
        int index;

        CallbackBankHandle registerListener(
            InputKey inputKey,
            Callback callback
        );

        void unregisterListener(
            InputKey inputKey,
            CallbackBankHandle handle
        );

        void addToNewStateBuffer(InputKey key, InputDeviceState state);
        void pushAndCapture(DeviceStateMap & capture);
        
    private:
        std::unordered_map<InputKey, CallbackBank> _onStateChanged;

        std::vector<std::pair<InputKey, InputDeviceState>> _newStateBuffer;
        DeviceStateMap _currentState;
    };

    using DeviceIndex = int;
    using DeviceMap = std::unordered_map<InputDeviceType, std::unordered_map<DeviceIndex, InputDevice>>;

    void registerDevice(const InputDevice& device);
	void removeDevice(InputDeviceType type, int index);

    virtual void processInput();

    void activate(ActionMap inputActionMap);

protected:
    InputDevice * getDevice(InputDeviceType type, int index);

    std::vector<ActionMap> _activeMaps;
    double _mouseX;
    double _mouseY;

private:
    bool containsDevice(InputDeviceType deviceType, int deviceIndex);

    DeviceMap _devices;
};


} // namespace arete::input