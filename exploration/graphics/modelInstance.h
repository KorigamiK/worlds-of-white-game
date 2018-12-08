#ifndef WILT_MODELINSTANCE_H
#define WILT_MODELINSTANCE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

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

  virtual void update(GLFWwindow *window, float time);

  virtual void draw_faces(Program& program, float time);
  virtual void draw_lines(Program& program, float time);
  virtual void draw_debug(Program& program, float time);

}; // class ModelInstance

class IAnimator
{
public:
  virtual void applyAnimation(Program& program, float time, ModelInstance& instance) = 0;

}; // class IAnimator

#include "model.h"
#include "program.h"

#endif // !WILT_MODELINSTANCE_H
