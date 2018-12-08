#ifndef WILT_FOLLOWCAMERA_H
#define WILT_FOLLOWCAMERA_H

#include "ICamera.h"
#include "../graphics/modelInstance.h"

class FollowCamera : public ICamera
{
public:
  ModelInstance** _instance;
  float distance = 1.5f;
  float offsetAngle = 0.0f;

public:
  FollowCamera(ModelInstance** instance)
    : _instance{ instance }
  { }

public:
  virtual void update(GLFWwindow *window, float time)
  {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      distance -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      distance += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      offsetAngle += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      offsetAngle -= 0.01f;
  }

  virtual glm::mat4 transform() const
  {
    return glm::lookAt(position(), (*_instance)->position + glm::vec3(0, 0, 1), { 0, 0, 1 });
  }

  virtual glm::vec3 position() const
  {
    return (*_instance)->position + glm::vec3(glm::rotate(glm::mat4(), /*(*_instance)->rotation +*/ offsetAngle, { 0, 0, 1 }) * glm::vec4(0, 4, 2, 1)) * distance;
  }
};

#endif // !WILT_FOLLOWCAMERA_H
