#include "SpiritEntity.h"

#include <iostream>
#include <random>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

const auto SPIRIT_CORRECTION_RATE = 0.04f;
const auto SPIRIT_MIN_SPEED = 0.1f;
const auto SPIRIT_MIN_ROTATION = 0.2f;
const auto SPIRIT_MIN_PLAYER_DISTANCE = 1.0f;
const auto SPIRIT_TAIL_DISTANCE_1 = 0.25f;
const auto SPIRIT_TAIL_DISTANCE_2 = 0.35f;
const auto SPIRIT_TAIL_DISTANCE_3 = 0.45f;
const auto SPIRIT_TAIL_SIZE_1 = 0.50f;
const auto SPIRIT_TAIL_SIZE_2 = 0.30f;
const auto SPIRIT_TAIL_SIZE_3 = 0.20f;

SpiritEntity::SpiritEntity(Model* model, const EntitySpawnInfo& info)
  : Entity{ model, info }
  , state{ IDLING }
  , desiredPosition{ position }
  , playerPosition{ }
  , tailPosition1{ position + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_1 } // make this use rotation.y
  , tailPosition2{ tailPosition1 + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_2 } // make this use rotation.y
  , tailPosition3{ tailPosition2 + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_3 } // make this use rotation.y
{
  static std::random_device rd;
  static std::mt19937 gen(rd());

  std::uniform_real_distribution<> dis1(0, 2 * 3.1415926535);
  std::uniform_real_distribution<> dis2(0.3, 0.7);

  // TODO: make these pull from a fixed set instead of random
  heightMax    = (float)dis2(gen) / 2.0f;
  heightPeriod = (float)dis2(gen);
  heightPoint  = (float)dis1(gen);
  anglePoint   = (float)dis1(gen);
  distance     = (float)dis2(gen) * 2.5f;
}

void SpiritEntity::update(GameState& state, float time)
{
  { // calculate new idle position
    auto offset = glm::rotateZ(glm::vec3{ 0, distance, std::sin(heightPoint * heightPeriod) * heightMax + 0.25 }, anglePoint);
    auto offsetRatio = glm::length(playerPosition - position) > 2.0f
      ? 1.5f / glm::length(playerPosition - position) + 0.25f
      : 1.0f;

    desiredPosition = playerPosition + offset * offsetRatio;
    heightPoint += 0.01f;
    anglePoint += 0.01f;
  }

  auto oldPosition = position;

  auto dx = desiredPosition.x - position.x;
  auto dy = desiredPosition.y - position.y;
  auto dz = desiredPosition.z - position.z;
  auto az = std::atan2(dy, dx);
  auto ay = std::atan2(dz, std::hypot(dx, dy));

  auto currentAngle = glm::quat(rotation);
  auto desiredAngle = glm::quat(glm::vec3(0,-ay, az));

  // ok, do this later
  //auto requiredAngle = std::acos(glm::dot(currentAngle, desiredAngle));
  //if (requiredAngle > SPIRIT_MIN_ROTATION)
  //{
  //  // min speed
  //}
  //else
  //{
  //  // just do it
  //}

  auto newPosition = oldPosition * (1 - SPIRIT_CORRECTION_RATE) + desiredPosition * SPIRIT_CORRECTION_RATE;

  if (glm::length(playerPosition - newPosition) < SPIRIT_MIN_PLAYER_DISTANCE)
    newPosition = playerPosition + glm::normalize(newPosition - playerPosition) * SPIRIT_MIN_PLAYER_DISTANCE;

  {
    auto dx = newPosition.x - oldPosition.x;
    auto dy = newPosition.y - oldPosition.y;
    auto dz = newPosition.z - oldPosition.z;
    auto az = std::atan2(dy, dx);
    auto ay = std::atan2(dz, std::hypot(dx, dy));

    rotation.x = 0.0f;
    rotation.y =-ay;
    rotation.z = az;

    position = newPosition;

    tailPosition1 = position + glm::normalize(tailPosition1 - position) * SPIRIT_TAIL_DISTANCE_1;
    tailPosition2 = tailPosition1 + glm::normalize(tailPosition2 - tailPosition1) * SPIRIT_TAIL_DISTANCE_2;
    tailPosition3 = tailPosition2 + glm::normalize(tailPosition3 - tailPosition2) * SPIRIT_TAIL_DISTANCE_3;
  }
}


void SpiritEntity::draw_faces(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_faces(program, time, model->makeEntityTransform(position, rotation, scale));
  model->draw_faces(program, time, model->makeEntityTransform(tailPosition1, {}, scale * SPIRIT_TAIL_SIZE_1));
  model->draw_faces(program, time, model->makeEntityTransform(tailPosition2, {}, scale * SPIRIT_TAIL_SIZE_2));
  model->draw_faces(program, time, model->makeEntityTransform(tailPosition3, {}, scale * SPIRIT_TAIL_SIZE_3));
}

void SpiritEntity::draw_lines(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_lines(program, time, model->makeEntityTransform(position, rotation, scale));
  model->draw_lines(program, time, model->makeEntityTransform(tailPosition1, {}, scale * SPIRIT_TAIL_SIZE_1));
  model->draw_lines(program, time, model->makeEntityTransform(tailPosition2, {}, scale * SPIRIT_TAIL_SIZE_2));
  model->draw_lines(program, time, model->makeEntityTransform(tailPosition3, {}, scale * SPIRIT_TAIL_SIZE_3));
}

void SpiritEntity::draw_debug(GameState& state, Program& program, float time)
{

}
