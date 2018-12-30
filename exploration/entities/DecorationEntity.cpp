#include "DecorationEntity.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "../DecorationModel.h"

DecorationEntity::DecorationEntity(Model* model, const EntitySpawnInfo& info)
  : Entity{ model, info }
  , drawPercentage{ 0.0f }
  , drawState{ HIDDEN }
  , transform{ model->makeEntityTransform(position, rotation, scale)}
{

}

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

void DecorationEntity::draw_faces(GameState& state, DepthProgram& program, float time)
{
  if (drawPercentage <= 0.0f)
    return;

  std::array<glm::mat4, MAX_JOINTS> jointTransforms;
  program.setPositions(jointTransforms);
  program.setDrawPercentage(drawPercentage);
  model->draw_faces(program, time, transform);
}

void DecorationEntity::draw_lines(GameState& state, LineProgram& program, float time)
{
  if (drawPercentage <= 0.0f)
    return;

  std::array<glm::mat4, MAX_JOINTS> jointTransforms;
  program.setPositions(jointTransforms);
  program.setDrawPercentage(drawPercentage);
  model->draw_lines(program, time, transform);
}

void DecorationEntity::draw_debug(GameState& state, DebugProgram& program, float time)
{
  if (drawPercentage <= 0.0f)
    return;

  auto entityTransform = transform * model->transform;

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

  program.setModel(boxTransform);
  glBindVertexArray(state.boxVAO);
  glDrawArrays(GL_LINES, 0, 24);
  program.setModel(levelTransform);
  glBindVertexArray(state.boxVAO);
  glDrawArrays(GL_LINES, 0, 24);
}
