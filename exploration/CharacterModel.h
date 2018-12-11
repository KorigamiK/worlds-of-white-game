#ifndef WILT_CHARACTERMODEL_H
#define WILT_CHARACTERMODEL_H

#include "Model.h"
#include "instances/CharacterInstance.h"

class CharacterModel : public Model
{
public:
  Instance* spawn(const InstanceSpawnInfo& info) override
  {
    return new CharacterInstance(this, info);
  }
};

#endif // !WILT_CHARACTERMODEL_H
