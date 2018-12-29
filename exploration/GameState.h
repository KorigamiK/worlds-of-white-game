#ifndef WILT_GAMESTATE_H
#define WILT_GAMESTATE_H

#include <map>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "InputManager.h"

class ICamera;
class Entity;
class IEntityType;

class GameState
{
public:
  InputManager* input;
  btCollisionWorld* world;
  btBvhTriangleMeshShape* terrain;
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
