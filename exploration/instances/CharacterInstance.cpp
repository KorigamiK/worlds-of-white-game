#include "CharacterInstance.h"

void CharacterInstance::update(GameState& state, float time)
{
  if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS) {
    velocity = glm::vec3(glm::rotate(glm::mat4(), glm::radians(1.0f), { 0, 0, 1 }) * glm::vec4(velocity, 0.0));
    rotation += glm::radians(1.0f);
  }
  if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS) {
    velocity = glm::vec3(glm::rotate(glm::mat4(), glm::radians(-1.0f), { 0, 0, 1 }) * glm::vec4(velocity, 0.0));
    rotation -= glm::radians(1.0f);
  }

  if (glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS)
  {
    if (glfwGetKey(state.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      speed = 0.003f;
    else
      speed = 0.0005f;
  }
  else
  {
    speed = 0.0f;
  }

  position += velocity * speed;
}
