#ifndef WILT_MODELINSTANCE_H
#define WILT_MODELINSTANCE_H

#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "../GameState.h"
#include "../graphics/IAnimator.h"

class IAnimator;
class Model;
class Program;

class ModelInstance
{
public:
  Model* model;
  glm::vec3 position;
  float rotation;
  IAnimator* animator;
  float scale;

  ModelInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale = 1.0f);

  virtual void update(GameState& state, float time);

  virtual void draw_faces(Program& program, float time);
  virtual void draw_lines(Program& program, float time);
  virtual void draw_debug(Program& program, float time);

}; // class ModelInstance

#include "../graphics/model.h"
#include "../graphics/program.h"
#include "../graphics/IAnimator.h"

#endif // !WILT_MODELINSTANCE_H
