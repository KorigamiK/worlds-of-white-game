#include "FollowCamera.h"

FollowCamera::FollowCamera(ModelInstance** instance)
  : _instance{ instance }
  , distance{ 1.5f }
  , offsetAngle{ 0.0f }
{ }

void FollowCamera::update(GLFWwindow *window, float time, int selectedJoystickId)
{
  // keyboard
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    distance -= 0.01f;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    distance += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    offsetAngle += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    offsetAngle -= 0.01f;

  // controller
  const auto CAMERA_DEADZONE = 0.12f;
  const auto CAMERA_ROTATION_RATE = 0.05f;

  auto axesCount = 0;
  auto axes = glfwGetJoystickAxes(selectedJoystickId, &axesCount);
  auto cameraXAxis = axes[4];
  auto cameraYAxis = axes[3]; // not used yet

  offsetAngle += std::abs(cameraXAxis) < CAMERA_DEADZONE ? 0.0f : cameraXAxis * CAMERA_ROTATION_RATE;
}

glm::mat4 FollowCamera::transform() const
{
  return glm::lookAt(position(), (*_instance)->position + glm::vec3(0, 0, 1), { 0, 0, 1 });
}

glm::vec3 FollowCamera::position() const
{
  return (*_instance)->position + glm::vec3(glm::rotate(glm::mat4(), /*(*_instance)->rotation +*/ offsetAngle, { 0, 0, 1 }) * glm::vec4(0, 4, 2, 1)) * distance;
}
