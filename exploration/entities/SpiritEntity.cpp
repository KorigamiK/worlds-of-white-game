#include "SpiritEntity.h"

#include <iostream>

#include <glm/gtx/quaternion.hpp>

const auto SPIRIT_CORRECTION_RATE = 0.04f;
const auto SPIRIT_MIN_SPEED = 0.1f;
const auto SPIRIT_MIN_ROTATION = 0.2f;
const auto SPIRIT_MIN_PLAYER_DISTANCE = 1.0f;

void SpiritEntity::update(GameState& state, float time)
{
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

  position = position * (1 - SPIRIT_CORRECTION_RATE) + desiredPosition * SPIRIT_CORRECTION_RATE;

  if (glm::length(playerPosition - position) < SPIRIT_MIN_PLAYER_DISTANCE)
    position = playerPosition + glm::normalize(position - playerPosition) * SPIRIT_MIN_PLAYER_DISTANCE;

  rotation.y = glm::yaw(desiredAngle);
  rotation.z = glm::roll(desiredAngle);
}
