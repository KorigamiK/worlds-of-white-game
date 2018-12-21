#include "SpiritEntity.h"

#include <iostream>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

const auto SPIRIT_CORRECTION_RATE = 0.04f;
const auto SPIRIT_MIN_SPEED = 0.1f;
const auto SPIRIT_MIN_ROTATION = 0.2f;
const auto SPIRIT_MIN_PLAYER_DISTANCE = 1.0f;
const auto SPIRIT_TAIL_DISTANCE_1 = 0.2f;
const auto SPIRIT_TAIL_DISTANCE_2 = 0.2f;

SpiritEntity::SpiritEntity(Model* model, const EntitySpawnInfo& info)
  : Entity{ model, info }
  , desiredPosition{ position }
  , playerPosition{ }
  , tailPosition1{ position + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_1 } // make this use rotation.y
  , tailPosition2{ tailPosition1 + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_2 } // make this use rotation.y
{ }

void SpiritEntity::update(GameState& state, float time)
{
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
  }
}
