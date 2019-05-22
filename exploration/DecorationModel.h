#ifndef WILT_DECORATIONMODEL_H
#define WILT_DECORATIONMODEL_H

#include <fstream>

#include "Model.h"
#include "entities/DecorationEntity.h"

class DecorationModel : public Model
{
public:
  float farHideDistance = 8.5f;
  float farDrawDistance = 7.0f;
  float farDrawRate = 1.0f;
  float nearHideDistance = 1.5f;
  float nearDrawDistance = 2.5f;
  float nearDrawRate = 4.0f;

public:
  Entity* spawn(const EntitySpawnInfo& info) override
  {
    return new DecorationEntity(this, info);
  }

  void read(std::ifstream& file)
  {
    // get rotations
    transform = glm::scale(glm::mat4(), { 1.0f, 1.0f, 1.0f });

    // read version
    int version;
    file >> version;

    switch (version)
    {
    case 1:
      Model::readVersion1(*this, file);
      break;

    case 2:
      Model::readVersion2(*this, file);
      break;

    case 3:
      file >> farHideDistance >> farDrawDistance >> farDrawRate;
      file >> nearHideDistance >> nearDrawDistance >> nearDrawRate;
      Model::readVersion2(*this, file);
      break;

    case 4:
      file >> farHideDistance >> farDrawDistance >> farDrawRate;
      file >> nearHideDistance >> nearDrawDistance >> nearDrawRate;
      Model::readVersion3(*this, file);
      break;
    }
  }
};

#endif // !WILT_DECORATIONMODEL_H
