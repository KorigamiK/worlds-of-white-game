#ifndef WILT_INSTANCESPAWNINFO_H
#define WILT_INSTANCESPAWNINFO_H

#include <string>

#include <glm/glm.hpp>

class InstanceSpawnInfo
{
public:
  std::string name;
  glm::vec3 location;
  glm::vec3 rotation;
  glm::vec3 scale;
};

#endif // !WILT_INSTANCESPAWNINFO_H
