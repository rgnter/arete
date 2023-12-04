#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>
#include <string>

namespace arete::input
{

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

enum class InputSource {
	UNKNOWN,
	KEYBOARD,
	MOUSE,
	GAMEPAD
};

struct InputAction {
	std::string actionName { "" };
	float scale { 1.f };
};

InputSource getInputSourceFromKey(InputKey key);

enum class InputDeviceType {
	KEYBOARD,
	MOUSE,
	GAMEPAD
};

struct InputDeviceState {
	float value { -99.f };
};

using InputDeviceStateCallbackFunc = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;

struct InputDevice {
	InputDeviceType type;
	int index;
	std::unordered_map<InputKey, InputDeviceState> currentState;
	InputDeviceStateCallbackFunc stateFunc;
};

class Input
{
public:

	using ActionCallbackFunc = std::function<bool(InputSource, int, float)>;

public:
  //! Default contructor.
  Input() = default;

  //! Default destructor.
  virtual ~Input() = default;

public:
	struct ActionCallback {
		std::string ref;
		ActionCallbackFunc func;
	};

	struct ActionEvent {
		std::string actionName;
		InputSource source;
		int sourceIndex;
		float value;
	};

	void registerDevice(const InputDevice& device);
	void removeDevice(InputDeviceType type, int index);

	void registerActionCallback(const std::string& actionName, const ActionCallback& callback);
	void removeActionCallback(const std::string& actionName, const std::string& callbackRef);

	void mapInputToAction(InputKey key, const InputAction& action);
	void unmapInputFromAction(InputKey key, const std::string& actionName);

	void processInput();

	std::unordered_map<InputKey, InputDeviceState> getKeyboardState		(int index) { return _keyboardState			; }
	std::unordered_map<InputKey, InputDeviceState> getMouseState		(int index) { return _mouseState			; }
	std::unordered_map<InputKey, InputDeviceState> getGamepadStateById	(int joystickId);

protected:
	std::vector<ActionEvent> generateActionEvent(int deviceIndex, InputKey key, float newVal);
	void propagateActionEvent(ActionEvent event);

	void updateKeyboardState(InputKey key, float value);
	void updateMouseState(InputKey button, float value);

private:
	std::unordered_map<InputKey, std::vector<InputAction>>	_inputActionMapping;
	std::unordered_map<std::string, std::vector<ActionCallback>>	_actionCallbacks;

	std::vector<InputDevice>										_devices;

	std::unordered_map<InputKey, InputDeviceState>							_keyboardState;
	std::unordered_map<InputKey, InputDeviceState>							_mouseState;

	std::unordered_map<int, std::unordered_map<InputKey, InputDeviceState>> _gamepadState;
};

} // namespace arete::input