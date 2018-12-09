#ifndef WILT_GAMESTATE_H
#define WILT_GAMESTATE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <btBulletDynamicsCommon.h>

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
};

#include "cameras/ICamera.h"

#endif // !WILT_GAMESTATE_H
