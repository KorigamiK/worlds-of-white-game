#ifndef WILT_CHARACTERINSTANCE_H
#define WILT_CHARACTERINSTANCE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "Instance.h"

class CharacterInstance : public Instance
{
public:
  using Instance::Instance;

  float speed = 0.0;
  glm::vec3 velocity{ 0, 1, 0 };

public:
  void update(GameState& state, float time) override;
};

#endif // !WILT_CHARACTERINSTANCE_H
