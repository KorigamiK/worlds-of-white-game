#include "FollowCamera.h"

#include <glm/gtx/rotate_vector.hpp>

#include "../entities/Entity.h"

const auto CAMERA_DEADZONE = 0.12f;
const auto CAMERA_ROTATION_RATE = 0.05f;
const auto CAMERA_DISTANCE = 7.0f;
const auto CAMERA_DRIFT_MAX = 0.5f;
const auto CAMERA_DRIFT_CORRECTION_RATE = 0.1f;
const auto CAMERA_LOOK_OFFSET = glm::vec3(0, 0, 1);
const auto CAMERA_HEIGHT_ANGLE = 0.5f;

glm::vec3 calcPosition(float xangle, float yangle, float distance);

FollowCamera::FollowCamera(Entity** entity)
  : entity_{ entity }
  , desiredXAngle_{ (*entity)->rotation.z }
  , desiredYAngle_{ CAMERA_HEIGHT_ANGLE }
  , desiredDistance_{ CAMERA_DISTANCE }
  , desiredPosition_{ (*entity)->position + calcPosition(desiredXAngle_, desiredYAngle_, desiredDistance_) }
  , currentPosition_{ desiredPosition_ }
{ }

void FollowCamera::update(GameState& state, float time)
{
  { // adjustments based on previous position
    auto dx = (*entity_)->position.x - desiredPosition_.x;
    auto dy = (*entity_)->position.y - desiredPosition_.y;
    desiredXAngle_ = std::atan2(dy, dx) + glm::radians(90.0f);
  }

  { // keyboard
    if (glfwGetKey(state.window, GLFW_KEY_UP) == GLFW_PRESS)
      desiredDistance_ -= CAMERA_ROTATION_RATE;
    if (glfwGetKey(state.window, GLFW_KEY_DOWN) == GLFW_PRESS)
      desiredDistance_ += CAMERA_ROTATION_RATE;
    if (glfwGetKey(state.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      desiredXAngle_ += CAMERA_ROTATION_RATE;
    if (glfwGetKey(state.window, GLFW_KEY_LEFT) == GLFW_PRESS)
      desiredXAngle_ -= CAMERA_ROTATION_RATE;
  }

  { // controller
    auto axesCount = 0;
    auto axes = glfwGetJoystickAxes(state.selectedJoystickId, &axesCount);
    auto cameraXAxis = axes[4];
    auto cameraYAxis = axes[3]; // not used yet
    desiredXAngle_ += std::abs(cameraXAxis) < CAMERA_DEADZONE ? 0.0f : cameraXAxis * CAMERA_ROTATION_RATE;
    desiredYAngle_ -= std::abs(cameraYAxis) < CAMERA_DEADZONE ? 0.0f : cameraYAxis * CAMERA_ROTATION_RATE;
  }

  desiredYAngle_ = desiredYAngle_ * (1 - CAMERA_DRIFT_CORRECTION_RATE) + CAMERA_HEIGHT_ANGLE * CAMERA_DRIFT_CORRECTION_RATE;
  desiredPosition_ = (*entity_)->position + calcPosition(desiredXAngle_, desiredYAngle_, desiredDistance_);
  currentPosition_ = (currentPosition_) * (1 - CAMERA_DRIFT_CORRECTION_RATE) + (desiredPosition_) * CAMERA_DRIFT_CORRECTION_RATE;
}

glm::mat4 FollowCamera::getTransform() const
{
  return glm::lookAt(getPosition(), (*entity_)->position + CAMERA_LOOK_OFFSET, { 0, 0, 1 });
}

glm::vec3 FollowCamera::getPosition() const
{
  return currentPosition_;
}

float FollowCamera::getAngle() const
{
  return desiredXAngle_;
}

glm::vec3 calcPosition(float xangle, float yangle, float distance)
{
  auto mat = glm::mat4();
  auto vec = glm::vec4(0, 1, 0, 1);
  mat = glm::rotate(mat, xangle, { 0, 0, 1 });
  mat = glm::rotate(mat, yangle, { 1, 0, 0 });

  return glm::vec3(mat * vec) * distance;
}
