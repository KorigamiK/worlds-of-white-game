#ifndef WILT_DECORATIONMODEL_H
#define WILT_DECORATIONMODEL_H

#include "Model.h"
#include "instances/DecorationInstance.h"

class DecorationModel : public Model
{
public:
  Instance* spawn(const InstanceSpawnInfo& info) override
  {
    return new DecorationInstance(this, info);
  }
};

#endif // !WILT_DECORATIONMODEL_H
