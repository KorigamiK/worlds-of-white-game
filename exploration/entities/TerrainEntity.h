#ifndef WILT_TERRAINENTITY_H
#define WILT_TERRAINENTITY_H

#include "PhysicsEntity.h"

class TerrainEntity : public PhysicsEntity
{
public:
  TerrainEntity(Model* model, const EntitySpawnInfo& info);
};

#endif // !WILT_TERRAINENTITY_H
