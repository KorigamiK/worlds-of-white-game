#ifndef WILT_ENTITYSPAWNINFO_H
#define WILT_ENTITYSPAWNINFO_H

#include <string>

#include <glm/glm.hpp>

class EntitySpawnInfo
{
public:
  std::string name;
  glm::vec3 location;
  glm::vec3 rotation;
  glm::vec3 scale;
};

#endif // !WILT_ENTITYSPAWNINFO_H
