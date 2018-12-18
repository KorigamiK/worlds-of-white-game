#ifndef WILT_GAMESTATE_H
#define WILT_GAMESTATE_H

#include <map>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

class ICamera;
class Entity;
class IEntityType;

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
  std::map<std::string, IEntityType*>& types;
  std::vector<Entity*>& entities;
  unsigned int boxVAO;

  std::vector<Entity*> addList;
  std::vector<Entity*> removeList;
};

#include "EntityType.h"
#include "cameras/ICamera.h"
#include "entities/Entity.h"

#endif // !WILT_GAMESTATE_H
