#ifndef WILT_ENTITY_H
#define WILT_ENTITY_H

#include <vector>

#include <glm/glm.hpp>

#include "../EntitySpawnInfo.h"
#include "../GameState.h"

class IAnimator;
class Model;
class Program;

class Entity
{
public:
  Model* model;
  glm::vec3 position;
  glm::vec3 rotation;
  float scale;

  Entity(Model* model, const EntitySpawnInfo& info);

  virtual void update(GameState& state, float time);

  virtual void draw_faces(GameState& state, Program& program, float time);
  virtual void draw_lines(GameState& state, Program& program, float time);
  virtual void draw_debug(GameState& state, Program& program, float time);

}; // class Entity

#include "../model.h"
#include "../graphics/program.h"

#endif // !WILT_ENTITY_H
