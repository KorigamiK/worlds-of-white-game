#ifndef WILT_MODEL_H
#define WILT_MODEL_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "joint.h"
#include "texture.h"
#include "program.h"
#include "IAnimator.h"

constexpr int MAX_JOINTS = 24;

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
  glm::mat4 transform;
  std::vector<Joint> joints;

public:
  void load();

  void draw_faces(Program& program, float time, glm::vec3 position, float rotation, float scale);
  void draw_lines(Program& program, float time, glm::vec3 position, float rotation, float scale);

public:
  static Model read(const std::string& modelPath, float scale = 1.0f);

public:
  static const unsigned int DATA_COUNT_PER_VERTEX = 10;

}; // class Model

#endif // !WILT_MODEL_H
