#include "ModelInstance.h"

ModelInstance::ModelInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale)
  : model{ model }
  , position{ position }
  , rotation{ rotation }
  , animator{ animator }
  , scale{ scale }
{ }

void ModelInstance::update(GLFWwindow *window, float time) { }

void ModelInstance::draw_faces(Program& program, float time)
{
  model->draw_faces(*this, program, time);
}

void ModelInstance::draw_lines(Program& program, float time)
{
  model->draw_lines(*this, program, time);
}

void ModelInstance::draw_debug(Program& program, float time)
{

}