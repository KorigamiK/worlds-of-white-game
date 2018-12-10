#ifndef WILT_GAMESTATE_H
#define WILT_GAMESTATE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

class ICamera;

class GameState
{
public:
  GLFWwindow* window;
  int selectedJoystickId; 
  btCollisionWorld* world;
  btBvhTriangleMeshShape* terrain;
  bool* canJump;
  ICamera* camera;
  glm::vec3 playerPosition;
};

#include "cameras/ICamera.h"

#endif // !WILT_GAMESTATE_H
