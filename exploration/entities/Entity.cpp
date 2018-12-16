#include "Entity.h"

Entity::Entity(Model* model, const EntitySpawnInfo& info)
  : model{ model }
  , position{ info.location}
  , rotation{ info.rotation.z } // TODO: use full rotation info
  , scale{ info.scale.x }       // TODO: use full scale info
{ }

void Entity::update(GameState& state, float time)
{

}

void Entity::draw_faces(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_faces(program, time, position, rotation, scale);
}

void Entity::draw_lines(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_lines(program, time, position, rotation, scale);
}

void Entity::draw_debug(GameState& state, Program& program, float time)
{

}
