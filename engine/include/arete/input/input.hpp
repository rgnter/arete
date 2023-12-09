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



// Input / Input Device

InputDeviceType getInputDeviceTypeFromKey(InputKey key);

class Input
{
public:

	struct InputDeviceState {
		float value { 0 };
	};

	//! Callback for device state changed
	//! 	@1 - newState
	//! 	@1 - oldState
	using DeviceCallback = std::function<void(float, float)>;
	using DeviceCallbackBank = std::vector<DeviceCallback>;
	using DeviceCallbackBankHandle = DeviceCallbackBank::const_iterator;

	struct InputDevice {
		using DeviceStateMap = std::unordered_map<InputKey, InputDeviceState>;

		InputDevice();
		InputDevice(InputDeviceType Type, int Index);

		InputDeviceType type;
		int index;

		DeviceCallbackBankHandle registerListener(
			InputKey inputKey,
			DeviceCallback callback
		);

		void unregisterListener(
			InputKey inputKey,
			DeviceCallbackBankHandle handle
		);

		void addToNewStateBuffer(InputKey key, InputDeviceState state);
		void pushAndCapture(DeviceStateMap & capture);
			
	private:
		std::unordered_map<InputKey, DeviceCallbackBank> _onStateChanged;

		std::vector<std::pair<InputKey, InputDeviceState>> _newStateBuffer;
		DeviceStateMap _currentState;
	};

	using DeviceIndex = int;
	using DeviceMap = std::unordered_map<InputDeviceType, std::unordered_map<DeviceIndex, InputDevice>>;

	void registerDevice(const InputDevice& device);
	void removeDevice(InputDeviceType type, int index);

	InputDevice * getDevice(InputDeviceType type, int index);

	using OnProcessCallback = std::function<void(void)>;

	void addOnProcessListener(OnProcessCallback callback);
	void removeOnProcessListener(OnProcessCallback callback);

	virtual void processInput();


protected:

	int _mouseX = 0;
	int _mouseY = 0;

	std::vector<OnProcessCallback> _onProcessInput;

private:
	bool containsDevice(InputDeviceType deviceType, int deviceIndex);

	DeviceMap _devices;
};




// Composite

enum class Composite {
	VALUE,
	DELTA
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

struct EventContext {
	bool started = false;
	bool performed = false;
	bool canceled = false;
};

class ActionMap;

template<typename T>
class InputAction;

template<>
class InputAction<bool>
{
	friend ActionMap;

public:
	using T_typeName = bool;
	using InputMappingTemplated = InputMapping<T_typeName>;
	using ActionEvent = std::function<void(EventContext&, T_typeName)>;


	InputAction() : name("Undefined") {}
	InputAction(const std::string & Name) : name(Name) {}

	//! MapInput Maps input to device callbacks based on input mapping parameter
	//! @param inputMappings mapping of input for this action
	void mapInput(std::vector<InputMappingTemplated> inputMappings)
	{
		_inputMappings = inputMappings;
	}


	EventContext ctx;

	ActionEvent started;
	ActionEvent performed;
	ActionEvent canceled;

	std::string name;



protected:

	//! Callback for input action to process received values and
	//! 	invoke event potentially invoke event
	void onInputProcess()
	{
		if (ctx.started && started)
		{
			started(ctx, _value);
		}
		if (ctx.performed && performed)
		{
			performed(ctx, _value);
		}
		if (ctx.canceled && canceled)
		{
			canceled(ctx, _value);
		}

		ctx.started = false;
		ctx.performed = false;
		ctx.canceled = false;
	}

	//! Binds input device callbacks, so Input Action receives values from
	//!		mapped device based on provided Input Mappings
	//! @param input - input which handles input devices
	void bind(Input & input)
	{
		for (auto & inputMapping : _inputMappings)
		{
			if (auto * device = input.getDevice(getInputDeviceTypeFromKey(inputMapping.inputKey), 0))
			{
				_inputDeviceCallbackHandles.emplace_back(
					device->registerListener(
						inputMapping.inputKey,
						[&](float newValue, float oldValue) {
							processValueChange(&inputMapping, newValue, oldValue);
						}
					)
				);
			}
		}

		input.addOnProcessListener(std::bind(&InputAction<T_typeName>::onInputProcess, this));
	}


	void processValueChange(InputMappingTemplated * inputMappingPtr, float value, float oldValue)
	{
		T_typeName newValue = value != 0;

		if (newValue && _activeInputs.size() == 0)
		{
			ctx.started = true;
		}
		
		_activeInputs[inputMappingPtr] = newValue;
		_value = _value || newValue;

		ctx.performed = true;
		
		if (!newValue)
		{
			_activeInputs.erase(inputMappingPtr);
			if (_activeInputs.size() == 0)
			{
				_value = false;
				ctx.canceled = true;
			}
		}
	}

	
	T_typeName _value = false;

	std::vector<InputMappingTemplated> _inputMappings;
	std::vector<Input::DeviceCallbackBankHandle> _inputDeviceCallbackHandles;

	std::unordered_map<InputMappingTemplated*, T_typeName> _activeInputs;

};

template<>
class InputAction<glm::vec3>
{
	friend ActionMap;

public:

	using T_typeName = glm::vec3;
	using InputMappingTemplated = InputMapping<T_typeName>;
	using ActionCallback = std::function<void(EventContext&, T_typeName)>;
	

	InputAction() : name("Undefined"), composite(Composite::VALUE) {}
	InputAction(const std::string & Name, Composite Composite) : name(Name), composite(Composite) {}

	//! MapInput Maps input to device callbacks based on input mapping parameter
	//! @param inputMappings mapping of input for this action
	void mapInput(std::vector<InputMappingTemplated> inputMappings)
	{
		_inputMappings = inputMappings;
	}


	EventContext ctx;

	Composite composite = Composite::VALUE;
	
	ActionCallback started;
	ActionCallback performed;
	ActionCallback canceled;

	std::string name;



protected:

	//! Callback for input action to process received values and
	//! 	invoke event potentially invoke event
	void onInputProcess()
	{
		if (!_shouldProcessInput)
		{
			return;
		}

		T_typeName relaxedValue = glm::vec3(0);
		T_typeName eventValue = T_typeName(0);

		switch (composite)
		{
			case Composite::VALUE:
				eventValue = _value;

				if (!_isActive && _valueTemp == relaxedValue && _value != relaxedValue)
				{
					ctx.started = true;
					_isActive = true;
				}
				if (_isActive) 
				{
					if(_valueTemp != _value)
					{
						ctx.performed = true;
					}
					if (_value == relaxedValue && _valueTemp != relaxedValue)
					{
						ctx.canceled = true;
						_isActive = false;
						_shouldProcessInput = false;
					}				
				}

				_valueTemp = eventValue;


				if (ctx.started && started)
				{
					started(ctx, eventValue);
				}
				if (ctx.performed && performed)
				{
					performed(ctx, eventValue);
				}
				if (ctx.canceled)
				{
					if (canceled)
					{
						canceled(ctx, eventValue);
					}
					_activeInputs.clear();
				}

				ctx.started = false;
				ctx.performed = false;
				ctx.canceled = false;
			break;



			case Composite::DELTA:
				eventValue = _value - _valueTemp;
				_valueTemp = _value;
				bool isRelaxed = eventValue == glm::vec3(0);

				if (!isRelaxed)
				{
					if (!_isActive)
					{
						ctx.started = true;
						_isActive = true;
					}

					if (_isActive)
					{
						ctx.performed = true;
					}
				}

				if (isRelaxed && _isActive)
				{
					ctx.performed = true;
					ctx.canceled = true;
					_isActive = false;
					_shouldProcessInput = false;
				}

				if (ctx.started && started)
				{
					started(ctx, eventValue);
				}
				if (ctx.performed && performed)
				{
					performed(ctx, eventValue);
				}
				if (ctx.canceled)
				{
					if (canceled)
					{
						canceled(ctx, eventValue);
					}
					ctx.performed = false;
					_activeInputs.clear();
				}

				ctx.started = false;
				ctx.canceled = false;
			break;
		}
	}

	//! Binds input device callbacks, so Input Action receives values from
	//!		mapped device based on provided Input Mappings
	//! @param input - input which handles input devices
	void bind(Input & input)
	{
		for (auto & inputMapping : _inputMappings)
		{
			if (auto * device = input.getDevice(getInputDeviceTypeFromKey(inputMapping.inputKey), 0))
			{
				_inputDeviceCallbackHandles.emplace_back(
					device->registerListener(
						inputMapping.inputKey,
						[&](float newValue, float oldValue) {
							processValueChange(&inputMapping, newValue, oldValue);
						}
					)
				);
			}
		}

		input.addOnProcessListener(std::bind(&InputAction<T_typeName>::onInputProcess, this));
	}


	void processValueChange(InputMappingTemplated* inputMappingPtr, float newState, float oldState)
	{
		bool hasValue = newState != 0;

		if (hasValue && _activeInputs.size() == 0)
		{
			_value = T_typeName(0);
			_valueTemp = T_typeName(0);
			_shouldProcessInput = true;
		}

		T_typeName newValue = newState * inputMappingPtr->axis;

		if (_activeInputs.contains(inputMappingPtr))
		{
			_value -= _activeInputs[inputMappingPtr].first;
			_activeInputs[inputMappingPtr].first = newValue;
			_value += newValue;
		}
		else
		{
			_activeInputs[inputMappingPtr].first = newValue;
			_value += newValue;

			T_typeName oldValue = oldState * inputMappingPtr->axis;
			_activeInputs[inputMappingPtr].second = oldValue;
			_valueTemp += oldValue;
		}
	
		if (!hasValue)
		{
			_activeInputs.erase(inputMappingPtr);
			if (_activeInputs.size() == 0)
			{
				_value = T_typeName(0);
			}
		}
	}


	bool _shouldProcessInput = false;
	bool _isActive = false;
	T_typeName _value = T_typeName(0);
	T_typeName _valueTemp = T_typeName(0);

	std::vector<InputMappingTemplated> _inputMappings;
	std::vector<Input::DeviceCallbackBankHandle> _inputDeviceCallbackHandles;

	// map for active input mappings
	//	InputMappingTemplated - inputMapping
	//  std::pair <currentValue, onActivateValue>
	std::unordered_map<InputMappingTemplated*, std::pair<T_typeName, T_typeName>> _activeInputs;
};


class ActionMap
{

public:
	template<typename T>
	using Mapping = std::unordered_map<std::string, InputAction<T>>;

	template<typename T>
	InputAction<T> * createAction(
		const std::string & actionName
	);

	template<typename T>
	InputAction<T> * createAction(
		const std::string & actionName,
		Composite composite
	);

	template<typename T>
	InputAction<T> * getAction(const std::string & actionName);

	void bind(Input & input);

private:
	Mapping<bool> _boolInputActions;
	Mapping<glm::vec3> _vectorInputActions;

};


} // namespace arete::input