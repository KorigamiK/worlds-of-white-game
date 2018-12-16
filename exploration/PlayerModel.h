#ifndef WILT_PLAYERMODEL_H
#define WILT_PLAYERMODEL_H

#include "Model.h"
#include "instances/PlayerInstance.h"

class PlayerModel : public Model
{
public:
  Instance* spawn(const InstanceSpawnInfo& info) override
  {
    return new PlayerInstance(this, info);
  }

  static Model* read(std::ifstream& file)
  {
    auto model = new PlayerModel();

    // get rotations
    model->transform = glm::scale(glm::mat4(), { 1.0f, 1.0f, 1.0f });

    // read version
    int version;
    file >> version;

    switch (version)
    {
    case 1: Model::readVersion1(*model, file); break;
    case 2: Model::readVersion2(*model, file); break;
    }

    return model;
  }
};

#endif // !WILT_PLAYERMODEL_H
