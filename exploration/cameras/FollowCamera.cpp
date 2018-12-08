#include "FollowCamera.h"

#include <iostream>

const auto CAMERA_DEADZONE = 0.12f;
const auto CAMERA_ROTATION_RATE = 0.05f;
const auto CAMERA_DISTANCE = 1.5f;
const auto CAMERA_DRIFT_MAX = 0.5f;
const auto CAMERA_DRIFT_CORRECTION_RATE = 0.1f;

FollowCamera::FollowCamera(ModelInstance** instance)
  : _instance{ instance }
  , optimalDistance{ 1.5f }
  , optimalAngle{ 0.0f }
  , distance{ 1.5f }
  , offsetAngle{ 0.0f }
  , optimalPosition{ 0, 6, 3 }
  , currentPosition{ }
{ }

void FollowCamera::update(GLFWwindow *window, float time, int selectedJoystickId)
{
  { // adjustments based on previous position
    auto dx = (*_instance)->position.x - optimalPosition.x;
    auto dy = (*_instance)->position.y - optimalPosition.y;
    optimalAngle = std::atan2(dy, dx) + glm::radians(90.0f);
  }

  { // keyboard
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      optimalDistance -= CAMERA_ROTATION_RATE;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      optimalDistance += CAMERA_ROTATION_RATE;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      optimalAngle += CAMERA_ROTATION_RATE;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      optimalAngle -= CAMERA_ROTATION_RATE;
  }

  { // controller
    auto axesCount = 0;
    auto axes = glfwGetJoystickAxes(selectedJoystickId, &axesCount);
    auto cameraXAxis = axes[4];
    auto cameraYAxis = axes[3]; // not used yet
    optimalAngle += std::abs(cameraXAxis) < CAMERA_DEADZONE ? 0.0f : cameraXAxis * CAMERA_ROTATION_RATE;
  }

  optimalPosition = (*_instance)->position + glm::vec3(glm::rotate(glm::mat4(), optimalAngle, { 0, 0, 1 }) * glm::vec4(0, 4, 2, 1)) * optimalDistance;
  currentPosition = (currentPosition) * (1 - CAMERA_DRIFT_CORRECTION_RATE) + (optimalPosition) * CAMERA_DRIFT_CORRECTION_RATE;
}

glm::mat4 FollowCamera::transform() const
{
  return glm::lookAt(position(), (*_instance)->position + glm::vec3(0, 0, 1), { 0, 0, 1 });
}

glm::vec3 FollowCamera::position() const
{
  return currentPosition;
}
