#include "DecorationEntity.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

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
  if (drawPercentage <= 0.0f)
    return;

  auto entityTransform = glm::mat4()
    * glm::translate(glm::mat4(), position)
    * glm::rotate(glm::mat4(), rotation, { 0,0,1 })
    * glm::scale(glm::mat4(), glm::vec3(scale, scale, scale))
    * model->transform;

  auto percentage = drawPercentage > 1.0f ? 1.0f : drawPercentage;
  auto offsetBox = (model->boundingA + model->boundingB) / 2.0f;
  auto offsetLvl = offsetBox;
  auto scales = offsetBox - model->boundingA;
  offsetLvl.z = model->boundingA.z * (1 - percentage) + model->boundingB.z * percentage;

  auto boxTransform = entityTransform
    * glm::translate(glm::mat4(), offsetBox)
    * glm::scale(glm::mat4(), glm::vec3(scales.x, scales.y, scales.z));

  auto levelTransform = entityTransform
    * glm::translate(glm::mat4(), offsetLvl)
    * glm::scale(glm::mat4(), glm::vec3(scales.x, scales.y, scales.z * 0.0f));

  program.setMat4("model", boxTransform);
  glBindVertexArray(state.boxVAO);
  glDrawArrays(GL_LINES, 0, 24);
  program.setMat4("model", levelTransform);
  glBindVertexArray(state.boxVAO);
  glDrawArrays(GL_LINES, 0, 24);
}
