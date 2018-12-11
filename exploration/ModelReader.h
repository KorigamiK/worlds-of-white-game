#ifndef WILT_MODELREADER_H
#define WILT_MODELREADER_H

#include <string>
#include <fstream>

#include "Model.h"
#include "DecorationModel.h"
#include "CharacterModel.h"

class ModelReader
{
public:
  static Model* read(std::string filename)
  {
    std::ifstream file(filename);

    // read type
    std::string type;
    file >> type;
    if (type == "character")
      return CharacterModel::read(file);
    if (type == "decoration")
      return DecorationModel::read(file);
    else
      return Model::read(file);
  }
};

#endif // !WILT_MODELREADER_H
