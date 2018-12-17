#include "DecorationEntity.h"

#include <iostream>

#include "../DecorationModel.h"

void DecorationEntity::update(GameState& state, float time)
{
  auto& model = *(DecorationModel*)this->model;

  auto distance = glm::length(position - state.playerPosition);
  auto middle = (model.farDrawDistance + model.nearDrawDistance) / 2;

  if (drawState != HIDDEN && distance > model.farHideDistance)
    drawState = HIDING_FAR;
  if (drawState != HIDDEN && distance < model.nearHideDistance)
    drawState = HIDING_NEAR;
  if (drawState == HIDDEN && distance < model.farDrawDistance && distance > middle)
    drawState = DRAWING_FAR;
  if (drawState == HIDDEN && distance > model.nearDrawDistance && distance < middle)
    drawState = DRAWING_NEAR;

  switch (drawState)
  {
  case DRAWING_FAR: 
    drawPercentage += model.farDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage >= 1.0f)
      drawState = DRAWN;
    break;
  case DRAWING_NEAR:
    drawPercentage += model.nearDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage >= 1.0f)
      drawState = DRAWN;
    break;
  case HIDING_FAR:
    drawPercentage -= model.farDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage <= 0.0f)
      drawState = HIDDEN;
    break;
  case HIDING_NEAR:
    drawPercentage -= model.nearDrawRate / 144.0f; // TODO: use time delta
    if (drawPercentage <= 0.0f)
      drawState = HIDDEN;
    break;
  }
}

void DecorationEntity::draw_faces(GameState& state, Program& program, float time)
{
  if (drawPercentage <= 0.0f)
    return;

  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", drawPercentage);
  model->draw_faces(program, time, position, rotation, scale);
}

void DecorationEntity::draw_lines(GameState& state, Program& program, float time)
{
  if (drawPercentage <= 0.0f)
    return;

  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", drawPercentage);
  model->draw_lines(program, time, position, rotation, scale);
}

void DecorationEntity::draw_debug(GameState& state, Program& program, float time)
{

}
