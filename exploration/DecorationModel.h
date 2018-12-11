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
