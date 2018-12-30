#ifndef WILT_ENTITY_H
#define WILT_ENTITY_H

#include <vector>

#include <glm/glm.hpp>

#include "../EntitySpawnInfo.h"
#include "../GameState.h"

class IAnimator;
class Model;
class Program;
class DepthProgram;
class LineProgram;
class DebugProgram;

class Entity
{
public:
  Model* model;
  glm::vec3 position;
  glm::vec3 rotation;
  float scale;

  Entity(Model* model, const EntitySpawnInfo& info);

  virtual void update(GameState& state, float time);

  virtual void draw_faces(GameState& state, DepthProgram& program, float time);
  virtual void draw_lines(GameState& state, LineProgram& program, float time);
  virtual void draw_debug(GameState& state, DebugProgram& program, float time);

}; // class Entity

#include "../model.h"
#include "../graphics/program.h"
#include "../graphics/DepthProgram.h"
#include "../graphics/LineProgram.h"
#include "../graphics/DebugProgram.h"

#endif // !WILT_ENTITY_H
