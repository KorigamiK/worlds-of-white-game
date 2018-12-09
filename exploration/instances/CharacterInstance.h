#ifndef WILT_CHARACTERINSTANCE_H
#define WILT_CHARACTERINSTANCE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "ModelInstance.h"

class CharacterInstance : public ModelInstance
{
public:
  using ModelInstance::ModelInstance;

  float speed = 0.0;
  glm::vec3 velocity{ 0, 1, 0 };

public:
  void update(GLFWwindow* window, float time) override;
};

#endif // !WILT_CHARACTERINSTANCE_H
