#ifndef WILT_MODEL_H
#define WILT_MODEL_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "joint.h"
#include "texture.h"
#include "program.h"
#include "IAnimator.h"
#include "../utilities/narray/narray.hpp"

class Model
{
public:
  std::vector<float> vertexData;
  std::vector<unsigned int> lineIndexes;
  std::vector<unsigned int> faceIndexes;
  unsigned int vertexDataVBO;
  unsigned int vertexDataVAO;
  unsigned int lineIndexesID;
  unsigned int faceIndexesID;
  wilt::NArray<unsigned char, 3> textureData;
  Texture texture;
  glm::mat4 transform;
  std::vector<Joint> joints;

public:
  void load();

  void draw_faces(Program& program, float time, glm::vec3 position, float rotation, float scale, IAnimator* animator);
  void draw_lines(Program& program, float time, glm::vec3 position, float rotation, float scale, IAnimator* animator);

public:
  static Model read(const std::string& modelPath, const std::string& texturePath, float scale = 1.0f);

}; // class Model

#endif // !WILT_MODEL_H
