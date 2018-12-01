#ifndef WILT_MODELINSTANCE_H
#define WILT_MODELINSTANCE_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class IAnimator;
class Model;
class Program;

struct ModelInstance
{
  Model* model;
  glm::vec3 position;
  float rotation;
  IAnimator* animator;
  float scale;

  ModelInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale = 1.0f)
    : model{ model }
    , position{ position }
    , rotation{ rotation }
    , animator{ animator }
    , scale{ scale }
  { }

  virtual void update(GLFWwindow *window, float time) { }

}; // class ModelInstance

class IAnimator
{
public:
  virtual void applyAnimation(Program& program, float time, ModelInstance& instance) = 0;

}; // class IAnimator

#include "model.h"
#include "program.h"

#endif // !WILT_MODELINSTANCE_H
