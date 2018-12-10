#include "DecorationInstance.h"

#include <iostream>

void DecorationInstance::update(GameState& state, float time)
{
  auto distance = glm::length(position - state.playerPosition);
  auto middle = (farDrawDistance + nearDrawDistance) / 2;

  if (drawState == DRAWN && distance > farHideDistance)
    drawState = HIDING_FAR;
  if (drawState == DRAWN && distance < nearHideDistance)
    drawState = HIDING_NEAR;
  if (drawState == HIDDEN && distance < farDrawDistance && distance > middle)
    drawState = DRAWING_FAR;
  if (drawState == HIDDEN && distance > nearDrawDistance && distance < middle)
    drawState = DRAWING_NEAR;

  switch (drawState)
  {
  case DRAWING_FAR: 
    drawPercentage += farDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage >= 1.0f)
      drawState = DRAWN;
    break;
  case DRAWING_NEAR:
    drawPercentage += nearDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage >= 1.0f)
      drawState = DRAWN;
    break;
  case HIDING_FAR:
    drawPercentage -= farDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage <= 0.0f)
      drawState = HIDDEN;
    break;
  case HIDING_NEAR:
    drawPercentage -= nearDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage <= 0.0f)
      drawState = HIDDEN;
    break;
  }
}

void DecorationInstance::draw_faces(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", drawPercentage);
  model->draw_faces(program, time, position, rotation, scale);
}

void DecorationInstance::draw_lines(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", drawPercentage);
  model->draw_lines(program, time, position, rotation, scale);
}

void DecorationInstance::draw_debug(GameState& state, Program& program, float time)
{

}
