#include <glm/gtc/matrix_transform.hpp>

#include "FreeCamera.h"

FreeCamera::FreeCamera()
  : _location{ 0, -0.5, 1 }
  , _direction{ 0, 1, 0 }
  , CAMERA_SPEED{ 0.05f }
{ }

void FreeCamera::update(GLFWwindow *window, float time)
{
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    _location += _direction * CAMERA_SPEED;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    _location -= _direction * CAMERA_SPEED;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    _direction = glm::vec3(glm::rotate(glm::mat4(), glm::radians(30.0f) / 144.0f, { 0, 0, 1 }) * glm::vec4(_direction, 1.0f));
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    _direction = glm::vec3(glm::rotate(glm::mat4(), -glm::radians(30.0f) / 144.0f, { 0, 0, 1 }) * glm::vec4(_direction, 1.0f));
}

glm::mat4 FreeCamera::transform() const
{
  return glm::lookAt(_location, _location + _direction, { 0, 0, 1 });
}

glm::vec3 FreeCamera::position() const
{
  return _location;
}
