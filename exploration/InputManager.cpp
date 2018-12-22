#include "InputManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const auto INPUTMANAGER_MAX_KEY_ID = 512;

InputManager::InputManager(int joystickId)
  : joystickId{ joystickId }
{
  keysCurrent.assign(INPUTMANAGER_MAX_KEY_ID, GLFW_RELEASE);
  keysPrevious.assign(INPUTMANAGER_MAX_KEY_ID, GLFW_RELEASE);
}

void InputManager::update()
{
  { // keys
    keysPrevious = keysCurrent;

    // keysCurrent is filled out by calls to setKeyState
  }

  { // buttons
    auto buttonsCount = 0;
    auto buttonsRaw = glfwGetJoystickButtons(joystickId, &buttonsCount);

    buttonsPrevious = std::move(buttonsCurrent);
    buttonsCurrent = std::vector<unsigned char>(buttonsRaw, buttonsRaw + buttonsCount); // TODO: reuse vector space
  }

  { // axes
    auto axesCount = 0;
    auto axesRaw = glfwGetJoystickAxes(joystickId, &axesCount);

    axes = std::vector<float>(axesRaw, axesRaw + axesCount); // TODO: reuse vector space
  }
}

bool InputManager::isKeyTriggered(int key)
{
  return keysPrevious[key] == GLFW_RELEASE && keysCurrent[key] >= GLFW_PRESS;
}

bool InputManager::isKeyHeld(int key)
{
  return keysPrevious[key] >= GLFW_PRESS && keysCurrent[key] >= GLFW_PRESS;
}

bool InputManager::isKeyReleased(int key)
{
  return keysPrevious[key] >= GLFW_PRESS && keysCurrent[key] == GLFW_RELEASE;
}

bool InputManager::isButtonTriggered(int button)
{
  if (button < 0 || button >= buttonsPrevious.size() || button >= buttonsCurrent.size())
    return false;

  return buttonsPrevious[button] == GLFW_RELEASE && buttonsCurrent[button] >= GLFW_PRESS;
}

bool InputManager::isButtonHeld(int button)
{
  if (button < 0 || button >= buttonsPrevious.size() || button >= buttonsCurrent.size())
    return false;

  return buttonsPrevious[button] >= GLFW_PRESS && buttonsCurrent[button] >= GLFW_PRESS;
}

bool InputManager::isButtonReleased(int button)
{
  if (button < 0 || button >= buttonsPrevious.size() || button >= buttonsCurrent.size())
    return false;

  return buttonsPrevious[button] >= GLFW_PRESS && buttonsCurrent[button] == GLFW_RELEASE;
}

float InputManager::getAxis(int axis)
{
  return axes[axis];
}

void InputManager::setKeyState(int key, int state)
{
  if (key < 0 || key >= INPUTMANAGER_MAX_KEY_ID)
    return;
  if (state != GLFW_RELEASE && state != GLFW_PRESS && state != GLFW_REPEAT)
    return;

  keysCurrent[key] = state;
}
