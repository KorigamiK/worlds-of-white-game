#ifndef WILT_DECORATIONMODEL_H
#define WILT_DECORATIONMODEL_H

#include "Model.h"
#include "entities/DecorationEntity.h"

class DecorationModel : public Model
{
public:
  Entity* spawn(const EntitySpawnInfo& info) override
  {
    return new DecorationEntity(this, info);
  }

  static Model* read(std::ifstream& file)
  {
    auto model = new DecorationModel();

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

#endif // !WILT_DECORATIONMODEL_H
