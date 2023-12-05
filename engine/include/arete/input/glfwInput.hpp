#pragma once

#include "./input.hpp"

struct GLFWwindow;

namespace arete::input
{

class GlfwInput : public Input
{
public:
    void setup(GLFWwindow * glfwWindow);

    virtual void processInput() override;

    static InputKey keyToInputKey(int key);
    static InputKey mouseButtonToInputKey(int mouseButton);
};

} // namespace arete::input