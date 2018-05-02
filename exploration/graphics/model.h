#ifndef WILT_MODEL_H
#define WILT_MODEL_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "joint.h"
#include "texture.h"
#include "../utilities/narray/narray.hpp"

class Model
{
public:
  std::vector<float> vertexData;
  std::vector<float> faceData;
  std::vector<unsigned int> lineIndexes;
  unsigned int vertexDataVBO;
  unsigned int vertexDataVAO;
  unsigned int faceDataVBO;
  unsigned int faceDataVAO;
  unsigned int lineIndexesID;
  wilt::NArray<unsigned char, 3> textureData;
  Texture texture;
  glm::mat4 transform;
  std::vector<Joint> joints;

public:
  void load();

public:
  static Model read(const std::string& modelPath, const std::string& texturePath, float scale = 1.0f);

}; // class Model

#endif // !WILT_MODEL_H
