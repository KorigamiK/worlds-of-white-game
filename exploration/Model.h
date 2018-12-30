#ifndef WILT_MODEL_H
#define WILT_MODEL_H

#include <vector>
#include <fstream>

#include <glm/glm.hpp>

#include "EntitySpawnInfo.h"
#include "entities/Entity.h"
#include "graphics/joint.h"
#include "graphics/programs/DepthProgram.h"
#include "graphics/programs/LineProgram.h"

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
  glm::vec3 boundingA = glm::vec3(-1, -1, -1);
  glm::vec3 boundingB = glm::vec3(1, 1, 1);

public:
  void load();

  glm::mat4 makeEntityTransform(glm::vec3 position, glm::vec3 rotation, float scale);

  void draw_faces(DepthProgram& program, float time, glm::mat4 entityTranform);
  void draw_lines(LineProgram& program, float time, glm::mat4 entityTranform);

  virtual Entity* spawn(const EntitySpawnInfo& info);

public:
  static Model* read(std::ifstream& file);
  static void readVersion1(Model& model, std::ifstream& file);
  static void readVersion2(Model& model, std::ifstream& file);
  static void readVersion3(Model& model, std::ifstream& file);

public:
  static const unsigned int DATA_COUNT_PER_VERTEX = 10;

}; // class Model

#endif // !WILT_MODEL_H
