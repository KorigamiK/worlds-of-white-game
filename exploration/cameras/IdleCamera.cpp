#include "IdleCamera.h"

#include <glm/gtx/rotate_vector.hpp>

#include "../entities/Entity.h"

const auto CAMERA_DEADZONE = 0.12f;
const auto CAMERA_ROTATION_RATE = 0.05f;
const auto CAMERA_DISTANCE = 7.0f;
const auto CAMERA_DRIFT_MAX = 0.5f;
const auto CAMERA_DRIFT_CORRECTION_RATE = 0.1f;
const auto CAMERA_LOOK_OFFSET = glm::vec3(0, 0, 1);
const auto CAMERA_HEIGHT_ANGLE = 0.5f;
const auto CAMERA_IDLE_TIME = std::chrono::seconds(3);
const auto CAMERA_IDLE_RATE = 0.001f;

glm::vec3 calcPosition(float xangle, float yangle, float distance);

IdleCamera::IdleCamera(Entity** entity)
  : entity_{ entity }
  , desiredXAngle_{ (*entity)->rotation.z }
  , desiredYAngle_{ CAMERA_HEIGHT_ANGLE }
  , desiredDistance_{ CAMERA_DISTANCE }
  , desiredPosition_{ (*entity)->position + calcPosition(desiredXAngle_, desiredYAngle_, desiredDistance_) }
  , currentPosition_{ desiredPosition_ }
{ }

void IdleCamera::update(GameState& state, float time)
{
  auto now = std::chrono::high_resolution_clock::now();

  { // adjustments based on previous position
    auto dx = (*entity_)->position.x - desiredPosition_.x;
    auto dy = (*entity_)->position.y - desiredPosition_.y;
    desiredXAngle_ = std::atan2(dy, dx) + glm::radians(90.0f);
  }

  { // keyboard
    if (state.input->isKeyHeld(InputManager::KEY_UP))
    {
      desiredDistance_ -= CAMERA_ROTATION_RATE;
      lastInputTime = now;
    }
    if (state.input->isKeyHeld(InputManager::KEY_DOWN))
    {
      desiredDistance_ += CAMERA_ROTATION_RATE;
      lastInputTime = now;
    }
    if (state.input->isKeyHeld(InputManager::KEY_RIGHT))
    {
      desiredXAngle_ += CAMERA_ROTATION_RATE;
      lastInputTime = now;
    }
    if (state.input->isKeyHeld(InputManager::KEY_LEFT))
    {
      desiredXAngle_ -= CAMERA_ROTATION_RATE;
      lastInputTime = now;
    }
  }

  { // controller
    auto cameraXAxis = state.input->getAxis(4);
    auto cameraYAxis = state.input->getAxis(3);

    if (std::abs(cameraXAxis) >= CAMERA_DEADZONE)
    {
      desiredXAngle_ += cameraXAxis * CAMERA_ROTATION_RATE;
      lastInputTime = now;
    }
    if (std::abs(cameraYAxis) >= CAMERA_DEADZONE)
    {
      desiredYAngle_ -= cameraYAxis * CAMERA_ROTATION_RATE;
      lastInputTime = now;
    }
  }

  if (lastInputTime + CAMERA_IDLE_TIME < now)
    desiredXAngle_ += CAMERA_IDLE_RATE;

  desiredYAngle_ = desiredYAngle_ * (1 - CAMERA_DRIFT_CORRECTION_RATE) + CAMERA_HEIGHT_ANGLE * CAMERA_DRIFT_CORRECTION_RATE;
  desiredPosition_ = (*entity_)->position + calcPosition(desiredXAngle_, desiredYAngle_, desiredDistance_);
  currentPosition_ = (currentPosition_) * (1 - CAMERA_DRIFT_CORRECTION_RATE) + (desiredPosition_)* CAMERA_DRIFT_CORRECTION_RATE;
}

glm::mat4 IdleCamera::getTransform() const
{
  return glm::lookAt(getPosition(), (*entity_)->position + CAMERA_LOOK_OFFSET, { 0, 0, 1 });
}

glm::vec3 IdleCamera::getPosition() const
{
  return currentPosition_;
}

glm::vec3 IdleCamera::getDirection() const
{
  return glm::normalize(((*entity_)->position + CAMERA_LOOK_OFFSET) - currentPosition_);
}

float IdleCamera::getAngle() const
{
  return desiredXAngle_;
}

static glm::vec3 calcPosition(float xangle, float yangle, float distance)
{
  auto mat = glm::mat4();
  auto vec = glm::vec4(0, 1, 0, 1);
  mat = glm::rotate(mat, xangle, { 0, 0, 1 });
  mat = glm::rotate(mat, yangle, { 1, 0, 0 });

  return glm::vec3(mat * vec) * distance;
}
