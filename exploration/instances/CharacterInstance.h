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
  // Instance overrides
  void update(GameState& state, float time) override;
  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;
};

#endif // !WILT_CHARACTERINSTANCE_H
