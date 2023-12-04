#pragma once

#include "./input.hpp"

struct GLFWwindow;
struct GLFWgamepadstate;

namespace arete::input
{

class GlfwInput : public Input
{
public:
    void setup(GLFWwindow * glfwWindow);

    InputKey keyToInputKey(int key);
    InputKey mouseButtonToInputKey(int mouseButton);

    std::unordered_map<InputKey, InputDeviceState> getGamepadStateByID (int joystickId);

private:
    std::unordered_map<InputKey, InputDeviceState> getGamepadState	(const GLFWgamepadstate& state);
};

} // namespace arete::input