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

enum Axis {
    Forward,
    Back,
    Right,
    Left,
    Up,
    Down
};

template<typename T>
struct InputMapping;

template<>
struct InputMapping<bool> {
    InputKey inputKey;
};

template<>
struct InputMapping<glm::vec3> {
    InputKey inputKey;
    Axis axis;
};



// ActionMap / InputAction

class InputActionBase
{
public:
    InputActionBase() {}
    InputActionBase(const std::string & Name);

    struct CallbackContext {
        
        template<typename T>
        T readValue();

    private:
        void * data;
    };

    using ActionCallback = std::function<void(const CallbackContext&)>;

    ActionCallback started;
    ActionCallback performed;
    ActionCallback canceled;

    std::string name;
};

template<typename T>
class InputAction : public InputActionBase
{
public:
    InputAction() : InputActionBase() {}
    InputAction(const std::string & Name) : InputActionBase(Name) {}

    void mapInput(
        std::vector<InputMapping<T>> inputMappings
    );

    T data;
};

class ActionMap
{

public:
    template<typename T>
    using Mapping = std::unordered_map<std::string, InputAction<T>>;

    template<typename T>
    void mapInput(
        const std::string & actionName,
        std::vector<InputMapping<T>> inputMappings
    );
    template<typename T>
    void mapInput(
        const std::string & actionName,
        InputMapping<T> inputMapping
    );

    template<typename T>
    bool getOrAddAction(const std::string & actionName, InputAction<T> & inputAction);
    template<typename T>
    bool getAction(const std::string & actionName, InputAction<T> & inputAction);

private:
    Mapping<bool> _boolInputActions;
    Mapping<glm::vec3> _vectorInputActions;

};



// Input / Input Device

static InputDeviceType getInputDeviceTypeFromKey(InputKey key);

class Input
{
friend InputAction<glm::vec3>;
friend InputAction<bool>;

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

    static void registerDevice(const InputDevice& device);
	static void removeDevice(InputDeviceType type, int index);


    void processInput();

protected:
    static bool tryGetInputDevice(InputDeviceType type, int index, InputDevice & inputDevice);

private:
    static bool containsDevice(InputDeviceType deviceType, int deviceIndex);

    static DeviceMap _devices;
};


} // namespace arete::input