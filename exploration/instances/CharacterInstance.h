#ifndef WILT_CHARACTERINSTANCE_H
#define WILT_CHARACTERINSTANCE_H

#include <chrono>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "PhysicsInstance.h"

class CharacterInstance : public PhysicsInstance
{
public:
  CharacterInstance(Model* model, glm::vec3 position, float rotation, float scale = 1.0f);

  glm::vec3 velocity;
  
  bool dashing;
  bool dashUsed;
  std::chrono::time_point<std::chrono::high_resolution_clock> dashTime;
  glm::vec3 dashDirection;

public:
  // Instance overrides
  void update(GameState& state, float time) override;
  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;
};

#endif // !WILT_CHARACTERINSTANCE_H
