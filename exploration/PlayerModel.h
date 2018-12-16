#ifndef WILT_PLAYERMODEL_H
#define WILT_PLAYERMODEL_H

#include "Model.h"
#include "entities/PlayerEntity.h"

class PlayerModel : public Model
{
public:
  Entity* spawn(const EntitySpawnInfo& info) override
  {
    return new PlayerEntity(this, info);
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
