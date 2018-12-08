#ifndef WILT_ICAMERA_H
#define WILT_ICAMERA_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

class ICamera
{
public:
  virtual void update(GLFWwindow *window, float time) = 0;
  virtual glm::mat4 transform() const = 0;
  virtual glm::vec3 position() const = 0;
};

#endif // !WILT_ICAMERA_H
