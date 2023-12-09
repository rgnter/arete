#pragma once

#include "./input.hpp"

struct GLFWwindow;

namespace arete::input
{

class GlfwInput : public Input
{
public:
    void bind(GLFWwindow * glfwWindow);

    virtual void processInput() override;
    virtual void setMouseMode(MouseMode) override;

    static InputKey keyToInputKey(int key);
    static InputKey mouseButtonToInputKey(int mouseButton);

private:
    GLFWwindow * boundWindow;
};

} // namespace arete::input