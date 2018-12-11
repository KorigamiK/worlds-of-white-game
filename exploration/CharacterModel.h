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

  static Model* read(std::ifstream& file)
  {
    auto model = new CharacterModel();

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

#endif // !WILT_CHARACTERMODEL_H
