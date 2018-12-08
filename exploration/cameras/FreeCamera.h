#ifndef WILT_FREECAMERA_H
#define WILT_FREECAMERA_H

#include "ICamera.h"
#include <glm/gtc/matrix_transform.hpp>

class FreeCamera : public ICamera
{
private:
  glm::vec3 _location;
  glm::vec3 _direction;

  float CAMERA_SPEED = 0.05f;

public:
  FreeCamera()
    : _location{ 0, -0.5, 1 }
    , _direction{ 0, 1, 0 }
  { }

public:
  virtual void update(GLFWwindow *window, float time)
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

  virtual glm::mat4 transform() const
  {
    return glm::lookAt(_location, _location + _direction, { 0, 0, 1 });
  }

  virtual glm::vec3 position() const
  {
    return _location;
  }
};

#endif // !WILT_FREECAMERA_H
